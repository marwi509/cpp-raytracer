/**
 * Class that includes the objects, lights, rendering information and functions
 *  to render the scene to an image file
 * */

#include "SurfacePoint.hpp"
#include "Vector2.hpp"

using namespace std;

class Scene
{
	public:
	
	/* Vectors including the objects and lights
	 * An object in this case is a polygon mesh */
	vector<Object*> vObj;
	vector<Light*> vLight;

	/* Display some info about the scene */
	void printInfo() const;	
	
	/* The luminance values for the pixels, and the amount of samples for each pixel */
	long double* HDRColor;
	long* nrSamples;
	
	/* The image the scene renders to */
	Image* bild;
	
	/* empty constructor */
	Scene()
	{
		bild = NULL;
	}

	SurfacePoint* castRay(Ray& ray);

	SurfacePoint* castRay(Ray& ray, bool shadowRay, float t);

	Vector3 sample(Ray& castRay, int errorCheck, bool diffuse, bool open);

	Vector3 getRefractedDirection(const SurfacePoint* surfacePoint);

	void renderSceneNew(const RenderSettings& renderSettings, bool open);

	Vector3 sampleExplicitLightPaths(const SurfacePoint& surfacePoint);

	Vector3 getDiffuseColor(const SurfacePoint* surfacePoint);
private:
	const Vector3& getCheckPatternColor(const Material* material, const Vector3& position, const SurfacePoint* surfacePoint);

	const Vector3& getBrickColor(const Vector3& position, const SurfacePoint* surfacePoint);
	
};

/* Get a cosine weighed direction */
Vector3 getCosWDirection(const Vector3& normal, const Vector3& tangent);
