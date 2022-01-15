/**
 *	Class that contains pixels and image information
 */

#include <cstdlib>
#include "Point.hpp"


#ifndef _IMAGEHPP
#define _IMAGEHPP

class Image
{
	
	
	public:

	/* Array with pixels */
	unsigned int* pixels;
	
	/* Size of the image */
	int height,width;
	
	/** Empty constructor */
	Image()
	{
		pixels = NULL;
	}
	~Image()
	{
		delete [] pixels;
	}
	/** Constructor with height and width */
	Image(int win, int hin)
	{
		/* Check that the sizes are > 0 */
		if(win > 0 && hin > 0)
		{
			/* Allocate memory for the pixel */
			pixels = (unsigned int*)malloc(sizeof(unsigned int)*hin*win);
			width = win;
			height = hin;
		}
		else
		{
			pixels = NULL;
		}
	}
	
	/** Get a pixel from the image */
	Vector3 getPixel(int x, int y) const;
	
	/** Function to add a rgb-value to a pixel in the image, does a gamma correction */
	bool putPixel(int x, int y, Vector3 c, float gamma);
	
	/** Funktion för att skriva pixlarna till en ppm-bild */
	bool writeImage(const char* filename);

	void render();
	
	/** Mosaic */
	void createMosaic(const char* filename, int tilesize);

	void Image::savebmp(const char* filename);
};

#endif
