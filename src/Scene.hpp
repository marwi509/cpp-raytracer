/**
 * Class that includes the objects, lights, rendering information and functions
 *  to render the scene to an image file
 * */


using namespace std;

class Scene
{
	public:
	
	/* Vectors including the objects and lights
	 * An object in this case is a polygon mesh */
	vector<Object*> vObj;
	vector<Light*> vLight;
	
	/* The luminance values for the pixels, and the amount of samples for each pixel */
	long double* HDRColor;
	long* nrSamples;
	
	/* The image the scene renders to */
	Image* bild;
	
	/* Multisampling, amount of aa-samples per pixel = MSAA*MSAA */
	unsigned int MSAA;
	
	/* empty constructor */
	Scene()
	{
		MSAA = 1;
		bild = NULL;
	}
		
	/* Rendera scenen linjärt */
	bool renderScene(int width, int height, const char* filename);
	
	/* Rendera scenen linjärt, välj MSAA */
	bool renderScene(int width, int height, unsigned int MSAA, const char* filename);
	
	/* Fixa färger med phong-modellen, todo */
	Vector3 getSimpleLighting(const Vector3& normal, const Material& mat, const Vector3& point) const;
	
	/* Display some info about the scene */
	void printInfo() const;
	
	/* Get a pixel color with standard path tracing, implicit light paths only */
	Vector3 samplePaths(const int samples, const Vector3& normal, const Vector3& tangent, const Vector3& position, Polygon* PCurr, const Material* material, const Vector3& dirIn, int depth, int& samplesTotal);
	
	/* Render the scene with monte carlo path tracing
		specifying width, height, file name, how far from the camera the focus point is
		 and how blurry the image will be due to DOF */
	void renderScenePath(const RenderSettings& renderSettings, const char* filename);
	
	/* Get a pixel color using standard path tracing with emitter sampling */
	Vector3 sampleBiDirPaths(const int samples, Vector3 normal, Vector3 tangent, const Vector3& position, const Polygon* PCurr, const Material* material, const Vector3& dirIn, int depth, int& samplesTotal, const Object& obj);
	
	/* Get a pixel color using bidirectional path tracing, currently does not work */
	Vector3 sampleBidirectional(int samples, const Vector3& normal, const Vector3& tangent, const Vector3& position, const Polygon* PCurr, const Material* material, const Vector3& dirIn, int depth, int& samplesTotal);
	
};

/* Get a cosine weighed direction */
Vector3 getCosWDirection(const Vector3& normal, const Vector3& tangent);
