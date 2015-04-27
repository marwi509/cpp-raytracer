#include "Vector3.hpp"
#include <cstdlib>

/** Class for a surface point, for the bidirectional path tracing that currently does not work */

class SurfacePoint
{
	public:
	Vector3
		position,
		incomingDirection,
		outgoingDirection,
		normal;
		
	const Material* material;
	SurfacePoint *prev, *next;
	const Polygon* poly;
	Vector3 color;
	Vector3 colorpre;
	
	
	SurfacePoint()
	{
		prev = next = NULL;
	}
	
	SurfacePoint(const Vector3& pos, const Vector3& inc, const Vector3& out, const Material* mat, const Polygon* p) : 
				position(pos), incomingDirection(inc), outgoingDirection(out), material(mat), poly(p)
	{
		prev = next = NULL;
	}
};
