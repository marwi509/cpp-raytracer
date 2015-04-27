#include "Vector3.hpp"
#include <cstdlib>

#ifndef _POINT_HPP
#define _POINT_HPP

/** Class point for the mosaic post-process */

class Point
{
	public:
	float x,y;
	Point *left, *right;
	Point()
	{
		left = right = NULL;
		color = Vector3(50,0,0);
	}
	Vector3 color;
	Point(float xin, float yin)
	{
		left = right = NULL;
		x = xin;
		y = yin;
		color = Vector3(50,0,0);
	}
};

#endif

/* Functions that make the kd-tree and finds nearest neighbours */
Point* nearestNeighbour(Point* tree, Point punkt, float& best, Point*& bestpunkt, int axis, Point* current);
Point* createTree(Point* array, int n, int axis);
void quicksort(Point* a, int p, int r, int axis);
void sort(Point* array, int n, int axis);
int partition(Point* a, int start, int end, int piv, int axis);
