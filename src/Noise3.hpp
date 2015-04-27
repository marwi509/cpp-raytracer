#ifndef NOISE_HPP
#define NOISE_HPP

/** Three dimensional perlin noise, implemented according to the
 * 		article "Simplex noise demystified" by Stefan Gustavsson */

/* How many permutations are used */
const int PERMUT = 1024;

/* The gradients used */
const int GRADIENTS[12][3]  = {{1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},
                                 {1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},
                                 {0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1}};

class Noise3
{
	/* The noise frequency in all dimensions */
	int noiseWidth, noiseHeight, noiseDepth;
	int permutations[PERMUT];
	
	public:
	/* Constructor that generates the permutations */
	Noise3(int nw, int nh, int nd, int seed);
	
	/* Get a noise value for coordinates x,y,z */
	float getNoise(float x, float y, float z);
	
	/* Different interpolation methods, fade5 works best */
	inline float fade5(float t){  return t*t*t*(t*(t*6-15)+10); }
	inline float fade3(float t){  return (3 - 2*t)*t*t; }
	
	/* Dot product for float coordinates and integer vector */
	inline float dot(const int* vec, float x, float y, float z)
	{	return vec[0] * x + vec[1] * y + vec[2] * z;	}
	
	/* Interpolate with a given interpolation value */
	inline float mix(float a, float b, float t){	return (1 - t) * a + t * b; }
};
#endif
