#include <fstream>
#include "Vector3.hpp"
#include "Image.hpp"
#include <cstdio>
#include <iostream>
#include "Window.h"

using namespace std;

/** Function that writes the image to a ppm-file */
bool Image::writeImage(const char* filename)
{
	/* Red green and blue */
	int r,g,b;
	FILE *f;
	
	/* Return false if the file can't be opened */
	if(!(f = fopen(filename, "w")))
		return false;
	
	/* Write header info */
	fprintf(f, "P3\n%d %d\n%d\n", width, height, 255); 
	
	/* Loop through all pixels */
	for (unsigned short i=0; i < height; i++) 
	{
		for(unsigned short j = 0; j < width; j++)
		{
			/* Get the subppixels with bitshifts */
			b = pixels[(height-i-1)*width+j] >> 16;
			g = (pixels[(height-i-1)*width+j] - (b << 16)) >> 8;
			r = pixels[(height-i-1)*width+j] - (b << 16) - (g << 8);
			
			/* Write to file */
			fprintf(f,"%d %d %d ", r, g, b);
		}
	}
	
	/* Close file */
	fclose(f);
	return true;
}

/** Function to add a rgb-value to a pixel in the image, does a gamma correction */
bool Image::putPixel(int x, int y, Vector3 c, float gamma)
{
	/* Check so that x,y are inside the image */
	if(c.x < 0.0f || c.y < 0.0f || c.z < 0.0f ||
		x < 0 || y < 0 || x > width || y > height)
		return false;
		
	/* Gamma correct */
	c = Vector3(powf(c.x, 1.0f / gamma), powf(c.y, 1.0f / gamma), powf(c.z, 1.0f / gamma));
	
	/* Scale to [0, 255] */
	c = c * 255.0f;
	c = Vector3(c.x > 255 ? 255 : c.x, c.y > 255 ? 255 : c.y, c.z > 255 ? 255 : c.z);
	
	/* Insert the pixels with bitshifts */
	pixels[y*width + x] = (int)c.x | ((int)c.y) << 8 | ((int)c.z) << 16;
	return true;
}

/** Get a pixel from the image */
Vector3 Image::getPixel(int x, int y) const
{
	if(x < 0 || x > width-1 || y < 0 || y > height -1)
		return Vector3();
	int b,g,r;
	b = pixels[(height-y-1)*width+x] >> 16;
	g = (pixels[(height-y-1)*width+x] - (b << 16)) >> 8;
	r = pixels[(height-y-1)*width+x] - (b << 16) - (g << 8);

	return Vector3(r,g,b);
}


/** Function that creates a mosaic of the image, tilesize is how many pixels on average a tile takes up */
void Image::createMosaic(const char* filename, int tilesize)
{
	
	/* Number of tiles */
	int number_points = height*width/tilesize;
	
	/* Allocate the center points for the tiles */
	Point *points = new Point[number_points];
	
	/* Randomly distribute the points on the image plane */
	for(int i = 0; i < number_points; i ++)
	{
		points[i].x = randf();
		points[i].y = randf();
		
		/* Multisample */
		points[i].color = 
			(
				getPixel(floorf(points[i].x*width), floorf(points[i].y*height)) + 
				getPixel(floorf(points[i].x*width)+1, floorf(points[i].y*height)) + 
				getPixel(floorf(points[i].x*width), floorf(points[i].y*height)+1) + 
				getPixel(floorf(points[i].x*width)+1, floorf(points[i].y*height)+1)
			) * .25;
	}
	
	/* Create a balanced kd-tree for quick nearest neighbour */
	Point* tree = createTree(points, number_points, 0);
	
	/* Mosaic image */
	Image *nybild = new Image(width, height);
	
	/* Anti-aliasing for the edges */
	int aa = 2;
	
	/* Loop through all pixels of the new image */
	for(int i = 0; i < height; i ++)
	{
		for(int j = 0; j < width; j ++)
		{
			Vector3 color;
			
			/* Multisample */
			for(int k = 0; k < aa; k ++)
			{
				for(int o = 0; o < aa; o ++)
				{
					bool edge = false;
					Point *p;
					float t = 100000000;
					Point denna = Point(float(j*aa + k) / float(width * aa), 1-float(i*aa + o) / float(height * aa));
					
					nearestNeighbour(tree, denna, t, p, 0, NULL);
					
					Point *second;
					
					/* Check the neigbouring pixels */
					for(int m = 0; m <= 1; m ++)
					for(int l = 0; l <= 1; l ++)
					{
						if(!(m == 0 && l == 0) && !edge)
						{
							denna = Point((float(j*aa+m*aa+k)-0.5) / float(width * aa), 1-(float(i*aa+l*aa+o)-0.5) / float(height * aa));
							t = 100000000;
							nearestNeighbour(tree, denna, t, second, 0, NULL);
							
							/* If one of the neighbouring pixels has another point nearest, we're on an edge */
							if(second != p)
							{
								edge = true;
							}
						}
					}
					
					/* If the pixel is on an edge, return dark grey */
					if(edge)
						color = color + Vector3(0.1,0.1,0.1);
					/* Else, return the color of the closest tile */
					else
						color = color + p -> color * (1.0 / 255.0);
					
				}
			}
			/* insert the pixel into the new image */
			nybild -> putPixel(j,i,color * (1.0f / float(aa*aa)), 1.0f);
		}
	}
	/* Write the image to disk and delete it */
	nybild -> writeImage(filename);
	delete nybild;
	delete [] points;
}

void Image::render() 
{
	Window::draw(*this);
}









