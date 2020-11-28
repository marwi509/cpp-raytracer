//#include "Object.hpp"

const int CIRCLE = 0;
const int SQUARE = 1;
const int SQUIRCLE = 2;
const int ELIPSOID = 3;

bool rejectCircle(float radius, float x, float y)
{
	return x*x + y*y < radius*radius;
}

bool rejectSquircle(float radius, float x, float y)
{
	return x*x*x*x + y*y*y*y < radius*radius*radius*radius;
}

bool rejectSquare(float radius, float x, float y)
{
	return max(fabsf(x),fabsf(y), -10000.0f) < radius;
}

bool rejectElips(float radius, float x, float y)
{
	return x*x + 4.0f * y * y < radius*radius;
}

bool parametricReject(int variant, float radius, float x, float y)
{
	if(variant == CIRCLE)
		return rejectCircle(radius,x,y);
	if(variant == SQUIRCLE)
		return rejectSquircle(radius,x,y);
	if(variant == SQUARE)
		return rejectSquare(radius,x,y);
	if(variant == ELIPSOID)
		return rejectElips(radius,x,y);
	return false;
}

bool ngonReject(int npol, float radius, float x, float y)
{
	//npol += 2;
	float *xpositions = new float[npol];
	float *ypositions = new float[npol];
	int rrr = 0;
	
	for(float theta2 = 0.0f;  rrr < npol; theta2 += 2.0f * 3.14 / float(npol))
	{
		xpositions[rrr] = radius * cos(theta2);
		ypositions[rrr++] = radius * sin(theta2);
	}
	return (pnpoly(npol, xpositions, ypositions, x, y));
	
}

