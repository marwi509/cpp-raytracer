#include <cstdlib>
#include <cmath>
#include <iostream>

#include "Noise3.hpp"

/** Three dimensional perlin noise, implemented according to the
 * 		article "Simplex noise demystified" by Stefan Gustavsson */

using namespace std;

int fastfloor(float f)
{
	return f < 0 ? (int) f-1 : (int) f;
}

Noise3::Noise3(int nw, int nh, int nd, int seed)
{
	noiseWidth = nw;
	noiseHeight = nh;
	noiseDepth = nd;
	srand(seed);
	
	/* Generate the permuations */
	for(int i = 0; i < PERMUT; i ++)
	{
		permutations[i] = abs(rand() % (PERMUT/2));
	}
}

float Noise3::getNoise(float xIn, float yIn, float zIn)
{
	/* Get the relative x,y and z values according to the noise frequency */
	float X = xIn * noiseWidth;
	float Y = yIn * noiseHeight;
	float Z = zIn * noiseDepth;
	
	/* Get the integer index, what "cell" of the noise we're in */
	int x_low = fastfloor(X);
	int y_low = fastfloor(Y);
	int z_low = fastfloor(Z);
	
	/* Get the integer index + 1, so that we have indexes for all the corners of the cell */
	int x_high = x_low + 1;
	int y_high = y_low + 1;
	int z_high = z_low + 1;
	
	/* Get the fractional part of x,y and x */
	float x = X - x_low;
	float y = Y - y_low;
	float z = Z - z_low;
	
	/* Get the gradients using the permutation array */
	const int *grad000 = GRADIENTS[permutations[x_low + permutations[y_low + permutations[z_low]]] % 12];
	const int *grad001 = GRADIENTS[permutations[x_low + permutations[y_low + permutations[z_high]]] % 12];
	
	const int *grad010 = GRADIENTS[permutations[x_low + permutations[y_high + permutations[z_low]]] % 12];
	const int *grad011 = GRADIENTS[permutations[x_low + permutations[y_high + permutations[z_high]]] % 12];
	
	const int *grad100 = GRADIENTS[permutations[x_high + permutations[y_low + permutations[z_low]]] % 12];
	const int *grad101 = GRADIENTS[permutations[x_high + permutations[y_low + permutations[z_high]]] % 12];
	
	const int *grad110 = GRADIENTS[permutations[x_high + permutations[y_high + permutations[z_low]]] % 12];
	const int *grad111 = GRADIENTS[permutations[x_high + permutations[y_high + permutations[z_high]]] % 12];
	
	
	
	float n000= dot(grad000, x, y, z);
    float n100= dot(grad100, x-1, y, z);
    float n010= dot(grad010, x, y-1, z);
    float n110= dot(grad110, x-1, y-1, z);
    float n001= dot(grad001, x, y, z-1);
    float n101= dot(grad101, x-1, y, z-1);
    float n011= dot(grad011, x, y-1, z-1);
    float n111= dot(grad111, x-1, y-1, z-1);
	
	/* Get the interpolation coeffecients */
	float u = fade5(x);
	float v = fade5(y);
	float w = fade5(z);
	
	/* Interpolate the values */
	float nx00 = mix(n000, n100, u);
    float nx01 = mix(n001, n101, u);
    float nx10 = mix(n010, n110, u);
    float nx11 = mix(n011, n111, u);
    
    float nxy0 = mix(nx00, nx10, v);
    float nxy1 = mix(nx01, nx11, v);
    
    float nxyz = mix(nxy0, nxy1, w);
    
    /* Return the resulting coeffecient */
    return nxyz;
}
