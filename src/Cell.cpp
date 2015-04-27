#include <cstdlib>
#include <vector>
#include <iostream>

#include "Vector3.hpp"
#include "Ray.hpp"
#include "Vertex.hpp"
#include "Material.hpp"
#include "Polygon.hpp"
#include "Cell.hpp"

/** Function that checks if a polygon is contained by a cell (only triangles and quads)
	 * */
inline bool Cell::contains(Vertex** v, int nrVertices, const Vector3& normal)
{

	/* Only quads and triangles are supported (and quads kinda don't work) */
	if(nrVertices != 3 && nrVertices != 4)
		return false;
	
	/* For quads, do two tests (doesn't work that well for some reason) */
	if(nrVertices == 4)
	{
		Vertex** vFirst = new Vertex*[3];
		vFirst[0] = v[0];
		vFirst[1] = v[1];
		vFirst[2] = v[2];
		
		Vertex** vSecond = new Vertex*[3];
		vSecond[0] = v[0];
		vSecond[1] = v[3];
		vSecond[2] = v[2];
		bool hit = contains(vFirst, 3, normal) || contains(vSecond, 3, normal);
		delete vSecond;
		delete vFirst;
		
		return hit;
	}
	
	/* The array verts contains the vertex positions */
	Vector3 verts[3] = {v[0]->position, v[1]->position, v[2]->position};
	
	/* The array v contains the vectors between the vertex positions and the position of the box */
	Vector3 direct[3] = {verts[0] - position, verts[1] - position, verts[2] - position};
	
	/** Test if the vertex closes to the box in each coordinate is farther away
	 * 	than the radius on that coordinate. In that case the triangel can't be contained by the cell */
	/* test in X-direction */
	float min, max;
	min_max(direct[0].x,direct[1].x,direct[2].x,min,max);
	if(min>radius.x || max<-radius.x) return false;

   /* test in Y-direction */
	min_max(direct[0].y,direct[1].y,direct[2].y,min,max);
	if(min>radius.y || max<-radius.y) return false;

   /* test in Z-direction */
	min_max(direct[0].z,direct[1].z,direct[2].z,min,max);
	if(min>radius.z || max<-radius.z) return false;
	
	float d=-normal.dot(direct[0]);
	
	Vector3 vmin,vmax;
	
	/* Control if the members of the normal are negative, change sign in that case */
	if(normal.x>0.0f)
	{
		vmin.x=-radius.x;
		vmax.x=radius.x;
	}
	else
	{
		vmin.x=radius.x;
		vmax.x=-radius.x;
	}
	if(normal.y>0.0f)
	{
		vmin.y=-radius.y;
		vmax.y=radius.y;
	}
	else
	{
		vmin.y=radius.y;
		vmax.y=-radius.y;
	}
	if(normal.z>0.0f)
	{
		vmin.z=-radius.z;
		vmax.z=radius.z;
	}
	else
	{
		vmin.z=radius.z;
		vmax.z=-radius.z;
	}
	
	
	if(normal.dot(vmin) + d > 0.0f) return false;
	if(normal.dot(vmax) + d >= 0.0f) return true;
	
	return false;
}


/** Ray -> Cell test
 * Todo: fatta denna matte
 * */
bool Cell::rayCell(const Ray& R, float t) const
{
	
	float tminx, tmaxx, tminy, tmaxy, tminz, tmaxz;
	if(R.direction.x >=0)
	{
		tminx = (position.x-radius.x - R.position.x) * R.directionInv.x;
		tmaxx = (position.x+radius.x - R.position.x) * R.directionInv.x;
	}
	else
	{
		tminx = (position.x+radius.x - R.position.x) * R.directionInv.x;
		tmaxx = (position.x-radius.x - R.position.x) * R.directionInv.x;
	}
	
	if(R.direction.y >=0)
	{
		tminy = (position.y-radius.y - R.position.y) * R.directionInv.y;
		tmaxy = (position.y+radius.y - R.position.y) * R.directionInv.y;
	}
	else
	{
		tminy = (position.y+radius.y - R.position.y) * R.directionInv.y;
		tmaxy = (position.y-radius.y - R.position.y) * R.directionInv.y;
	}
	if ( (tminx > tmaxy) || (tminy > tmaxx) )
		return false;
	if(R.direction.z >=0)
	{
		tminz = (position.z-radius.z - R.position.z) * R.directionInv.z;
		tmaxz = (position.z+radius.z - R.position.z) * R.directionInv.z;
	}
	else
	{
		tminz = (position.z+radius.z - R.position.z) * R.directionInv.z;
		tmaxz = (position.z-radius.z - R.position.z) * R.directionInv.z;
	}
	
		
	if (tminy > tminx)
		tminx = tminy;
	if (tmaxy < tmaxx)
		tmaxx = tmaxy;
	
	if ( (tminx > tmaxz) || (tminz > tmaxx) )
		return false;
		
	if (tminz > tminx)
		tminx = tminz;
	if (tmaxz < tmaxx)
		tmaxx = tmaxz;
		
	return (tmaxx > 0) && (tminx < t);
}

/* Returns max and min for three floats */
inline void min_max(float v1, float v2, float v3, float& min, float& max)
{
	min = v1;
	max = v1;
	if(v2 > max)
		max = v2;
	if(v2 < min)
		min = v2;
	if(v3 > max)
		max = v3;
	if(v3 < min)
		min = v3;
}

/** Function that starts the recursive creation of the tree */
Cell* createTree(const Vector3& positionIn, const Vector3& radiusIn, Polygon* pList, int nrTriangles, Vertex* vertexList, int axis, int maxDepth, int currentDepth)
{
	vector<Polygon*> polyListOut;
	for(int i = 0; i < nrTriangles; i ++)
	{
		polyListOut.push_back(&pList[i]);
	}
	return new Cell(positionIn, radiusIn, polyListOut, vertexList, axis, maxDepth, currentDepth);
}

/** Create a kd-tree with a list of polygons */
Cell::Cell(const Vector3& positionIn, const Vector3& radiusIn, vector<Polygon*>& polyListIn, Vertex* vertexList, int axis, int maxDepth, int currentDepth)
{
	nrInters = 0;
	position = positionIn;
	radius = radiusIn;
	
	/* Loop through all polygons contained by the parent */
	for(unsigned int i = 0; i < polyListIn.size(); i ++)
	{
		
		/* Allocate a list to check if the polygon is contained by the cell */
		Vertex** vTemp = new Vertex*[polyListIn[i] -> nrVertices];
		
		/* Loop through all the vertices of the polygon */
		for(int j = 0; j  < polyListIn[i] -> nrVertices; j ++)
		{
			vTemp[j] = &(vertexList[polyListIn[i] -> vertices[j]]);
		}
		
		/* If the polygon is contained by the cell, add it to the vector */
		if(contains(vTemp, polyListIn[i] -> nrVertices, polyListIn[i] -> normal))
		{
			polyList.push_back(polyListIn[i]);
			n++;
		}
		delete vTemp;
	}
	
	/* Split the current cell in the coordinate where the cell is longest */
	axis=1;
	if(radiusIn.x >= radiusIn.y && radiusIn.x >= radiusIn.z)
		axis = 1;
	if(radiusIn.y >= radiusIn.x && radiusIn.y >= radiusIn.z)
		axis = 2;
	if(radiusIn.z >= radiusIn.x && radiusIn.z >= radiusIn.y)
		axis = 3;
	
	/* If the max depth is reached or if less than two polygons are contained by the cell, make the current cell a leaf */
	if(currentDepth >= maxDepth || polyList.size() < 2)
	{
		right = NULL;
		left = NULL;
	}
	else
	{
		/* The difference between the current cells position and its childrens positions */
		Vector3 posDiff = Vector3(axis == 1 ? radiusIn.x / 2.0f : 0.0f, axis == 2 ? radiusIn.y / 2.0f : 0.0f, axis == 3 ? radiusIn.z / 2.0f : 0.0f);
		
		/* Split the radius in two according to the choice of axis */
		Vector3 radiusNew = Vector3(axis == 1 ? radiusIn.x / 2.0f : radiusIn.x, axis == 2 ? radiusIn.y / 2.0f : radiusIn.y, axis == 3 ? radiusIn.z / 2.0f : radiusIn.z);
		
		/* Create the children */
		//#pragma omp task shared(positionIn)
		//{ 
			left = new Cell(positionIn + posDiff, radiusNew, polyList, vertexList, axis, maxDepth, currentDepth+1); 
		//}

		//#pragma omp task shared(positionIn)
		//{ 
			right = new Cell(positionIn - posDiff, radiusNew, polyList, vertexList, axis, maxDepth, currentDepth+1); 
		//}

		//#pragma omp taskwait
		
		
		
		/* Clear the polygon list if the current node is not a leaf */
		polyList.clear();
		polyList.resize(0);
		polyList.~vector<Polygon* >();
	}
}



/** Check if a point is contained by a cell */
bool Cell::contains(const Vector3& V) const
{
	if
	(
		V.x <= position.x+radius.x && V.x >= position.x-radius.x &&
		V.y <= position.y+radius.y && V.y >= position.y-radius.y &&
		V.z <= position.z+radius.z && V.z >= position.z-radius.z
	)
		return true;
	return false;
}


/** Translate a kd-tree */
void Cell::translate(const Vector3& tr)
{
	position = position + tr;
	if(left != NULL)
		left -> translate(tr);
	if(right != NULL)
		right -> translate(tr);
}

/** Scale a kd-tree */
void Cell::scale(const Vector3& sc)
{
	radius = radius * sc;
	if(left != NULL)
		left -> scale(sc);
	if(right != NULL)
		right -> scale(sc);
}

Cell::~Cell()
{
	
}

