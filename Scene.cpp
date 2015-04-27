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
#include "Aperture.hpp"
#include "HdrImage.hpp"
#include "LargeHdrImage.hpp"

using namespace std;

/** Render, isn't used any more. See "renderScenePath" */
bool Scene::renderScene(int width, int height, const char* filename)
{
	/* Abort of something is wrong */
	if(width < 0 || height < 0)
		return false;
	
	bild = new Image(width, height);
	
	cout << "Renderar..." << endl;
	
	//#pragma omp parallel for schedule(dynamic, 1)
	/* Loop through all pixels */
	for(int i = 0; i < height; i ++)
	{
		Vector3 color;
		Vector3 normal, ntemp;
		Material* material;
		Object* objTemp;
		float t;
		bool hit;
		bool light;
		for(int j = 0; j < width; j ++)
		{
			color = Vector3();
			
			/* Antialiasing */
			for(unsigned int m = 0; m < MSAA; m ++)
			{
				for(unsigned int b = 0; b < MSAA; b ++)
				{
					hit = false;
					light = false;
					
					/* Set t to a large float */
					t = 1000000.0f;//1.0f/0.0f;
					
					/* Create a ray */
					Ray R = Ray(i*MSAA+m,j*MSAA+b,height*MSAA,width*MSAA);
					Polygon* PCurr = NULL;
					for(unsigned int k = 0; k < vObj.size(); k ++)
					{

						
						normal = Vector3();
						Polygon* temp;
						temp = vObj[k] -> traverseTree(R, t, normal, vObj[k]->boundingBox, PCurr);
						if(temp != NULL)
						{
							hit = true;
							ntemp = normal;
							material = vObj[k] -> material;
							//color = Vector3(100,100,100);
							objTemp = vObj[k];
							PCurr = temp;
						}
					}
					for(unsigned int k = 0; k < vLight.size(); k ++)
					{
						
						normal = Vector3();
						if(vLight[k] -> traverseTree(R, t, normal, vLight[k]->boundingBox, PCurr))
						{
							hit = true;
							light = true;
							ntemp = normal;
							material = vLight[k] -> material;
						}
					}
					Vector3 dir = (R*t).norm();
					Ray shadow(R*t, Vector3() - dir);
					
					float influ = 1.0f;
					int samples = 0;
					bool hitOcc = false;
					
					if(hit)
					{
						Vector3 tangent = (objTemp -> vertexList[PCurr->vertices[0]].position - objTemp -> vertexList[PCurr->vertices[1]].position).norm();
						for(int i = 0; i < samples; i ++)
						{
							hitOcc = false;
							float tOcc = 1.0f;
							Vector3 dir = getCosWDirection(ntemp, tangent);
							while(dir.dot(PCurr -> normal) < 0.0f)
							{
								dir = getCosWDirection(ntemp, tangent);
							}
							
							Ray rayNew(R*t, dir);
							for(unsigned int k = 0; k < vObj.size(); k ++)
							{
								if(!hitOcc)
								{
									Vector3 normal2 = Vector3();
									if(vObj[k] -> traverseTree(rayNew, tOcc, normal2, vObj[k]->boundingBox, PCurr) != NULL)
									{
										influ -= 1.0f / (float)samples;
										hitOcc = true;
									}
								}
							}
						}
					}
					
					color = color + material -> diffuseColor*30.0f;
					
					//if(shadowInfl)
					//	influ = 0.02f;
					if(hit)
					{
						/* Rendera ljuset lite annorluna än ett vanligt objekt */
						if(!light)
							color = color + material -> diffuseColor*fabs(ntemp.dot(dir))*255.0f*influ;
						if(light)
							color = color + material -> diffuseColor*255.0f*powf(1.0f - fabs(ntemp.dot(dir)), 0.3f)*2.0f;
					}
				}
			}
			/* Sätt in den resulterande pixeln i "bild" */
			bild -> putPixel(j,i, color * (1.0f / (float)(MSAA*MSAA)), 1.8);
		}
		
		
		
		/* Skriv ut hur långt i renderingen den har kommit */
		fprintf(stderr, "\r%f", 100.0f * (float) (i+3) / (float) height);
	}
	printInfo();
	cout << endl << "Skriver till fil" << endl;
	return (bild -> writeImage(filename));
	
}

/** Rendera scenen, och sätt MSAA manuellt */
bool Scene::renderScene(int width, int height, unsigned int MSAA, const char* filename)
{
	this -> MSAA = MSAA;
	return renderScene(width, height, filename);
}


/** A function that returns a cosine weighed direction
 *	The vector is rotated (theta, phi) = (cos⁻¹(sqrt(epsilon_1)), 2pi*epsilon_2)
 * 		around the normal where epsilon_1 and epsilon_2 are random numbers [0 1] */
inline Vector3 getCosWDirection(const Vector3& normal, const Vector3& tangent)
{
	
	float eps1 = randf()*M_PI*2.0f;
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
	
	float x = cosf(eps2 * 2.0f * M_PI)*sqeps1;
	float y = sinf(eps2 * 2.0f * M_PI)*sqeps1;
	float z = eps1;
	
	return tangent * x + normal.cross(tangent) * y + normal * z;
}

/** Return a direction used for non-perfect reflection *
 * 		according to the phong model 
 The vector is rotated (theta, phi) = (cos⁻¹(sqrt_(1/blur)_(epsilon_1)), 2pi*epsilon_2) (where 1/blur = n + 1 in the traditional phong model)
 * 		around the reflected direction where epsilon_1 and epsilon_2 are random numbers [0 1] */
inline Vector3 getCosWPowerDirection(const Vector3& normal, const Vector3& tangent, float blur)
{
	float eps1 = randf()*M_PI*2.0f;
	
	float eps2 = powf(randf(), blur);
	float x = cosf(eps1)*sqrtf(1.0 - eps2*eps2);
	float y = sinf(eps1)*sqrtf(1.0 - eps2*eps2);
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
		<< "Fullscreen multisampling :" << MSAA << "x" << MSAA
		<< endl;
}

/* Get a pixel color with standard path tracing, implicit light paths only */
Vector3 Scene::samplePaths(const int samples, const Vector3& normal, const Vector3& tangent, const Vector3& position, Polygon* PCurr, const Material* material, const Vector3& dirIn, int depth, int& samplesTotal)
{
	/* Abort if the depth is too high (probably means it got stuck somewhere) */
	if(depth > 200)
		return Vector3();
	
	/* The color that will be returned */
	Vector3 colorOut;
	
	/* Loop through all samples */
	for(int i = 0; i < samples; i ++)
	{
		
		/* Sum the reflectance of the material */
		float reflectivity = material -> diffuseColor.sum();
		
		/* Get a random number */
		float decision = randf() * 3.0f;
		
		/* Russian roulette */
		if(reflectivity > decision)
		{
			/* Get a new direction */
			Vector3 direction = getCosWDirection(normal, tangent);
			
			
			int errCheck = 0;
			
			/* If the direction points into the triangle, get a new direction */
			while(direction.dot(PCurr -> normal) < 0.0f)
			{
				direction = getCosWDirection(normal, tangent);
				if(errCheck ++ > 100)
				{
					return Vector3();
				}
			}
			
			/* New ray */
			Ray R(position, direction);
			
			/* normalIn används som utvariabel från traverseTree, ntemp blir den "riktiga" normalen */
			Vector3 normalIn, ntemp;
			
			/* Temporär variabel som används för att lagra materialet från det objekt man träffar */
			Material* materialIn;
			
			/* Temporär variabel som innehåller polygonen man träffar */
			Polygon* pOut;
			
			/* Sätt t till ett stort värde */
			float t = 1e10;
			
			/* hit är sann om man träffat något, light är sann om det man träffat är en ljuskälla */
			bool hit = false;
			bool light = false;
			
			/* Loopa igenom alla objekt */
			for(unsigned int k = 0; k < vObj.size(); k ++)
			{

				Polygon* temp = vObj[k] -> traverseTree(R, t, normalIn, vObj[k] -> boundingBox, PCurr);
				
				/* temp blir null om man inte träffat något */
				if(temp != NULL)
				{
					hit = true;
					ntemp = normalIn;
					materialIn = vObj[k] -> material;
					pOut = temp;
				}
			}
			
			/* Loopa igenom alla ljuskällor, samma sak fast man behöver inte bry sig om att spara polygonen och objektet */
			for(unsigned int k = 0; k < vLight.size(); k ++)
			{
				if(vLight[k] -> traverseTree(R, t, normalIn, vLight[k]->boundingBox, PCurr))
				{
					hit = true;
					light = true;
					ntemp = normalIn;
					materialIn = vLight[k] -> material;
				}
			}
			
			if(hit && !light)
			{
								
				if(R.direction.dot(pOut -> normal) > 0.0f)
				{
					pOut -> normal = Vector3() - pOut -> normal;
					ntemp = Vector3() - ntemp;
				}
				if(R.direction.dot(ntemp) > 0.0f)
				{
					ntemp = Vector3() - ntemp;
				}
				
			}
			
			/* Har den träffat ett ljus, returnera ljusets färg */
			if(light)
			{
				//if(R.direction.y > 0)
				colorOut = colorOut + materialIn -> diffuseColor * material -> diffuseColor;
			}
			
			/* Har den träffat ett objektet, sampla vidare därifrån */
			if(hit && !light)
			{
				//Vector3 tangentIn = (objTemp -> vertexList[pOut->vertices[0]].position - objTemp -> vertexList[pOut->vertices[1]].position).norm();
				Vector3 tangentIn = ntemp.cross(Vector3(randf(), randf(), randf())).norm();
				colorOut = colorOut + (material -> diffuseColor * (3.0f / material -> diffuseColor.sum())) * samplePaths(1, ntemp, tangentIn, R*t, pOut,  materialIn, dirIn, depth + 1, samplesTotal);
			}
			
			
		} // if reflectivity > decision
	} // for i
	
	return colorOut;
}


/** render scene with path tracing */
void Scene::renderScenePath(const RenderSettings& renderSettings, const char* filename)
{
	int c_image = 0;
	cout << "Rendering to filename " << filename << endl;
	int samplesPP = 1;
	
	/* Allocate memory for rgb values (probably a lot bigger than it needs to be) */
	LargeHdrImage largeHdrImage(renderSettings.getWidth(), renderSettings.getHeight(), MSAA);
	printInfo();
	
	/* Render until the user aborts */
	for(;;)
	{
		/* y-led */
		for(int i = 0; i < renderSettings.getHeight(); i ++)
		{
			/* x-led */
			for(int j = 0; j < renderSettings.getWidth(); j ++)
			{
				
				
				int samplesTotal = 0;//samplesPP * MSAA * MSAA;
				
				/* MSAA i y-led */
				for(unsigned int m = 0; m < MSAA; m ++)
				{
					/* MSAA i x-led */
					for(unsigned int b = 0; b < MSAA; b ++)
					{
						/* Temporary variable for color */
						Vector3 color;
						
						/* Create a ray */
						Ray R = Ray(i*MSAA+m,j*MSAA+b,renderSettings.getHeight()*MSAA,renderSettings.getWidth()*MSAA);
						
						
						/* Simulate depth of field */
						Vector3 focal_point = R*renderSettings.getFocalDistance();
						
						int err_check = 0;
						float x_random, y_random;
						do
						{
							x_random = (randf() - randf()) * renderSettings.getApertureSize();
							y_random = (randf() - randf()) * renderSettings.getApertureSize();
						}while(!parametricReject(SQUIRCLE, renderSettings.getApertureSize(), x_random, y_random) 
							&& err_check++ < 100 && renderSettings.getApertureSize() > 0.01f);
						
						Vector3 new_pos = R.position + Vector3(x_random,y_random, 0) ;
						
						// new ray
						R = Ray(new_pos, (focal_point - new_pos).norm());
						
						// material of hit object
						Material* material;
						
						bool hit = false, light = false;
						
						/* Normal used as the returned normal, ntemp is the final normal */
						Vector3 normal, ntemp;
						
						Object* objTemp;
						
						/* Define t as a large number */
						float t = 1e10;
						
						Polygon* PCurr = NULL;
						
						/* Loop through all objects */
						for(unsigned int k = 0; k < vObj.size(); k ++)
						{
							
							Polygon* temp;
							
							/* Traverse the kd-tree */
							temp = vObj[k] -> traverseTree(R, t, normal, vObj[k]->boundingBox, PCurr);
							
							/* Nothing hit? */
							if(temp != NULL)
							{
								hit = true;
								ntemp = normal;
								material = temp -> material;
								objTemp = vObj[k];
								PCurr = temp;
							}
						}
						
						/* Loop through all light sources */
						for(unsigned int k = 0; k < vLight.size(); k ++)
						{
							
							/* A light source hit? */
							if(vLight[k] -> traverseTree(R, t, normal, vLight[k]->boundingBox, PCurr))
							{
								hit = true;
								light = true;
								ntemp = normal;
								material = vLight[k] -> material;
							}
						}
						
						if(hit && !light)
						{
							/* Flip the normal if the backside was hit */
							if(R.direction.dot(PCurr -> normal) > 0.0f)
							{
								PCurr -> normal = Vector3() - PCurr -> normal;
								ntemp = Vector3() - ntemp;
							}
							
							if(R.direction.dot(ntemp) > 0.0f)
							{
								ntemp = Vector3() - ntemp;
							}
						}
						
						/* Start sampling */
						if(!light && hit)
						{
							/* Get a tangent */
							Vector3 tangent = ntemp.cross(Vector3(randf(), randf(), randf())).norm();
							
							/* Sample! */
							color = sampleBiDirPaths(samplesPP, ntemp, tangent, R*t, PCurr, material, R.direction, 0, samplesTotal, *objTemp);
						}
						
						/* If a light source was hit, just return the color of the light */
						if (hit && light)
						{
							color = material -> diffuseColor * (float) samplesPP;
						}
						
						largeHdrImage.addSample(color, j, i, b, m);
					}	//for b
				}	// for m
			}	// for j
			
			/* Print some info */
			fprintf(stderr, "\r%f	Antal samples: %d", 100.0f * 
				(float) (i+1) / (float) renderSettings.getHeight(), c_image * MSAA * MSAA);
			
		} // for i
		
		c_image++;
		largeHdrImage.toHdrImage().bloom(20, 0.2f).toImage().writeImage(filename);
	}
}


/** Sample using path tracing with explicit light paths,
 * TODO: fix proper russian roulette for reflective and refractive, and explicit light paths for reflective */
Vector3 Scene::sampleBiDirPaths(int samples, Vector3 normal, Vector3 tangent, const Vector3& position, const Polygon* PCurr, const Material* material, const Vector3& dirIn, int depth, int& samplesTotal, const Object& obj)
{
	
	
	/* Depth too high probably means it's stuck for some reason */
	if(depth > 20)
		return Vector3();
	
	/* Color vector */
	Vector3 colorOut;
	
	/* Loop through all samples */
	for(int i = 0; i < samples; i ++)
	{
		samplesTotal++;
		Ray R;//(position, direction);
		
		/* Normal used as the returned normal, ntemp is the final normal */
		Vector3 normalIn, ntemp;
		
		Material* materialIn;
		Object* objTemp;
		Polygon* pOut;
		float t = 1e10;
		
		/* Hit is true if an object is hit, light is true if it's a light source */
		bool hit = false;
		bool light = false;
		float reflective = material -> reflective;
	
		Vector3 diffC = material -> diffuseColor;
		
		/* Simple three-dimensional perling noise */
		if(material -> texture || material -> noise != NULL)
		{
			
			Vector3 relativePosition = position - (obj.boundingBox -> position - obj.boundingBox -> radius);
			Vector3 relNormalized = relativePosition * 0.075;
			
			float noise = material -> noise -> getNoise(relNormalized.x, relNormalized.y, relNormalized.z);
					
			diffC = diffC * ((1.0 - material -> noiseStrength)  + material -> noiseStrength * noise);
			
		}
		
		/* Simple chess pattern */
		else if (material -> check)
		{
			float checkweightX = 2.0f*(.5 - fract(material -> checkfreq * position.x + .1f)); 
			float checkweightY = 2.0f*(.5 - fract(material -> checkfreq * position.y + .1f));
			float checkweightZ = 2.0f*(.5 - fract(material -> checkfreq * position.z + .1f));
			
			if(checkweightX <= 0) checkweightX = -1; else checkweightX = 1;
			if(checkweightY <= 0) checkweightY = -1; else checkweightY= 1;
			if(checkweightZ <= 0) checkweightZ = -1; else checkweightZ = 1;
			
			
			float finalweight = 0.0f;
			finalweight = clamp((checkweightX) * (checkweightY) * (checkweightZ) , 0.5, 1.0);
			diffC = diffC * finalweight;
			//if(finalweight < 0.5)
			//	finalweight = 
		}
		
		/* Simple brick wall */
		else if(material -> brick)
		{
			Vector3 lightposition_brick = vLight[0] -> getRandomPoint();
			Vector3 lightdirection_brick = lightposition_brick - position;
			bool what_direction = lightdirection_brick.z > 0.0f;
			float thickness = 0.1f;
			float freq = 1.0f;
			bool odd = 0.5f - fract(position.y  * freq * 0.5f) < 0.0f;
			bool edge = false;
			bool dark = false;
			bool edgy = false;
			
			/* Horizontal cement? */
			if (0.5f - fract(position.y * freq ) > .5 - thickness)
			{
				edge = true;
				edgy = true;
				/* Shadow from the brick? */
				if (0.5f - fract(position.y * freq ) < .5 - thickness * 0.7f)
				{
					dark = true;
					
				}
			}
			
			/* Vertical cement? */
			if(0.5f - fract((odd ? 0.4 : 0.0f) + position.z * freq / 4.0f) > 0.5f - thickness / 4.0f)
			{
				edge = true;
				dark = false;
				/* Shadow from the brick? */
				if(what_direction)
				{
					if (0.5f - fract((odd ? 0.4 : 0.0f) + position.z * freq / 4.0f) < 0.5f - (thickness / 4.0f)*.7f)
					{
						if(!edgy)
						dark = true;
					}
				}
				else
				{
					if (0.5f - fract((odd ? 0.4 : 0.0f) + position.z * freq / 4.0f) > 0.5f - (thickness / 4.0f)*.3f)
					{
						if(!edgy)
						dark = true;
					}
				}
			}
			if(edge == true)
			{
				diffC = Vector3(0.3f, 0.3f, 0.3f);
				if(dark == true)
				{
					diffC = Vector3(0.1f, 0.1f, 0.1f);
				}
			}
				
		}
		
		float reflectiveDecision = randf();
		bool reflected = reflectiveDecision < reflective;
		bool refracted = false;
		
		
			
		
		
		if(!reflected)
		{
			if(reflectiveDecision < reflective + material -> refractive) /* ! */
				refracted = true;
		}
		
		
		
		/* Diffuse? */
		if(!reflected && !refracted)
		{
			/* Sample the light sources (only one right now) */
			for(unsigned int j = 0; j < vLight.size(); j ++)
			{
				
				hit = false;
				
				Vector3 lightpos = vLight[j] -> getRandomPoint();//Vector3(vLight[j] -> translation.x+randf()*3 - randf()*3, vLight[j] -> translation.y, vLight[j] -> translation.z + randf()*3 - randf()*3);
				
				Vector3 lightdir = (lightpos - position);
				
				if(true)
				{
					float lightlength = lightdir.length();
					
					lightdir = lightdir * (1.0f / lightlength);
					
					R = Ray(position, lightdir);
					
					//if(normal.y < 0)
					//	hit = true;
						
					/* Send shadow ray */
					for(unsigned int k = 0; k < vObj.size(); k ++)
					{
						if(!hit)
						{
							Polygon* temp = vObj[k] -> traverseTree(R, lightlength, normalIn, vObj[k] -> boundingBox, PCurr);
							
							
							/* temp blir null om man inte träffat något */
							if(temp != NULL)
							{
								hit = true;
							}
						}
					}
					
					/* Loop through all other light sources */
					if(!hit)
					for(unsigned int k = 0; k < vLight.size(); k ++)
					{
						if(vLight[k] != vLight[j])
						{
							if(vLight[k] -> traverseTree(R, lightlength, normalIn, vLight[k]->boundingBox, PCurr))
							{
								hit = true;
							}
						}
					}
					
					/* No shadow? */
					if(!hit)
					{
						if(lightlength < 1e-3)
							lightlength = 1e-3;
						//if(normal.dot(dirIn) 
						//float weight = (temp -> normal).dot(lightdir);
						float weight = fabsf(lightdir.dot(vLight[j] -> normal));
						colorOut = colorOut + diffC * vLight[j] -> material -> diffuseColor * (1.0f /  (lightlength * lightlength)) * fabs(lightdir.dot(normal)) * vLight[j] -> getArea() * weight;
					}
				}
			}
			
			/* Russian roulette */
			float reflectivity = diffC.sum();
			float decision = randf() * 3.0f;
			if(reflectivity > decision)
			{
			
				
				/* New cosine weighed direction */
				Vector3 direction = getCosWDirection(normal, tangent);
				int errCheck = 0;
				
				/* If the new direction points towards the triangle, get a new one (could happen if shading normals are used) */
				while(direction.dot(PCurr -> normal) < 0.0f)
				{
					direction = getCosWDirection(normal, tangent);
					if(errCheck ++ > 100)
					{
						return Vector3();
					}
				}
				
				R = Ray(position, direction);
				hit = light = false;
				/* Loop through all objects */
				for(unsigned int k = 0; k < vObj.size(); k ++)
				{

					Polygon* temp = vObj[k] -> traverseTree(R, t, normalIn, vObj[k] -> boundingBox, PCurr);
					
					/* temp is null if nothing's hit */
					if(temp != NULL)
					{
						hit = true;
						ntemp = normalIn;
						materialIn = temp -> material;
						objTemp = vObj[k];
						pOut = temp;
					}
				}
				
				/* Loop through all light sources */
				for(unsigned int k = 0; k < vLight.size(); k ++)
				{
					if(vLight[k] -> traverseTree(R, t, normalIn, vLight[k]->boundingBox, PCurr))
					{
						hit = true;
						light = true;
						ntemp = normalIn;
						materialIn = vLight[k] -> material;
					}
				}
				
				if(hit && !light)
				{
									
					if(R.direction.dot(pOut -> normal) > 0.0f)
					{
						pOut -> normal = Vector3() - pOut -> normal;
						ntemp = Vector3() - ntemp;
					}
					if(R.direction.dot(ntemp) > 0.0f)
					{
						ntemp = Vector3() - ntemp;
					}
					
				}
				
				/* No implicit light paths for diffuse surfaces */
				if(light)
				{
					//colorOut = colorOut + diffC * (3.0 / diffC.sum()) * 0.5;
					samples++;
				}
				
				/* If it hits an object, start sampling from there */
				if(hit && !light)
				{
					//Vector3 tangentIn = (objTemp -> vertexList[pOut->vertices[0]].position - objTemp -> vertexList[pOut->vertices[1]].position).norm();
					int samplesOut = 1;
					Vector3 tangentIn = ntemp.cross(Vector3(randf(), randf(), randf())).norm();
					colorOut = colorOut + diffC * (3.0f / (diffC.sum())) * sampleBiDirPaths(samplesOut, ntemp, tangentIn, R*t, pOut,  materialIn, direction, depth + 1, samplesTotal, *objTemp);
				}
				
				
				
			} // if reflectivity > decision
		}
		
		/* Specular reflection */
		else if(reflected)
		{
			if(depth > 7)
				return Vector3();
			
			/* Perfect reflection direction */
			Vector3 direction2 =  (
									Vector3()-
										(((normal * 2.0) * 
										(normal.dot(dirIn)))-
										dirIn))
								.norm();
			
			float sampling_strat = (1.0f - material -> blur * 2.0f);
			float weight_emit = 0.0f;
			
			/* Get direct lighting contribution, is really work in progress, so disabled */
			if(false)
			for(unsigned int j = 0; j < vLight.size(); j ++)
			{
				
				hit = false;
				
				Vector3 lightpos = vLight[j] -> getRandomPoint();
				//Vector3 lightpos = Vector3(vLight[j] -> translation.x+randf()*3 - randf()*3, vLight[j] -> translation.y, vLight[j] -> translation.z + randf()*3 - randf()*3);
				
				Vector3 lightdir = (lightpos - position);
				
				float lightlength = lightdir.length();
					
				lightdir = lightdir * (1.0f / lightlength);
				
				if(lightdir.y > 0 && !hit)
				{
					R = Ray(position, lightdir);
					
					if(normal.y < 0)
						hit = true;
					/* Loop through all objects */
					for(unsigned int k = 0; k < vObj.size(); k ++)
					{
						if(!hit)
						{
							Polygon* temp = vObj[k] -> traverseTree(R, lightlength, normalIn, vObj[k] -> boundingBox, PCurr);
							
							
							/* temp blir null om man inte träffat något */
							if(temp != NULL)
							{
								hit = true;
							}
						}
					}
					
					/* Loopa igenom alla ljuskällor, samma sak fast man behöver inte bry sig om att spara polygonen och objektet */
					if(!hit)
					for(unsigned int k = 0; k < vLight.size(); k ++)
					{
						if(vLight[k] != vLight[j])
						{
							if(vLight[k] -> traverseTree(R, lightlength, normalIn, vLight[k]->boundingBox, PCurr))
							{
								hit = true;
							}
						}
					}
					
					/* ingen skugga? */
					if(!hit)
					{
						if(lightlength < 1e-3)
							lightlength = 1e-3;
						//if(normal.dot(dirIn)
						weight_emit = 
							powf(fabsf(lightdir.dot(direction2)), (1.0f / material -> blur) - 1.0f) * 
							fabsf(lightdir.dot(normal)) *
							(1.0f /  (lightlength * lightlength)) *
							9.0f *
							1.0f ;
						//float pdf_f = 1.0f;
						//float pdf_f_square = pdf_f * pdf_f;
						//float pdf_g = 1.0f;//powf(fabsf(lightdir.dot(direction2)), (1.0f / material -> blur) - 1.0f) * (1.0f /  (lightlength * lightlength)) * fabs(lightdir.y);
						//float weight = pdf_g * pdf_f;
						//Vector3 val = material -> specularColor * vLight[j] -> material -> diffuseColor * (1.0f /  (lightlength * lightlength)) * powf(fabs(lightdir.dot(direction2)), (1.0f / material -> blur) - 1.0f) * 9.0f * fabs(lightdir.y);
						Vector3 val = vLight[j] -> material -> diffuseColor * material -> specularColor;// * weight;
						//val = val;// * pdf_f * 0.5f;// (pdf_f / (pdf_g + pdf_f));
						//if(weight_emit > 1.0f)
							val = val * weight_emit * (1.0f - sampling_strat);
						colorOut = colorOut + val;//material -> specularColor * vLight[j] -> material -> diffuseColor * (1.0f /  (lightlength * lightlength)) * powf(fabs(lightdir.dot(direction2)), (1.0f / material -> blur) - 1.0f) * 9.0f * fabs(lightdir.y) * 0.5f;
					}
				}
			}
			
			/* Get new direction according the phong model */
			Vector3 direction = getCosWPowerDirection(direction2, direction2.cross(Vector3(rand(), rand(), rand()).norm()).norm(), material -> blur);
			
			int err_check = 0;
			/* Check if the new direction points toward the triangle */
			while(direction.dot(PCurr -> normal) < 0.0f && err_check++ < 100)
			{
				/* Get a new direction */
				direction = getCosWPowerDirection(direction2, direction2.cross(Vector3(rand(), rand(), rand()).norm()).norm(), material -> blur);
			}
			
			/* New direction */
			R = Ray(position, direction);
			hit = light = false;
			
			/* Loop through all objects */
			for(unsigned int k = 0; k < vObj.size(); k ++)
			{
				Polygon* temp = vObj[k] -> traverseTree(R, t, normalIn, vObj[k] -> boundingBox, PCurr);
				
				/* temp is null if nothing's hit */
				if(temp != NULL)
				{
					hit = true;
					ntemp = normalIn;
					materialIn = temp -> material;
					objTemp = vObj[k];
					pOut = temp;
				}
			}
			
			/* Loop through all lights */
			for(unsigned int k = 0; k < vLight.size(); k ++)
			{
				if(vLight[k] -> traverseTree(R, t, normalIn, vLight[k]->boundingBox, PCurr))
				{
					hit = true;
					light = true;
					ntemp = normalIn;
					materialIn = vLight[k] -> material;
				}
			}
			
			if(hit && !light)
			{
								
				if(R.direction.dot(pOut -> normal) > 0.0f)
				{
					pOut -> normal = Vector3() - pOut -> normal;
					ntemp = Vector3() - ntemp;
				}
				if(R.direction.dot(ntemp) > 0.0f)
				{
					ntemp = Vector3() - ntemp;
				}
				
			}
			
			/* Did it hit a light? */
			if(light)
			{
				Vector3 val = material -> specularColor * materialIn -> diffuseColor;
				colorOut = colorOut + val * (sampling_strat);
			}
			
			/* If it hit an object, keep sampling from there */
			if(hit && !light)
			{
				int samplesOut = 1;
				Vector3 tangentIn = ntemp.cross(Vector3(randf(), randf(), randf())).norm();
				colorOut = colorOut + material -> specularColor * sampleBiDirPaths(samplesOut, ntemp, tangentIn, R*t, pOut,  materialIn, direction, depth + 1, samplesTotal, *objTemp);
			}
		}
		
		/* Refraction */
		else
		{
			bool into;
			
			/* Does it go into, or out from the object? */
			into = dirIn.dot(normal) < 0.0f;
			
			/* Flip the normal accordingly */
			Vector3 normalPos = into ? normal : Vector3() - normal;
			
			
			double 
				
				/* Refraction index used for air */
				nc=1.0f, 
			
				/* Refraction index of the object */
				nt=material -> refrI, 
				
				nnt=into?nc/nt:nt/nc,
				
				cos2t,
				
				ddn=dirIn.dot(normalPos);
			
			/* Total internal reflection */
			if ((cos2t=1-nnt*nnt*(1-ddn*ddn))<0)
				return Vector3();
			
			/* Direction of the refracted ray */
			Vector3 tDir = (dirIn*nnt - normal*((into?1:-1)*(ddn*nnt+sqrt(cos2t)))).norm();
			
			
			/* Fresnel equations */
			float n1 = into ? 1.0f : nt;
			float n2 = into ? nt : nc;
			float fresnelRoot = sqrtf(1.0f - powf(nnt * normalPos.cross(dirIn).length(), 2));
			float RsCos = fabsf(n1 * -ddn);
			float Rs = powf((RsCos - n2 * fresnelRoot) / (RsCos  + n2 * fresnelRoot), 2);
			
			float RpCos = fabsf(n2 * -ddn);
			float Rp = powf((- RpCos + n1 * fresnelRoot) / (RpCos  + n1 * fresnelRoot), 2);
			
			/* Fresrefract is the probability that the ray is refracted */
			float fresRefract = 1.0f - (Rp*Rp + Rs*Rs) * 0.5f;
			
			Vector3 direction;
			/* Reflect */
			if(randf() > fresRefract)
			{
				Vector3 direction2 = (Vector3()-(((normal * 2.0) * (normal.dot(dirIn)))-dirIn)).norm();
				direction = getCosWPowerDirection(direction2, direction2.cross(Vector3(rand(), rand(), rand()).norm()).norm(), material -> blur);
				int err_check = 0;
				while(direction.dot(PCurr -> normal) < 0.0f && err_check++ < 100)
					direction = getCosWPowerDirection(direction2, direction2.cross(Vector3(rand(), rand(), rand()).norm()).norm(), material -> blur);
			}
			
			/* Refract */
			else
			{
				direction = tDir;
			}
			
			/* New ray */
			R = Ray(position, direction);
			hit = light = false;
			
			/* Loop through all objects */
			for(unsigned int k = 0; k < vObj.size(); k ++)
			{
				Polygon* temp = vObj[k] -> traverseTree(R, t, normalIn, vObj[k] -> boundingBox, PCurr);
				
				/* temp is null if nothing's hit */
				if(temp != NULL)
				{
					hit = true;
					ntemp = normalIn;
					materialIn = temp -> material;
					objTemp = vObj[k];
					pOut = temp;
				}
			}
			
			/* Loop through all lights */
			for(unsigned int k = 0; k < vLight.size(); k ++)
			{
				if(vLight[k] -> traverseTree(R, t, normalIn, vLight[k]->boundingBox, PCurr))
				{
					hit = true;
					light = true;
					ntemp = normalIn;
					materialIn = vLight[k] -> material;
				}
			}
			
			if(hit && !light)
			{
								
				if(R.direction.dot(pOut -> normal) > 0.0f)
				{
					pOut -> normal = Vector3() - pOut -> normal;
					ntemp = Vector3() - ntemp;
				}
				if(R.direction.dot(ntemp) > 0.0f)
				{
					ntemp = Vector3() - ntemp;
				}
				
			}
			
			/* hit light? */
			if(light)
			{
				colorOut = colorOut + material -> specularColor * materialIn -> diffuseColor;
			}
			
			/* hit object? */
			if(hit && !light)
			{
				int samplesOut = 1;
				Vector3 tangentIn = ntemp.cross(Vector3(randf(), randf(), randf())).norm();
				colorOut = colorOut + material -> specularColor * sampleBiDirPaths(samplesOut, ntemp, tangentIn, R*t, pOut,  materialIn, direction, depth + 1, samplesTotal, *objTemp);
			}
		} // if reflected / refracted 
	} // for i
	
	return colorOut;
}

/** An attempt at bidirectional path tracing. Does not work and is not used, so don't bother trying to understand the code. ;) */
Vector3 Scene::sampleBidirectional(int samples, const Vector3& normal, const Vector3& tangent, const Vector3& position, const Polygon* PCurr, const Material* material, const Vector3& dirIn, int depth, int& samplesTotal)
{
	Vector3 color;
	int lightcounter = samples;
	int samplesTotales = 0;
	int light_counter = samples;
	for(int i = 0; i < samples; i ++)
	{
		SurfacePoint* head_eye = new SurfacePoint(position, dirIn, Vector3(), material, PCurr);
		head_eye -> color = material -> diffuseColor * (3.0f / material -> diffuseColor.sum());
		head_eye -> colorpre = material -> diffuseColor;
		head_eye -> normal = head_eye -> poly -> normal;
		if(head_eye -> normal.dot(head_eye -> incomingDirection) > 0)
			head_eye -> normal = Vector3() - head_eye -> normal;
		SurfacePoint* current_eye = head_eye;
		
		Vector3 normalIn;
		int counter = 0;
		int samp = 0;
		//int samplesTotal = 0;
		
		/* Create a path from the eye */
		for(;;)
		{
			//break;
			if(counter++ > 100)
				break;
			if(current_eye -> prev != NULL)
			{
				current_eye -> color = (current_eye -> prev -> color * current_eye -> material -> diffuseColor) * (3.0f / current_eye -> material -> diffuseColor.sum());
				current_eye -> colorpre = (current_eye -> prev -> color * current_eye -> material -> diffuseColor);
			}
			
			float decision = randf();
			
			/* Continue the random walk */
			if(decision * 3.0f < current_eye -> material -> diffuseColor.sum())
			{
				//Vector3 ntemp =  current_eye -> normal;
				
				Vector3 direction = getCosWDirection(current_eye -> normal, current_eye -> normal.cross(Vector3(randf(), randf(), randf())).norm());
				Ray R(current_eye -> position, direction);
				current_eye -> outgoingDirection = direction;
				bool hit = false,  light = false;
				Polygon* pOut;
				float t = 100000.0f;
				
				/* Loopa igenom alla objekt */
				for(unsigned int k = 0; k < vObj.size(); k ++)
				{
					Polygon* temp = vObj[k] -> traverseTree(R, t, normalIn, vObj[k] -> boundingBox, current_eye -> poly);
					
					/* temp blir null om man inte träffat något */
					if(temp != NULL)
					{
						hit = true;
						//objTemp = vObj[k];
						pOut = temp;
					}
				}
				
				/* Loopa igenom alla ljuskällor, samma sak fast man behöver inte bry sig om att spara polygonen och objektet */
				for(unsigned int k = 0; k < vLight.size(); k ++)
				{
					if(vLight[k] -> traverseTree(R, t, normalIn, vLight[k]->boundingBox, current_eye -> poly))
					{
						hit = true;
						light = true;
					}
				}
				
				/* If nothing was hit, terminate the random walk */
				if(!hit)
					break;
				
				/* Continue with the current walk only if no light source was hit TODO */	
				if(!light)
				{
					current_eye -> next = new SurfacePoint(R*t, R.direction, Vector3(), pOut -> material, pOut);
					current_eye -> next -> prev = current_eye;
					
					current_eye = current_eye -> next;
					current_eye -> normal = current_eye -> poly -> normal;
					if(current_eye -> incomingDirection.dot(current_eye -> normal) > 0)
						current_eye -> normal = Vector3() - current_eye -> normal;
				}
			}
			
			/* Terminate the random walk */
			else
			{
				break;
			}
		}
		SurfacePoint* head_light = new SurfacePoint(Vector3(0.1,9.97,12.5) + Vector3(randf()*3.0f - randf()*3.0f, 0.0f ,randf()*3.0f - randf()*3.0f), Vector3(), Vector3(), vLight[0] -> material, & vLight[0] -> triangleList[0]);
		head_light -> color = head_light -> material -> diffuseColor * 9.0f;
		head_light -> colorpre = head_light -> material -> diffuseColor * 9.0f;
		head_light -> normal = Vector3(0,-1,0);
		SurfacePoint* current_light = head_light;
		counter = 0;
		
		/* Create a path from the light source */
		for(;;)
		{
			//break;
			
			//light_counter ++;

			if(current_light -> prev != NULL)
			{
				current_light -> color = (current_light -> prev -> color * current_light -> material -> diffuseColor) * (3.0f / current_light -> material -> diffuseColor.sum());//  * fabsf(current_light -> poly -> normal.dot(current_light -> incomingDirection));
				current_light -> colorpre = (current_light -> prev -> color * current_light -> material -> diffuseColor) * (3.0f / current_light -> material -> diffuseColor.sum());// * fabsf(current_light -> poly -> normal.dot(current_light -> incomingDirection));;
			}
			if(counter++ > 100)
				break;
			
			float decision = randf();
			
			/* Continue the random walk */
			if(head_light == current_light || decision * 3.0f < current_light -> material -> diffuseColor.sum() * fabsf(current_light -> poly -> normal.dot(current_light -> incomingDirection)))// * fabsf(current_light -> poly -> normal.dot(current_light -> incomingDirection)))
			{
				if(head_light != current_light)
					light_counter ++;
				
				//Vector3 ntemp =  current_light -> normal;
				
				Vector3 direction;
				if(head_light == current_light)
					direction = getCosWDirection(current_light -> normal, current_light -> normal.cross(Vector3(randf(), randf(), randf()).norm()));
				else
					direction = getRandomDirection(current_light -> normal, current_light -> normal.cross(Vector3(randf(), randf(), randf()).norm()));
				Ray R(current_light -> position, direction);
				current_light -> outgoingDirection = direction;
				bool hit = false,  light = false;
				Polygon* pOut;
				float t = 100000.0f;
				
				
				/* Loopa igenom alla objekt */
				for(unsigned int k = 0; k < vObj.size(); k ++)
				{
					Polygon* temp = vObj[k] -> traverseTree(R, t, normalIn, vObj[k] -> boundingBox, current_eye -> poly);
					
					/* temp blir null om man inte träffat något */
					if(temp != NULL)
					{
						hit = true;
						//objTemp = vObj[k];
						pOut = temp;
					}
				}
				
				/* Loopa igenom alla ljuskällor, samma sak fast man behöver inte bry sig om att spara polygonen och objektet *//*
				for(unsigned int k = 0; k < vLight.size(); k ++)
				{
					if(vLight[k] -> traverseTree(R, t, normalIn, vLight[k]->boundingBox, current_eye -> poly))
					{
						hit = true;
						light = true;
					}
				}*/
				
				// terminate if nothing was hit
				if(!hit)
					break;
				
				/* Continue with the current walk only if no light source was hit TODO */	
				if(!light)
				{
					current_light -> next = new SurfacePoint(R*t, R.direction, Vector3(), pOut -> material, pOut);
					current_light -> next -> prev = current_light;
					
					current_light = current_light -> next;
					current_light -> normal = current_light -> poly -> normal;
					if(current_light -> incomingDirection.dot(current_light -> normal) > 0)
						current_light -> normal = Vector3() - current_light -> normal;
					lightcounter ++;
				}
			}
			
			/* Terminate the random walk */
			else
			{
				current_light -> prev -> next = NULL;
				delete current_light;
				break;
			}
		}
		/* Start connecting the different vertices of the paths */
		//cout << "So far so good on this one!" << endl;
		
		current_light = head_light;
		counter = 0;
		
		bool light_check = true;
		bool eye_check = true;
		while(light_check)
		{
			eye_check = true;
			samp ++;
			if(counter++ > 1000)
				break;
			current_eye = head_eye;
			while(eye_check)
			{
				
				samplesTotales ++;
				Vector3 light_direction = current_light -> position - current_eye -> position;
				float lightsquare = light_direction.square();
				float lightlength = sqrtf(lightsquare);
				light_direction = light_direction * (1.0f / lightlength);
				Ray R(current_eye -> position, light_direction);
				bool hit = false;
				Vector3 temp_eye = current_eye -> normal, temp_light = current_light -> normal;
				float coeff = 0.0f;
				//if(temp_eye.dot(current_eye -> incomingDirection) > 0)
				//	temp_eye = Vector3() - temp_eye;
				//if(current_light != head_light && temp_light.dot(current_light -> incomingDirection) > 0)
				//	temp_light = Vector3() - temp_light;
				if(!(light_direction.dot(temp_eye) > 0 && light_direction.dot(temp_light) < 0))
					hit = true;
				if(!hit)
					coeff = fabsf(light_direction.dot(current_eye -> normal)) * 
									fabsf(light_direction.dot(current_light -> normal))  * 
									//fabs(light_direction.y) * 
									(1.0f / (lightsquare));
				if(coeff > 1)
					coeff = 1;
				if(coeff < randf() * 0.001f)
					hit = true;
				/* Loopa igenom alla objekt */
				for(unsigned int k = 0; k < vObj.size(); k ++)
				{
					if(!hit)
					{
						Polygon* temp = vObj[k] -> traverseTree(R, lightlength, normalIn, vObj[k] -> boundingBox, current_eye -> poly);
						
						
						/* temp blir null om man inte träffat något */
						if(temp != NULL && temp != current_light -> poly)
						{
							hit = true;
						}
					}
				}
				
				/* Loopa igenom alla ljuskällor, samma sak fast man behöver inte bry sig om att spara polygonen och objektet */
				if(!hit)
				for(unsigned int k = 0; k < vLight.size(); k ++)
				{
					Polygon* temp = vLight[k] -> traverseTree(R, lightlength, normalIn, vLight[k]->boundingBox, current_eye -> poly);
					if(temp != NULL && temp != current_light -> poly)
					{
						hit = false;//true;
					}
				}
				if(!hit)
				{
					if((lightlength < 1e-1))	
					{
						lightsquare = 1e-2;
					}
					
					
					//if(current_light != head_light)
					//if(light_direction.dot(temp_eye) > 0 && light_direction.dot(temp_light) < 0)
					color = color + 
									current_eye -> colorpre * 
									current_light -> colorpre * 
									fabsf(light_direction.dot(current_eye -> normal)) * 
									fabsf(light_direction.dot(current_light -> normal))  * 
									//fabs(light_direction.y) * 
									(1.0f / (lightsquare))
									
									;
					
				}
				current_eye = current_eye -> next;
				if(current_eye == NULL)
					eye_check = false;
			}
			current_light = current_light -> next;
			if(current_light == NULL)
				light_check = false;
		}
		SurfacePoint* temp;
		
		if(head_eye -> next == NULL)
		{
			delete head_eye;
		}
		else
		{
			current_eye = head_eye -> next;
			delete head_eye;
			for(;;)
			{
				temp = current_eye;
				current_eye = temp -> next;
				delete temp;
				if(current_eye == NULL)
					break;
			}
		}
		
		
		if(head_light -> next == NULL)
		{
			delete head_light;
		}
		else
		{
			current_light = head_light -> next;
			delete head_light;
			for(;;)
			{
				temp = current_light;
				current_light = temp -> next;
				delete temp;
				if(current_light == NULL)
					break;
			}
		}
		
	}
	//cout << lightcounter << endl;
	//if(samplesTotales > 0)
		return color * (float) samples * (1.0f / (float)light_counter);;// * 4.0f * (float) samples *  (1.0f / (float)samplesTotales);

}






























