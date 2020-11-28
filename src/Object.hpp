/**
 * Klass som representerar ett objekt
 * Innehåller främst en triangel-lista och en vertex-lista
 * 	samt lite info om hur den bör renderas
 * */
#ifndef _Object_HPP
#define _Object_HPP
#include "Material.hpp"
#include "Vertex.hpp"
#include "Cell.hpp"
#include "Ray.hpp"

const int X_AXIS = 1;
const int Y_AXIS = 2;
const int Z_AXIS = 3;



class Object
{
	public:
	/* Polygon list */
	Polygon* triangleList;
	
	/* Vertex list */
	Vertex* vertexList;
	
	/* KD-tree */
	Cell* boundingBox;
	
	bool SMOOTH, USE_AAB;
	
	/* Number of triangles and vertices */
	int nrTriangles, nrVertices;
	long nrTriTests;
	
	/* Total translation,
	 * Used so we can rotate and scale objects after translation without errors */
	Vector3 translation;
	
	/* The current material of the object 
	 * When new polygons are added, their material will be the one currently pointed to in their
	 * 	objects class. This one is then not actually used in the render step. */
	Material* material;
	
	/** Ray -> Polygon check */
	inline Polygon* rayPolygon(Polygon& P, const Ray& R, float& t, const Polygon* PCurr);
	
	/** Barycentric Ray -> Polygon check */
	inline Polygon* rayPolygonBary(Polygon& P, const Ray& R, float& t);
	
	/** Calculate a normal for a triangle */
	void getNormal(int triangleIndex);
	
	/** Return a normal for a triangle */
	Vector3 getNormal(const Vertex& v1, const Vertex& v2, const Vertex& v3);
	
	/** Ray -> Polygon list */
	bool rayObject(const Ray& R, float& t, Vector3& normal);
	
	/** Constructor that gets vertices and polygons from an obj-file */
	Object(const char* filename, const Vector3& color);
	
	/** Constructor that gets vertices and polygons from an obj-file */
	Object(const char* filename, Material* material);
	
	/** Translate the object */
	void translate(const Vector3& dist);
	
	/** Scale the object */
	void scale(const Vector3& coeff);
	
	/** Get the center of the object and the radius for its axis aligned bounding box */
	void findPosRadius(Vector3& pos, Vector3& rad) const;
	
	/** Empty constructor */
	Object()
	{
		material = Material::DIFFUSE(Vector3(0.75, 0.75, 0.75));
		nrTriangles = nrVertices = vCapacity = tCapacity = 0;
		triangleList = NULL;
		vertexList = NULL;
		nrTriTests = 0;
	}
	
	/** Ray -> Object, traverses the KD-tree */
	Polygon* traverseTree(Ray& R, float& t, Vector3& normal, Cell* nod, const Polygon* PCurr);
		
	/** Interpolate a normal with vertex normals */
	Vector3 interpolateNormal(const Polygon& P, const Vector3& point) const;
	
	/** Function that reads a triangle and vertexlist from an obj-file
	 * 	The obj-file needs to only contain triangles and vertices */
	void readObjFile(const char* filename);
	
	/** Function that creates a KD-tree for the object, whith depth <= maxLevels */
	void getAAB(int maxLevels);
	
	/** Function that manually adds a triangle */
	void addTriangle(int a, int b, int c);
	
	/** Function that manually adds a quad */
	void addQuad(int a, int b, int c, int d);
	
	/** Function that adds a vertex */
	void addVertex(const Vector3& v);
	
	/** Funktion som skapar en box med center i origo och längden 2 på sidorna */
	void makeUnitBox();
	
	/** Funktion som uppdaterar normaler för trianglar och vertices */
	void updateNormals();
	
	/** Rotate the object */
	void rotate(int axis, float degree);
	
	/** Place the object on the floor */
	void placeOnFloor(float floorHeight);
	
	/** Normalize the size */
	void normsize();
	
	float findClosestPosition();
	
	~Object();
	
	private:
	
	/** Current capacity of the lists, used to kind of replicate the behavior of a c++ vector */
	int vCapacity;
	int tCapacity;
};

/** Todo, fatta denna */
inline bool pnpoly(unsigned char npol, float *xp, float *yp, float x, float y)
{
	int i, j;
	bool c = false;
	for (i = 0, j = npol-1; i < npol; j = i++) 
	{
		if ((((yp[i] <= y) && (y < yp[j])) ||
			((yp[j] <= y) && (y < yp[i]))) &&
				(x < (xp[j] - xp[i]) * (y - yp[i]) / (yp[j] - yp[i]) + xp[i]))
				c = !c;
	}
	return c;
}

#endif