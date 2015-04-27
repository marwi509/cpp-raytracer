#ifndef _CELL_HPP
#define _CELL_HPP
/**
 * Class that contains a cell (axis aligned bounding box, the root contains one polygon mesh) and a list of polygons contained by that cell, 
 * 	OR to cell children that contain polygons. (only leaves contain polygons)
 * 	
*/

using namespace std;

static int nrInters;

class Cell
{

	public:
	
	/* Vector containing the polygons */
	vector<Polygon*> polyList;
	
	/* Number of polygons */
	int n;
	
	
	/* Limits for the AABB */
	Vector3 position, radius;
	
	/* Children of the cell, NULL for leaf */
	Cell *left, *right;
	
	/* Empty constructor */
	Cell()
	{
		right = NULL;
		left = NULL;
		nrInters = 0;
	}
	
	/* Constructor for a simple AABB */
	Cell(const Vector3& positionIn, const Vector3& radiusIn)
	{
		/* No children */
		left = NULL;
		right = NULL;
		position = positionIn;
		radius = radiusIn;
		nrInters = 0;
	}
	
	~Cell();
	
	/* Constructor for KD-tree */
	Cell(const Vector3& positionIn, const Vector3& radiusIn, vector<Polygon*>&, Vertex* vertexList, int axis, int maxDepth, int currentDepth);
	
	/** Function that checks if a polygon is contained by a cell (only triangles and quads) */
	inline bool contains(Vertex** v, int nrVertices, const Vector3& normal);
	
	/** Ray -> cell test */
	bool rayCell(const Ray& R, float t) const;
	
	/** Check if a point is contained by a cell */
	bool contains(const Vector3& V) const;
	
	/** Translate a kd-tree */
	void translate(const Vector3& tr);
	
	/** Scale a kd-tree */
	void scale(const Vector3& sc);
	
};


inline void min_max(float v1, float v2, float v3, float& min, float& max);
Cell* createTree(const Vector3& positionIn, const Vector3& radiusIn, Polygon* pList, int nrTriangles, Vertex* vertexList, int axis, int maxDepth, int currentDepth);

#endif
