#include "Point.hpp"

/** The partition part of quickselect */
int partition(Point* a, int start, int end, int piv, int axis)
{
	/* x axis */
	if(axis == 0)
	{
		Point xtemp = a[piv];
		a[piv] = a[end];
		a[end] = xtemp;
		int i=start,j=end-1;
		Point temp;
		while(i < j)
		{
			if(a[i].x <= xtemp.x)
			{
				if(a[j].x >= xtemp.x)
				{
					temp = a[i];
					a[i++] = a[j];
					a[j--] = temp;
				}
				else
				{
					j--;
				}
			}
			else if(a[j].x >= xtemp.x)
			{
				i++;
			}
			else
			{
				i++;
				j--;
			}
		}
		for(;a[i].x > xtemp.x; i++);
		a[end] = a[i];
		a[i] = xtemp;
		return i;
	}
	
	/* y axis */
	else
	{
		Point xtemp = a[piv];
		a[piv] = a[end];
		a[end] = xtemp;
		int i=start,j=end-1;
		Point temp;
		while(i < j)
		{
			if(a[i].y <= xtemp.y)
			{
				if(a[j].y >= xtemp.y)
				{
					temp = a[i];
					a[i++] = a[j];
					a[j--] = temp;
				}
				else
				{
					j--;
				}
			}
			else if(a[j].y >= xtemp.y)
			{
				i++;
			}
			else
			{
				i++;
				j--;
			}
		}
		for(;a[i].y > xtemp.y; i++);
		a[end] = a[i];
		a[i] = xtemp;
		return i;
	}
}


/** Quickselect for the createTree() function */
void select(Point* a, int p, int r, int n, int axis)
{
	int k;
	while(1)
	{
		if(r-p < 2)
			return;
			
		/* Select pivot in the middle of the list */
		k = (p+r)/2;
		
		/* partition */
		k = partition(a,p,r,k,axis);
		
		/* If k equals the point in the list we're looking for, exit */
		if(n == k)
			return;
			
		/* If k is larger than what we're looking for, keep looking on the left side of the list */
		if(n<k)
			r = k - 1;
			
		/* If k is smaller than what we're looking for, keep looking on the right side of the list */
		else
			p = k + 1;
	}
}

/** Function that creates a two-dimensional balanced kd-tree from a set of points */
Point* createTree(Point* array, int n, int axis)
{
	Point* current;
	/* Empty list, return nullpointer */
	if(n == 0)
		return NULL;
		
	/* List of one, create leaf */
	if(n == 1)
	{
		current = new Point(array[0].x, array[0].y);
		current -> color = array[0].color;
		return current;
	}
	
	/* Partition the array so the median is in the middle of the list */
	select(array,0,n-1,n/2,axis);
	
	/* Switch axis */
	axis = 1-axis;
	
	/* Create the current node of the kdtree as the middle of the list */
	current = new Point(array[n/2].x, array[n/2].y);
	current -> color = array[n/2].color;
	current -> left = createTree(array, n/2, axis);
	current -> right = createTree(array + n/2 + 1, n/2 -1, axis);
	
	/* return the pointer of the current node */
	return current;
}

/** Nearest neighbour search in the KD-tree of points (they are sorted in descending order) approx logn time complexity */
Point* nearestNeighbour(Point* tree, Point punkt, float& best, Point*& bestpunkt, int axis, Point* current)
{
	if(tree == NULL)
		return NULL;
	
	/* Change the axis from x, or to x */
	int axisout = 1 - axis;
	
	/* If the current node is better than the current best, update the best */
	if(best > (tree -> x - punkt.x)*(tree -> x - punkt.x) + (tree -> y - punkt.y)*(tree -> y - punkt.y))
	{
		if(current != tree)
		{
			best = (tree -> x - punkt.x)*(tree -> x - punkt.x) + (tree -> y - punkt.y)*(tree -> y - punkt.y);
			bestpunkt = tree;
		}
	}
	
	/* If x axis */
	if(axis == 0)
	{
		/* if the x value is smaller than the current node, look to the right first */
		if(punkt.x < tree -> x)
		{
			nearestNeighbour(tree -> right, punkt, best, bestpunkt, axisout, current);
			
			/* If the current best can be beaten on the left side, look there as well */
			if(best > powf(punkt.x - tree -> x, 2))
			{
				nearestNeighbour(tree -> left, punkt, best, bestpunkt, axisout, current);
			}
		}
		
		/* if the x value is larger than the current node, look to the right first */
		else if(punkt.x > tree -> x)
		{
			nearestNeighbour(tree -> left, punkt, best, bestpunkt, axisout, current);
			
			/* If the current best can be beaten on the right side, look there as well */
			if(best > powf(punkt.x - tree -> x, 2))
			{
				nearestNeighbour(tree -> right, punkt, best, bestpunkt, axisout, current);
			}
		}
		
		/* If it's equal, look at both sides of the node */
		else
		{
			nearestNeighbour(tree -> right, punkt, best, bestpunkt, axisout, current);
			nearestNeighbour(tree -> left, punkt, best, bestpunkt, axisout, current);
		}
	}
	
	/* If y axis */
	else
	{
		/* if the y value is smaller than the current node, look to the right first */
		if(punkt.y < tree -> y)
		{
			nearestNeighbour(tree -> right, punkt, best, bestpunkt, axisout, current);
			
			/* If the current best can be beaten on the left side, look there as well */
			if(best > powf(punkt.y - tree -> y, 2))
			{
				nearestNeighbour(tree -> left, punkt, best, bestpunkt, axisout, current);
			}
		}
		
		/* if the y value is larger than the current node, look to the right first */
		else if(punkt.y > tree -> y)
		{
			nearestNeighbour(tree -> left, punkt, best, bestpunkt, axisout, current);
			
			/* If the current best can be beaten on the right side, look there as well */
			if(best > powf(punkt.y - tree -> y, 2))
			{
				nearestNeighbour(tree -> right, punkt, best, bestpunkt, axisout, current);
			}
		}
		
		/* If they're equal, look at both sides */
		else
		{
			nearestNeighbour(tree -> right, punkt, best, bestpunkt, axisout, current);
			nearestNeighbour(tree -> left, punkt, best, bestpunkt, axisout, current);
		}
		//*/
	}
}
