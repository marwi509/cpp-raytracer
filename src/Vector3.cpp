#include "Vector3.hpp"
#include <cstdlib>

/** Return a random float [0 1], probably shouldn't be in this file */
float randf()
{
	return (float)rand() / (float)RAND_MAX;
}

float max(float f1, float f2, float f3)
{
	if(f1 >= f2 && f1 >= f3)
		return f1;
	if(f2 >= f1 && f2 >= f3)
		return f2;
	return f3;
}

float clamp(float value, float min, float max)
{
	if(value > max)
		return max;
	if(value < min)
		return min;
	return value;
}

float fract(float value)
{
	return value > 0.0f ? value - floorf(value) : value - (floorf(value));
}

float step(float value, float limit)
{
	if(value < limit) return 0.0f;
	return 1.0f;
}






