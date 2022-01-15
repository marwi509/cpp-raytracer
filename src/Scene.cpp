/**
 * Class that includes the objects, lights, rendering information and functions
 *  to render the scene to an image file
 * */


#include <vector>
#include <cmath>
#include <iostream>
#include <cstdio>
#include <cstdlib>

#include "Vector3.hpp"
#include "Ray.hpp"
#include "Image.hpp"
#include "Vertex.hpp"
#include "Material.hpp"
#include "Polygon.hpp"
#include "Cell.hpp"
#include "Object.hpp"
#include "Light.hpp"
#include "RenderSettings.hpp"
#include "Scene.hpp"
#include "Perlin.cpp"
#include "Check.hpp"
#include "SurfacePoint.hpp"
#include "Noise3.hpp"
#include "HdrImage.hpp"
#include "LargeHdrImage.hpp"


using namespace std;



/** A function that returns a cosine weighed direction
 *	The vector is rotated (theta, phi) = (cos⁻¹(sqrt(epsilon_1)), 2pi*epsilon_2)
 * 		around the normal where epsilon_1 and epsilon_2 are random numbers [0 1] */
inline Vector3 getCosWDirection(const Vector3& normal, const Vector3& tangent)
{
	
	float eps1 = randf()*3.14f*2.0f;
	float eps2 = sqrtf(randf());
	
	float x = cosf(eps1)*eps2;
	float y = sinf(eps1)*eps2;
	float z = sqrtf(1.0f - eps2*eps2);
	
	return tangent * x + normal.cross(tangent) * y + normal * z;
}

/** Get a uniform random direction */
inline Vector3 getRandomDirection(const Vector3& normal, const Vector3& tangent)
{
	float eps1 = randf();
	float eps2 = randf();
	float sqeps1 = sqrtf(1.0f - eps1*eps1);
	
	float x = cosf(eps2 * 2.0f * 3.14f)*sqeps1;
	float y = sinf(eps2 * 2.0f * 3.14f)*sqeps1;
	float z = eps1;
	
	return tangent * x + normal.cross(tangent) * y + normal * z;
}

inline Vector3 getRandomDirection() {
	return Vector3(randf(), randf(), randf()).norm();
}

/** Return a direction used for non-perfect reflection *
 * 		according to the phong model 
 The vector is rotated (theta, phi) = (cos⁻¹(sqrt_(1/blur)_(epsilon_1)), 2pi*epsilon_2) (where 1/blur = n + 1 in the traditional phong model)
 * 		around the reflected direction where epsilon_1 and epsilon_2 are random numbers [0 1] */
inline Vector3 getCosWPowerDirection(const Vector3& normal, const Vector3& tangent, float blur)
{
	float eps1 = randf()*3.14f*2.0f;
	
	float eps2 = powf(randf(), blur);
	float x = cosf(eps1)*sqrtf(1.0f - eps2*eps2);
	float y = sinf(eps1)*sqrtf(1.0f - eps2*eps2);
	float z = eps2;
	
	return (tangent * x + normal.cross(tangent) * y + normal * z).norm();
}

/* Display some info about the scene */
void Scene::printInfo() const
{
	int sumTriangles = 0;
	int sumTriTests = 0;
	for(unsigned int i = 0; i < vObj.size(); i ++)
	{
		sumTriangles += vObj[i] -> nrTriangles;
		sumTriTests += (vObj[i] -> nrTriTests);
	}
	for(unsigned int i = 0; i < vLight.size(); i ++)
	{
		sumTriangles += vLight[i] -> nrTriangles;
	}
	
	cout 
		<< endl 
		<< endl 
		<< "Totalt antal trianglar: " << sumTriangles
		<< endl
		//<< "Fullscreen multisampling :" << MSAA << "x" << MSAA
		<< endl;
}
vector<HdrImage> imageStack = vector<HdrImage>();
void Scene::renderSceneNew(const RenderSettings& renderSettings, bool open) {
	int c_image = 0;
	int samplesPP = 1;

	/* Allocate memory for rgb values (probably a lot bigger than it needs to be) */
	LargeHdrImage largeHdrImage(renderSettings.getWidth(), renderSettings.getHeight(), renderSettings.getMsaa());
	printInfo();

	int MSAA = renderSettings.getMsaa();
	
	/* Render until the user aborts */
	for (int pass = 0; pass < renderSettings.getPasses(); pass++)
	{
		//largeHdrImage.clearBuffers();
		//renderSettings.getCamera()->position = renderSettings.getCamera()->position - Vector3(0, 0.1, 0);
		int currentIndex = 0;
		int rowsRendered = 0;

#pragma omp parallel
		/* y-led */
		while (currentIndex < renderSettings.getHeight())
		{
			int index;
#pragma omp critical
			index = currentIndex++;
			/* x-led */
			for (int j = 0; j < renderSettings.getWidth(); j++)
			{
				/* MSAA i y-led */
				for (unsigned int m = 0; m < MSAA; m++)
				{
					/* MSAA i x-led */
					for (unsigned int b = 0; b < MSAA; b++)
					{
						/* Create a ray */
						Ray ray = renderSettings.getCamera()->getRay(index * MSAA + m, j * MSAA + b);
						largeHdrImage.addSample(sample(ray, 0, false, open), j, index, b, m);

					}	//for b
				}	// for m
			}	// for j
			rowsRendered++;
			/* Print some info */
			fprintf(stderr, "\r%f	Antal samples: %d", 100.0f *
				(float)(rowsRendered) / (float)renderSettings.getHeight(), c_image * MSAA * MSAA);

		} // for i

		c_image++;
		
	}
	HdrImage currentImage = largeHdrImage
		.toHdrImage();
	imageStack.push_back(currentImage);
	if (imageStack.size() == renderSettings.getFrameSmooth() + 1) {
		imageStack.erase(imageStack.begin());
		HdrImage resultImage = imageStack.at(0);
		for (int i = 1; i < renderSettings.getFrameSmooth(); i++) {
			resultImage = resultImage + imageStack.at(i);
		}
		resultImage = resultImage * (1.0f / renderSettings.getFrameSmooth());
		resultImage
			//.toHdrImage()
			.bloom(20, 0.2f)
			.toImage()
			.savebmp(renderSettings.getFileName());
	}
	
	
}

Vector3 Scene::sample(Ray& ray, int errorCheck, bool diffuse, bool open)
{
	if (errorCheck > 30) {
		return Vector3();
	}

	SurfacePoint* surfacePoint = castRay(ray);
	if (surfacePoint == NULL) {
		return open ? Vector3(1,1,1) : Vector3(1,1,1);
	}

	const Material* material = surfacePoint->material;
	Vector3 position = surfacePoint->position;
	Vector3 normal = surfacePoint->normal;
	

	if (surfacePoint->light) {
		delete surfacePoint;
		return (diffuse && !open) ? Vector3() : Vector3(material->diffuseColor);
	}
	else {
		float compensationDiffuse = 1.0f;
		float compensationReflective = 1.0f;
		bool isPorcelain = !open && (material->reflective > 0.05f && material->reflective < 0.2f && material->refractive == 0.0f);
		bool shouldReflectOverride = false;

		if (isPorcelain) {
			float reflectProb = 0.9f;

			compensationDiffuse = (1.0f - material->reflective) / (1.0f - reflectProb);
			compensationReflective = material->reflective / reflectProb;
			if (randf() < reflectProb) {
				shouldReflectOverride = true;
			}
		}

		float rouletteDecisionNumber = randf();
		float diffuseCutoff = surfacePoint->material->reflective + surfacePoint->material->refractive;
		
		if (!shouldReflectOverride && rouletteDecisionNumber >= diffuseCutoff) {
			Vector3 diffuseColor = getDiffuseColor(surfacePoint);
			Vector3 explicitContribution = !open ? diffuseColor * sampleExplicitLightPaths(*surfacePoint) * compensationDiffuse : Vector3();
			float reflectivity = diffuseColor.sum();
			float decision = randf() * 3.0f;
			
			if (reflectivity > decision) {
				Vector3 tangent = normal.cross(Vector3(randf(), randf(), randf()).norm());
				Vector3 newDirection = getCosWDirection(normal, tangent);
				delete surfacePoint;
				return explicitContribution + diffuseColor * (3.0f / reflectivity) * sample(Ray(position, newDirection), errorCheck+1, true, open) * compensationDiffuse;

			}
			delete surfacePoint;
			return explicitContribution;
		}
		else if (rouletteDecisionNumber < material->refractive) {
			Vector3 newDirection = getRefractedDirection(surfacePoint);
			
			if (surfacePoint->isInto()) {
				delete surfacePoint;
				return material->specularColor * sample(Ray(position, newDirection), errorCheck + 1, false, open);
			}
			else {
				delete surfacePoint;
				return material->specularColor * sample(Ray(position, newDirection), errorCheck + 1, false, open);
			}
		}
		else {
			int err_check = 0;
			Vector3 dirIn = Vector3() - surfacePoint->incomingDirection;
			Vector3 direction2 = (
				(((surfacePoint->normal * 2.0) *
				(surfacePoint->normal.dot(dirIn))) -
					dirIn))
				.norm();
			Vector3 newDirection = getCosWPowerDirection(direction2, direction2.cross(Vector3(rand(), rand(), rand()).norm()).norm(), surfacePoint->material->blur);				
			while (newDirection.dot(surfacePoint->normal) < 0.0f && err_check++ < 100)
			{
				newDirection = getCosWPowerDirection(direction2, direction2.cross(Vector3(rand(), rand(), rand()).norm()).norm(), surfacePoint->material->blur);
			}
			delete surfacePoint;
			return material->specularColor*sample(Ray(position, newDirection), errorCheck+1, false, open) * compensationReflective;
		}
	}
	delete surfacePoint;
	return Vector3(1,1,1)* 1000;
}

Vector3 Scene::getDiffuseColor(const SurfacePoint* surfacePoint) {
	const Material* material = surfacePoint->material;
	const Vector3& position = surfacePoint->position;
	if (material->texture || material->noise != NULL)
	{
		Vector3 relativePosition = surfacePoint -> position - (surfacePoint->object->boundingBox->position - surfacePoint->object->boundingBox->radius);
		Vector3 relNormalized = relativePosition * 0.025;

		float noise = material->noise->getNoise(relNormalized.x, relNormalized.y, relNormalized.z);

		return surfacePoint->material->diffuseColor * ((1.0 - material->noiseStrength) + material->noiseStrength * noise);
	}

	/* Simple chess pattern */
	else if (material->check)
	{
		return getCheckPatternColor(material, position, surfacePoint);
	}

	/* Simple brick wall */
	else if (material->brick)
	{
		return getBrickColor(position, surfacePoint);

	}
	return surfacePoint->material->diffuseColor;
}

const Vector3& Scene::getCheckPatternColor(const Material* material, const Vector3& position, const SurfacePoint* surfacePoint)
{
	float checkweightX = 2.0f * (.5 - fract(material->checkfreq * position.x + .1f));
	float checkweightY = 2.0f * (.5 - fract(material->checkfreq * position.y + .1f));
	float checkweightZ = 2.0f * (.5 - fract(material->checkfreq * position.z + .1f));

	if (checkweightX <= 0) checkweightX = -1; else checkweightX = 1;
	if (checkweightY <= 0) checkweightY = -1; else checkweightY = 1;
	if (checkweightZ <= 0) checkweightZ = -1; else checkweightZ = 1;


	float finalweight = 0.0f;
	finalweight = clamp((checkweightX) * (checkweightY) * (checkweightZ), 0.5, 1.0);
	return surfacePoint->material->diffuseColor * finalweight;
}

const Vector3& Scene::getBrickColor(const Vector3& position, const SurfacePoint* surfacePoint)
{
	Vector3 lightposition_brick = vLight[0]->getRandomPoint();
	Vector3 lightdirection_brick = lightposition_brick - position;
	bool what_direction = lightdirection_brick.z > 0.0f;
	float thickness = 0.1f;
	float freq = 1.0f;
	bool odd = 0.5f - fract(position.y * freq * 0.5f) < 0.0f;
	bool edge = false;
	bool dark = false;
	bool edgy = false;

	/* Horizontal cement? */
	if (0.5f - fract(position.y * freq) > .5 - thickness)
	{
		edge = true;
		edgy = true;
		/* Shadow from the brick? */
		if (0.5f - fract(position.y * freq) < .5 - thickness * 0.7f)
		{
			dark = true;

		}
	}

	/* Vertical cement? */
	if (0.5f - fract((odd ? 0.4 : 0.0f) + position.z * freq / 4.0f) > 0.5f - thickness / 4.0f)
	{
		edge = true;
		dark = false;
		/* Shadow from the brick? */
		if (what_direction)
		{
			if (0.5f - fract((odd ? 0.4 : 0.0f) + position.z * freq / 4.0f) < 0.5f - (thickness / 4.0f) * .7f)
			{
				if (!edgy)
					dark = true;
			}
		}
		else
		{
			if (0.5f - fract((odd ? 0.4 : 0.0f) + position.z * freq / 4.0f) > 0.5f - (thickness / 4.0f) * .3f)
			{
				if (!edgy)
					dark = true;
			}
		}
	}
	if (edge == true)
	{
		return Vector3(0.3f, 0.3f, 0.3f);
		if (dark == true)
		{
			return Vector3(0.1f, 0.1f, 0.1f);
		}
	}
	else {
		return surfacePoint->material->diffuseColor;
	}
}

Vector3 Scene::getRefractedDirection(const SurfacePoint* surfacePoint) {
	bool into = surfacePoint->incomingDirection.dot(surfacePoint->normal) < 0.0f;

	/* Flip the normal accordingly */
	Vector3 normalPos = into ? surfacePoint->normal : Vector3() - surfacePoint->normal;

	double

		/* Refraction index used for air */
		nc = 1.0f,

		/* Refraction index of the object */
		nt = surfacePoint->material->refrI,

		nnt = into ? nc / nt : nt / nc,

		cos2t,

		ddn = surfacePoint->incomingDirection.dot(normalPos);

	/* Total internal reflection */
	if ((cos2t = 1 - nnt*nnt*(1 - ddn*ddn))<0)
		return Vector3();

	/* Direction of the refracted ray */
	Vector3 tDir = (surfacePoint->incomingDirection*nnt - surfacePoint->normal*((into ? 1 : -1)*(ddn*nnt + sqrt(cos2t)))).norm();


	/* Fresnel equations */
	float n1 = into ? 1.0f : nt;
	float n2 = into ? nt : nc;
	float fresnelRoot = sqrtf(1.0f - powf(nnt * normalPos.cross(surfacePoint->incomingDirection).length(), 2));
	float RsCos = fabsf(n1 * -ddn);
	float Rs = powf((RsCos - n2 * fresnelRoot) / (RsCos + n2 * fresnelRoot), 2);

	float RpCos = fabsf(n2 * -ddn);
	float Rp = powf((-RpCos + n1 * fresnelRoot) / (RpCos + n1 * fresnelRoot), 2);

	/* Fresrefract is the probability that the ray is refracted */
	float fresRefract = 1.0f - (Rp*Rp + Rs*Rs) * 0.5f;
	/* Reflect */
	if (randf() > fresRefract) {
		Vector3 direction2 = ((((normalPos * 2.0) * normalPos.dot(surfacePoint->incomingDirection))) - surfacePoint->incomingDirection).norm();
		return getCosWPowerDirection(direction2, direction2.cross(Vector3(rand(), rand(), rand()).norm()).norm(), surfacePoint->material->blur);
	}
	else {
		return tDir;
	}
}

template<typename Base, typename T>
inline bool instanceof(const T*) {
	return is_base_of<Base, T>::value;
}

Vector3 Scene::sampleExplicitLightPaths(const SurfacePoint& surfacePoint) {
	Vector3 resultingColor = Vector3();

	for (int j = 0; j < vLight.size(); j++)
	{
		Vector3 lightpos = vLight[j]->getRandomPoint();

		Vector3 lightdir = (lightpos - surfacePoint.position);

		float lightlength = lightdir.length();

		lightdir = lightdir * (1.0f / lightlength);

		Ray R = Ray(surfacePoint.position, lightdir);

		SurfacePoint* shadowHit = castRay(R, true, lightlength);
		if (shadowHit == NULL) {
			if (lightlength < 1e-3)
				lightlength = 1e-3;
			float weight = fabsf(lightdir.dot(vLight[j]->normal));
			resultingColor = resultingColor + vLight[j]->material->diffuseColor * (1.0f / (lightlength * lightlength)) * fabs(lightdir.dot(surfacePoint.normal)) * vLight[j]->getArea() * weight;
		}
		delete shadowHit;

	}
	return resultingColor;
}


SurfacePoint* Scene::castRay(Ray& ray) {
	return castRay(ray, false, 1e10);
}

SurfacePoint* Scene::castRay(Ray& ray, bool shadowRay, float t) {

	Vector3 normal = Vector3();
	Polygon* polygon = NULL;
	bool hit = false, light = false;
	Vector3 ntemp = normal;
	Material* material;
	Object* objTemp;
	Polygon* PCurr = NULL;
	int lightIndex = -1;
	/* Loop through all objects */
	for (int k = 0; k < vObj.size(); k++)
	{

		Polygon* temp;

		/* Traverse the kd-tree */
		temp = vObj[k]->traverseTree(ray, t, normal, vObj[k]->boundingBox, PCurr);

		/* Nothing hit? */
		if (temp != NULL)
		{
			hit = true;
			ntemp = normal;
			material = temp->material;
			objTemp = vObj[k];
			PCurr = temp; 

			if (shadowRay) {
				/* Flip the normal if the backside was hit */
				if (ray.direction.dot(PCurr->normal) > 0.0f)
				{
					PCurr->normal = Vector3() - PCurr->normal;
					ntemp = Vector3() - ntemp;
				}

				if (ray.direction.dot(ntemp) > 0.0f)
				{
					ntemp = Vector3() - ntemp;
				}
				/* Get a tangent */
				Vector3 tangent = ntemp.cross(Vector3(randf(), randf(), randf()).norm()).norm();
				SurfacePoint* sp = new SurfacePoint(ray * t, ntemp, ray.direction, material, polygon, objTemp, false);
				sp->lightIndex = lightIndex;
				return sp;
			}
		}
	}

	if (!shadowRay)
	/* Loop through all light sources */
	for (int k = 0; k < vLight.size(); k++)
	{

		/* A light source hit? */
		if (vLight[k]->traverseTree(ray, t, normal, vLight[k]->boundingBox, PCurr))
		{
			hit = true;
			light = true;
			ntemp = normal;
			objTemp = vLight[k];
			material = vLight[k]->material;
			lightIndex = k;
		}
	}

	/* Start sampling */
	if (!light && hit)
	{
		/* Flip the normal if the backside was hit */
		if (ray.direction.dot(PCurr->normal) > 0.0f)
		{
			PCurr->normal = Vector3() - PCurr->normal;
			ntemp = Vector3() - ntemp;
		}

		if (ray.direction.dot(ntemp) > 0.0f)
		{
			ntemp = Vector3() - ntemp;
		}
		/* Get a tangent */
		Vector3 tangent = ntemp.cross(Vector3(randf(), randf(), randf()).norm()).norm();
		SurfacePoint* sp = new SurfacePoint(ray*t, ntemp, ray.direction, material, polygon, objTemp, false);
		sp->lightIndex = lightIndex;
		return sp;
	}

	/* If a light source was hit, just return the color of the light */
	if (hit && light)
	{
		return SurfacePoint::LIGHT(material);
	}

	return NULL;
	
}































