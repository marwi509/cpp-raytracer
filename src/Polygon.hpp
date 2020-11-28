
/** 
 * 	Polygon
 * 	Can only be a triangle or a quad
 * 	Doesn't include all information, since its vertices are just ints pointing to the vertex list
 * 
 * */
#ifndef _Polygon_HPP
#define _Polygon_HPP
class Polygon
{
	public:
	
	/* Vertex array */
	int* vertices;
	
	/* How many vertices? */
	int nrVertices;
	
	/* Normal of the polygon */
	Vector3 normal;
	Material* material;

	
	Polygon()
	{
		vertices = NULL;
	}
	
	~Polygon()
	{

	}
};

#endif