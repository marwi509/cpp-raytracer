#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cstring>

#include "Vector3.hpp"
#include "Vertex.hpp"
#include "Material.hpp"
#include "Polygon.hpp"
#include "Ray.hpp"
#include "Image.hpp"
#include "Cell.hpp"
#include "Object.hpp"
#include "Check.hpp"



using namespace std;

/**
 * 	Ray -> Polygon test
 * */
inline Polygon* Object::rayPolygon(Polygon& P, const Ray& R, float& t, const Polygon* PCurr)
{
	if (&P == PCurr)
		return NULL;

	/* For quads, make two triangles and test */
	if (P.nrVertices == 4)
	{
		Polygon p1, p2;
		p1.nrVertices = p2.nrVertices = 3;
		p1.normal = p2.normal = P.normal;
		p1.vertices = new int[3];
		p2.vertices = new int[3];

		p1.vertices[0] = P.vertices[0];
		p1.vertices[1] = P.vertices[1];
		p1.vertices[2] = P.vertices[2];

		p2.vertices[0] = P.vertices[0];
		p2.vertices[1] = P.vertices[3];
		p2.vertices[2] = P.vertices[2];

		Polygon* temp = NULL;
		temp = rayPolygonBary(p1, R, t);
		if (temp != NULL)
		{
			free(p1.vertices);
			free(p2.vertices);
			return &P;
		}
		temp = rayPolygonBary(p2, R, t);
		if (temp != NULL)
		{
			free(p1.vertices);
			free(p2.vertices);
			return &P;
		}
		free(p1.vertices);
		free(p2.vertices);
		return NULL;

	}
	/* For triangles, just test */
	else
	{
		return rayPolygonBary(P, R, t);
	}
}

/** Ray -> triangle test using barycentric coordinates */
inline Polygon* Object::rayPolygonBary(Polygon& P, const Ray& R, float& t)
{
	Vector3 p1 = vertexList[P.vertices[0]].position;
	Vector3 p2 = vertexList[P.vertices[1]].position;
	Vector3 p3 = vertexList[P.vertices[2]].position;

	Vector3 e1 = p2 - p1;
	Vector3 e2 = p3 - p1;
	Vector3 s1 = R.direction.cross(e2);
	float divisor = s1.dot(e1);
	if (divisor == 0.0f)
		return NULL;
	float invDivisor = 1.0f / divisor;

	Vector3 d = R.position - p1;
	float b1 = d.dot(s1) * invDivisor;
	if (b1 < 0.0f || b1 > 1.0f)
		return NULL;

	Vector3 s2 = d.cross(e1);
	float b2 = R.direction.dot(s2) * invDivisor;
	if (b2 < 0.0f || b2 + b1 > 1.0f)
		return NULL;

	float t_temp = e2.dot(s2) * invDivisor;
	if (t_temp > 0.0001f && t_temp < t)
	{
		t = t_temp;
		return &P;
	}
	return NULL;
	;
}

/** Calculate a normal for a triangle */
inline void Object::getNormal(int triangleIndex)
{
	/* Calculate the normal using cross product */
	triangleList[triangleIndex].normal = (((vertexList[triangleList[triangleIndex].vertices[0]].position - vertexList[triangleList[triangleIndex].vertices[1]].position).norm()).cross
	(((vertexList[triangleList[triangleIndex].vertices[1]].position - vertexList[triangleList[triangleIndex].vertices[2]].position).norm()))).norm();
}

/** Return a normal for a triangle */
inline Vector3 Object::getNormal(const Vertex& v1, const Vertex& v2, const Vertex& v3)
{
	return	(((v1.position - v2.position).norm()).cross
	(((v2.position - v3.position).norm()))).norm();
}

/** Ray -> polygon list, not actually used anymore */
bool Object::rayObject(const Ray& R, float& t, Vector3& normal)
{
	bool hit = false;

	Vector3 positionI;

	for (int i = 0; i < nrTriangles; i++)
	{
		Polygon* P = NULL;
		if (rayPolygon(triangleList[i], R, t, P))
		{
			normal = triangleList[i].normal;
			hit = true;
		}
	}
	return hit;
}

/** Translate the object */
void Object::translate(const Vector3& dist)
{
	translation = translation + dist;
	for (int i = 0; i < nrVertices; i++)
	{
		vertexList[i].position = vertexList[i].position + dist;
	}

}

/** Scale the object */
void Object::scale(const Vector3& coeff)
{

	for (int i = 0; i < nrVertices; i++)
	{

		/* Börja med att translatera tillbaka till origo */
		vertexList[i].position = vertexList[i].position - translation;

		/* Skala enligt vektorn coeff */
		vertexList[i].position = vertexList[i].position * coeff;

		/* Translatera tillbaka */
		vertexList[i].position = vertexList[i].position + translation;
	}

	if (!(coeff.x == coeff.y && coeff.x == coeff.z))
		updateNormals();

}

/** Update the normals for all triangles and vertices */
void Object::updateNormals()
{
	/* Uppdatera alla normaler */
	for (int i = 0; i < nrTriangles; i++)
	{
		getNormal(i);
	}

	/* Uppdatera alla vertex-normaler */
	for (int i = 0; i < nrVertices; i++)
	{
		vertexList[i].normal = Vector3(0, 0, 0);
	}
	for (int i = 0; i < nrTriangles; i++)
	{
		for (int j = 0; j < triangleList[i].nrVertices; j++)
		{
			vertexList[triangleList[i].vertices[j]].normal = vertexList[triangleList[i].vertices[j]].normal + triangleList[i].normal;
		}
	}
	for (int i = 0; i < nrVertices; i++)
	{
		vertexList[i].normal = vertexList[i].normal.norm();
	}
}

/** Get the center of the object and the radius for its axis aligned bounding box */
void Object::findPosRadius(Vector3& pos, Vector3& rad) const
{
	Vector3
		max(-1000000.0f, -1000000.0f, -1000000.0f),
		min(1000000.0f, 1000000.0f, 1000000.0f);

	/* Loop through all vertices */
	for (int j = 0; j < nrVertices; j++)
	{
		/* Test if the x coordinate is the new max */
		if (vertexList[j].position.x > max.x)
		{
			max.x = vertexList[j].position.x;
		}

		/* Test if the x coordinate is the new min */
		if (vertexList[j].position.x < min.x)
		{
			min.x = vertexList[j].position.x;
		}

		/* Test if the y coordinate is the new max */
		if (vertexList[j].position.y > max.y)
		{
			max.y = vertexList[j].position.y;
		}

		/* Test if the y coordinate is the new min */
		if (vertexList[j].position.y < min.y)
		{
			min.y = vertexList[j].position.y;
		}

		/* Test if the z coordinate is the new max */
		if (vertexList[j].position.z > max.z)
		{
			max.z = vertexList[j].position.z;
		}

		/* Test if the z coordinate is the new min */
		if (vertexList[j].position.z < min.z)
		{
			min.z = vertexList[j].position.z;
		}

	}

	/* Update pos and rad */
	pos = (max + min) * 0.5f;
	rad = Vector3(fabsf(min.x - max.x), fabsf(min.y - max.y), fabsf(min.z - max.z)) * 0.5f;

	if (rad.x < 0.1f)
		rad.x = 0.1f;
	if (rad.y < 0.1f)
		rad.y = 0.1f;
	if (rad.z < 0.1f)
		rad.z = 0.1f;
}

/** Function that reads a triangle and vertexlist from an obj-file
	 * 	The obj-file needs to only contain triangles and vertices */
void Object::readObjFile(const char* filename)
{
	/* If the filename is empty */
	if (filename == NULL)
	{
		return;
	}

	string s;
	Polygon tri;

	/* Temporary lists */
	vector<Vertex> vListIn;
	vector<Polygon>  pListIn;

	/* Number of read triangles */
	long countt = 0;

	/* Number of read vertices */
	long countv = 0;

	/* Temporary int array for read numbers */
	int tempInts[4];

	/* Temporary variables */
	char in;
	char temp[100];
	Vector3 normal;

	/* Open the file */
	fstream fs;
	fs.open(filename, ios::in);

	while (!fs.eof())
	{
		/* Read one character, should be v or t */
		fs >> in;

		/* v == Vertex */
		if (in == 'v')
		{
			/* Create a new vertex */
			vListIn.push_back(Vertex());
			Vector3 vectorIn;

			/* Read three floats for the position */
			fs >> vectorIn.x;
			fs >> vectorIn.y;
			fs >> vectorIn.z;

			/* Add the position to the new vertex */
			vListIn[countv].position = vectorIn;

			/* Go to the next line and increase countv */
			fs.ignore();
			countv++;
		}

		/*
		 * f == Polygon
		 * It can have three or four (exported using blender) vertices
		 * A stringstream is used to see how many there is
		 */
		if (in == 'f')
		{
			/* Read the whole line */
			fs.getline(temp, 100);

			/* Insert the line in to a stringstream */
			stringstream ss(temp);

			/* Read ints from the stream */
			ss >> tempInts[0];

			/* The .obj vertex list starts at one, while our C array starts at 0 */
			tempInts[0] = (tempInts[0]) - 1;

			/* Repeat for all four possible vertices */
			ss >> tempInts[1];
			tempInts[1] = (tempInts[1]) - 1;
			ss >> tempInts[2];
			tempInts[2] = (tempInts[2]) - 1;
			ss >> tempInts[3];
			tempInts[3] = (tempInts[3]) - 1;

			/* Abort if something is wrong */
			if (tempInts[0] < 0 || tempInts[1] < 0 || tempInts[2] < 0)
				break;

			/* If the fourth value seems reasonable, it's a quad. Create two triangles */
			if (!(tempInts[0] == tempInts[1] || tempInts[0] == tempInts[2] || tempInts[2] == tempInts[1]))
			{
				if (tempInts[3] < countv && tempInts[3] >= 0)
				{
					/* Allocate memory */
					tri = Polygon();
					tri.vertices = new int[3];
					tri.nrVertices = 3;

					/* Add the vertices */
					tri.vertices[0] = tempInts[0];
					tri.vertices[1] = tempInts[1];
					tri.vertices[2] = tempInts[2];

					/* Calculate the normal */
					tri.normal = getNormal(vListIn[tempInts[0]], vListIn[tempInts[1]], vListIn[tempInts[2]]);

					/* Update the vertex normal */
					vListIn[tempInts[0]].normal = vListIn[tempInts[0]].normal + tri.normal;
					vListIn[tempInts[1]].normal = vListIn[tempInts[1]].normal + tri.normal;
					vListIn[tempInts[2]].normal = vListIn[tempInts[2]].normal + tri.normal;

					/* Add the triangle */
					pListIn.push_back(tri);


					/* Repeat for the second triangle */
					tri = Polygon();
					tri.vertices = new int[3];
					tri.nrVertices = 3;

					/* Add the vertices */
					tri.vertices[0] = tempInts[0];
					tri.vertices[1] = tempInts[3];
					tri.vertices[2] = tempInts[2];

					/* Calculate the normal */
					tri.normal = getNormal(vListIn[tempInts[0]], vListIn[tempInts[2]], vListIn[tempInts[3]]);

					/* Update the vertex normals */
					vListIn[tempInts[0]].normal = vListIn[tempInts[0]].normal + tri.normal;
					vListIn[tempInts[3]].normal = vListIn[tempInts[3]].normal + tri.normal;
					vListIn[tempInts[2]].normal = vListIn[tempInts[2]].normal + tri.normal;

					/* Add the polygon */
					pListIn.push_back(tri);

					countt += 2;
				}

				/* If the fourth value does not seem reasonable, ignore it and create a triangle */
				else
				{
					/* Allocate memory */
					tri = Polygon();
					tri.vertices = new int[3];
					tri.nrVertices = 3;

					/* Add the vertices */
					tri.vertices[0] = tempInts[0];
					tri.vertices[1] = tempInts[1];
					tri.vertices[2] = tempInts[2];

					/* Calculate the normal */
					tri.normal = getNormal(vListIn[tempInts[0]], vListIn[tempInts[1]], vListIn[tempInts[2]]);

					/* Update the vertex normals */
					vListIn[tempInts[0]].normal = vListIn[tempInts[0]].normal + tri.normal;
					vListIn[tempInts[1]].normal = vListIn[tempInts[1]].normal + tri.normal;
					vListIn[tempInts[2]].normal = vListIn[tempInts[2]].normal + tri.normal;

					/* Add the triangle */
					pListIn.push_back(tri);
					countt++;
				}
			}
		}
		tempInts[0] = -1;
		tempInts[1] = -1;
		tempInts[2] = -1;
		tempInts[3] = -1;
		if (!(in == 'f' || in == 'v'))fs.getline(temp, 100);
	}

	/* Allocate memory for the vertices in the object */
	vertexList = new Vertex[countv];
	nrVertices = countv;

	for (int i = 0; i < nrVertices; i++)
	{
		/* Normalize the vertex normals */
		vListIn[i].normal = vListIn[i].normal.norm();

		/* Copy the vertex list */
		vertexList[i] = vListIn[i];
	}

	/* Allocate memory for the triangles */
	triangleList = new Polygon[countt];
	nrTriangles = countt;

	for (int i = 0; i < nrTriangles; i++)
	{
		/* Copy the polygon list */
		triangleList[i] = pListIn[i];
		triangleList[i].material = material;
	}
	cout << "File " << filename << " read." << endl;
}

/** Ray -> Object, traverses the KD-tree, could probably be faster */
Polygon* Object::traverseTree(Ray& R, float& t, Vector3& normal, Cell* nod, const Polygon* PCurr)
{
	/* Some temprorary variables */
	Polygon temp;
	bool hit = false;
	float tTemp = t;
	Polygon* tempPoly = NULL;

	/* If R hits this cell, continue */
	if (!USE_AAB || nod->rayCell(R, t))
	{
		/* If this cell is a leaf, do a Ray -> Polygon test for all polygons in it */
		if (!USE_AAB || nod->left == NULL && nod->right == NULL)
		{
			/* Loop through all polygons */
			for (unsigned int i = 0; i < nod->polyList.size(); i++)
			{

				/* Save t in a new variable, in case the point that's hit is not inside this cell */
				tTemp = t;

				Polygon* tempPoly2 = NULL;
				tempPoly2 = rayPolygon(*(nod->polyList[i]), R, t, PCurr);

				/* Does R hit the polygon? */
				if (tempPoly2 != NULL)
				{
					/* Does this cell contain the hit point? */
					if (!USE_AAB || nod->contains(R * t))
					{
						hit = true;
						temp = *(nod->polyList[i]);
						tempPoly = tempPoly2;
					}

					/* If not, restore t */
					else
					{
						t = tTemp;
					}
				}
			}
			/* If something was hit, update the normal */
			if (tempPoly != NULL)
			{
				/* Phong interpolation if SMOOTH */
				if (SMOOTH)
					normal = interpolateNormal(temp, R * t);
				/* Otherwise, just return the triangle normal */
				else
					normal = temp.normal;
			}
			/* Return tempPoly, NULL if nothing was hit */

			return tempPoly;
		}

		/* If the cell is an internal node */
		else
		{
			/* Only left child? */
			if (nod->right != NULL && nod->left == NULL)
			{

				return traverseTree(R, t, normal, nod->right, PCurr);
			}

			/* Only right child? */
			else if (nod->left != NULL && nod->right == NULL)
			{

				return traverseTree(R, t, normal, nod->left, PCurr);
			}

			/* Calculate the length to the child cells */
			float lengthLeft = (R.position - nod->left->position).square();
			float lengthRight = (R.position - nod->right->position).square();

			/* If the left one is closest, check it first */
			if (lengthLeft < lengthRight)
			{

				/* If it hits something, we know it's closer than whatever it could hit in the right node */
				tempPoly = traverseTree(R, t, normal, nod->left, PCurr);
				if (tempPoly != NULL)
					return tempPoly;
				/* If it didn't hit anything, check the right too */
				return traverseTree(R, t, normal, nod->right, PCurr);
			}

			/* If the right one is closest, check it first */
			if (lengthLeft > lengthRight)
			{

				/* If it hits something, we know it's closer than whatever it could hit in the right node */
				tempPoly = traverseTree(R, t, normal, nod->right, PCurr);
				if (tempPoly != NULL)
					return tempPoly;
				/* If it didn't hit anything, check the left one as well */
				return traverseTree(R, t, normal, nod->left, PCurr);
			}

			/* If the distances are equal, check both */
			tempPoly = traverseTree(R, t, normal, nod->left, PCurr);
			Polygon* temp2 = NULL;
			temp2 = traverseTree(R, t, normal, nod->right, PCurr);;
			if (temp2 != NULL)
				tempPoly = temp2;

			/* Return tempPoly, NULL if it didn't hit anything */
			return tempPoly;
		}

	}
	/* If R doesn't hit the cell, return NULL */
	else return NULL;
}






/** Phong interpolation of normals */
Vector3 Object::interpolateNormal(const Polygon& P, const Vector3& point) const
{

	/* Set the point farthest away */
	float d0 = (vertexList[P.vertices[0]].position - point).length();
	float d1 = (vertexList[P.vertices[1]].position - point).length();
	float d2 = (vertexList[P.vertices[2]].position - point).length();

	int a, b, c;

	if (d0 > d1 && d0 > d2)
	{
		a = 0;
		b = 1;
		c = 2;
	}
	else if (d1 > d0 && d1 > d2)
	{
		a = 1;
		b = 0;
		c = 2;
	}
	else // if (d2 > d0 && d2 > d1)
	{
		a = 2;
		b = 0;
		c = 1;
	}


	/* Declare some new variables, to keep it simple */
	Vector3 edgeVec, edgeNormal, normalA, normalB, normalC, positionA, positionB, positionC;

	normalA = vertexList[P.vertices[a]].normal;
	normalB = vertexList[P.vertices[b]].normal;
	normalC = vertexList[P.vertices[c]].normal;

	positionA = vertexList[P.vertices[a]].position;
	positionB = vertexList[P.vertices[b]].position;
	positionC = vertexList[P.vertices[c]].position;

	/* Calculate a plane from the two nearest vertices, and project the point onto the plane */
	edgeVec = positionC;
	edgeNormal = (positionC - positionB).cross(P.normal);

	/* Create a ray from the vertex to the edge plane */
	Ray edge(positionA, (point - positionA).norm());

	float den, tNew;
	den = edgeNormal.dot(edge.direction);

	/* Get the intersection point */
	tNew = (edgeVec - edge.position).dot(edgeNormal) / den;
	Vector3 positionQ = edge * tNew;

	/* Interpolate the normals */
	Vector3 normalQ, normalI;
	normalQ = normalB + (normalC - normalB) * ((positionB - positionQ).length() / (positionB - positionC).length());
	normalI = normalQ + (normalA - normalQ) * ((positionQ - point).length() / (positionQ - positionA).length());

	return normalI.norm();

}




/** Function that creates a KD-tree for the object
 * 		Must be run for each object */
void Object::getAAB(int maxLevels)
{
	if (maxLevels > 0)
		cout << "Bygger accelerationsstruktur... " << endl;
	Vector3 pos, rad;
	findPosRadius(pos, rad);
	boundingBox = createTree(pos, rad, triangleList, nrTriangles, vertexList, 0, maxLevels, 0);
}


/** Add a triangle to the object */
void Object::addTriangle(int a, int b, int c)
{
	/* If the array is not allocated, allocate */
	if (tCapacity == 0)
	{
		triangleList = new Polygon[10];
		tCapacity = 10;
	}

	/* If the array is full, double the capacity */
	else if (tCapacity <= nrTriangles)
	{
		/* Create a temporary pointer */
		Polygon* triangleListOld = triangleList;

		/* Allocate new memory */
		triangleList = new Polygon[2 * tCapacity];

		/* Copy the old memory */
		memcpy(triangleList, triangleListOld, sizeof(Polygon) * tCapacity);

		/* Delete the old memory */
		free(triangleListOld);
		tCapacity *= 2;
	}

	/* Add the triangle */
	Polygon temp;
	temp.vertices = new int[3];
	temp.vertices[0] = a;
	temp.vertices[1] = b;
	temp.vertices[2] = c;

	temp.nrVertices = 3;

	temp.material = material;
	triangleList[nrTriangles] = temp;
	getNormal(nrTriangles++);
}

void Object::addQuad(int a, int b, int c, int d)
{
	/* Allocate memory */
	if (tCapacity == 0)
	{
		triangleList = new Polygon[10];
		tCapacity = 10;
	}

	/* If the array is full, double the capacity */
	else if (tCapacity <= nrTriangles)
	{
		/* Create a temporary pointer */
		Polygon* triangleListOld = triangleList;

		/* Allocate memory */
		triangleList = new Polygon[2 * tCapacity];

		/* Copy the old memory */
		memcpy(triangleList, triangleListOld, sizeof(Polygon) * tCapacity);

		/* Remove the old memory */
		free(triangleListOld);
		tCapacity *= 2;
	}

	/* Add the quad */
	Polygon temp;
	temp.vertices = new int[4];
	temp.vertices[0] = a;
	temp.vertices[1] = b;
	temp.vertices[2] = c;
	temp.vertices[3] = d;

	temp.nrVertices = 4;

	temp.material = material;
	triangleList[nrTriangles] = temp;
	getNormal(nrTriangles++);
}

/** Add a vertex */
void Object::addVertex(const Vector3& v)
{
	/* Allocate memory */
	if (vCapacity == 0)
	{
		vertexList = new Vertex[10];
		vCapacity = 10;
	}

	/* Double the capacity if it's full */
	else if (vCapacity <= nrVertices)
	{
		/* Create a temporary pointer */
		Vertex* vertexListOld = vertexList;

		/* Allocate memory */
		vertexList = new Vertex[2 * vCapacity];

		/* Copy the list */
		memcpy(vertexList, vertexListOld, sizeof(Vertex) * vCapacity);

		/* Free old memory */
		free(vertexListOld);
		vCapacity *= 2;
	}

	/* Add the vertex */
	vertexList[nrVertices++] = Vertex(v);
}


/** Create a box with its center in origin and the length 2 */
void Object::makeUnitBox()
{
	addVertex(Vector3(1.0f, -1.0f, -1.0f));
	addVertex(Vector3(1.0f, -1.0f, 1.0f));
	addVertex(Vector3(-1.0f, -1.0f, 1.0f));
	addVertex(Vector3(-1.0f, -1.0f, -1.0f));

	addVertex(Vector3(1.0f, 1.0f, -1.0f));
	addVertex(Vector3(1.0f, 1.0f, 1.0f));
	addVertex(Vector3(-1.0f, 1.0f, 1.0f));
	addVertex(Vector3(-1.0f, 1.0f, -1.0f));

	addQuad(0, 1, 2, 3);
	//addTriangle(0,1,2);
	//addTriangle(0,3,2);

	addQuad(4, 7, 6, 5);
	//addTriangle(4,7,6);
	//addTriangle(4,5,6);

	//addQuad(0, 4, 5, 1);
	//addTriangle(0,4,5);
	//addTriangle(0,1,5);

	addQuad(1, 5, 6, 2);
	//addTriangle(1,5,6);
	//addTriangle(1,2,6);

	//addQuad(2, 6, 7, 3);
	//addTriangle(1,6,7);
	//addTriangle(1,3,7);

	addQuad(4, 0, 3, 7);
	//addTriangle(5,1,4);
	//addTriangle(5,7,4);

	SMOOTH = false;
	USE_AAB = false;

	material->diffuseColor = Vector3(1, 1, 1);

	boundingBox = new Cell();
	for (int i = 0; i < nrTriangles; i++)
	{
		boundingBox->polyList.push_back(&triangleList[i]);
	}

}


/** Constructor that reads a mesh from an obj-file */
Object::Object(const char* filename, const Vector3& color)
{
	material = Material::DIFFUSE(color);
	readObjFile(filename);

	nrTriTests = 0;
}

/** Constructor that reads a mesh from an obj-file */
Object::Object(const char* filename, Material* material)
{
	this->material = material;
	readObjFile(filename);

	nrTriTests = 0;
}



Object::~Object()
{
	/* Hopefully only called when the program is terminated, so the OS will free all memory */
}

/** Rotate an object */
void Object::rotate(int axis, float degree)
{

	/* Undo the translation */
	for (int i = 0; i < nrVertices; i++)
	{
		vertexList[i].position = vertexList[i].position - translation;
	}

	/* Create the matrix */
	float M[3][3];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			M[i][j] = 0.0f;
	if (axis == 1)
	{

		M[0][0] = 1.0f;
		M[1][1] = cosf(degree);
		M[2][2] = cosf(degree);

		M[1][2] = sinf(degree);
		M[2][1] = -sinf(degree);
	}

	if (axis == 2)
	{

		M[1][1] = 1.0f;
		M[0][0] = cosf(degree);
		M[2][2] = cosf(degree);

		M[0][2] = sinf(degree);
		M[2][0] = -sinf(degree);
	}

	if (axis == 3)
	{

		M[2][2] = 1.0f;
		M[0][0] = cosf(degree);
		M[1][1] = cosf(degree);

		M[0][1] = sinf(degree);
		M[1][0] = -sinf(degree);
	}

	/* Rotate the vertices */
	for (int i = 0; i < nrVertices; i++)
	{
		vertexList[i].position = Vector3(
			M[0][0] * vertexList[i].position.x + M[1][0] * vertexList[i].position.y + M[2][0] * vertexList[i].position.z,
			M[0][1] * vertexList[i].position.x + M[1][1] * vertexList[i].position.y + M[2][1] * vertexList[i].position.z,
			M[0][2] * vertexList[i].position.x + M[1][2] * vertexList[i].position.y + M[2][2] * vertexList[i].position.z
		);
	}

	/* Redo the translation */
	for (int i = 0; i < nrVertices; i++)
	{
		vertexList[i].position = vertexList[i].position + translation;
	}

	/* Update the normals */
	updateNormals();

}


/* Places an object on the floor, whatever height that is */
void Object::placeOnFloor(float floorHeight)
{
	float lowest = 100000.0f;
	for (int i = 0; i < nrVertices; i++)
	{
		if (vertexList[i].position.y < lowest)
			lowest = vertexList[i].position.y;
	}
	translate(Vector3(0, floorHeight - lowest, 0));
}

float Object::findClosestPosition() {
	float lowest = 100000.0f;
	for (int i = 0; i < nrVertices; i++)
	{
		float square = vertexList[i].position.square();
		if (square < lowest)
			lowest = square;
	}
	return sqrtf(lowest);
}

void Object::normsize()
{
	Vector3 rad, pos;
	findPosRadius(pos, rad);
	float use = rad.z;
	if (rad.x > rad.y && rad.x > rad.z)
		use = rad.x;
	else if (rad.y > rad.x && rad.y > rad.z)
		use = rad.y;
	scale(Vector3(1.0f / use, 1.0f / use, 1.0f / use));

}