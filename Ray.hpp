#include <iostream>

using namespace std;

/**
 * Ray
 * Includes position and direction
 * */

class Ray
{
	public:
	Vector3
		/* Origin of the ray */
		position,
		
		/* Direction of the ray */
		direction,
		
		/* One divided by the direction, used in the Ray -> Cell calculation */
		directionInv,
		
		/* Current attenuation, isn't used yet */
		color;
		
	/* Current refraction index, should be used but isn't */
	float refractionIndex;
	
	/* Current recursion depth, not used yet either */
	int depth;
	
	/** Empty constructor */
	Ray()
	{
		refractionIndex = 1.0f;
		depth = 0;
		color = Vector3(1,1,1);
	}
	
	/** Konstruktor med riktning och position */
	Ray(const Vector3& p, const Vector3& d)
	{
		/* Tilldela enligt invariablerna */
		position = p;
		direction = d;
		directionInv = Vector3(1.0f / direction.x, 1.0f / direction.y, 1.0f / direction.z); 
		
		/* Standard-värden */
		refractionIndex = 1.0f;
		depth = 0;
		color = Vector3(1,1,1);
	}
	
	/** Get a ray from a pixel position */
	Ray(int i, int j, int height, int width)
	{
		float scale = 10.0f;
		float ar = (float) height / (float) width;
		float x = -1.0f*scale + scale * 2.0f * (float) j / (float) width;
		float y = - ar*scale + scale * 2.0f * ar * (float) i / (float) height;
		
		position = Vector3(x,y,0);
		direction = Vector3(x/scale,y/scale,1).norm();
		directionInv = Vector3(1.0f / direction.x, 1.0f / direction.y, 1.0f / direction.z); 
		color = Vector3();
		
	}
	
	/* Get a position easily */
	Vector3 operator*(float f) const
	{
		return position + direction*f;
	}
	
	
};
