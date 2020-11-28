#include "Vector3ld.hpp"
#include <iostream>

#ifndef HDRIMAGE_HPP
#define HDRIMAGE_HPP

class HdrImage {
	
	private:
	Vector3** pixels;;
	int width, height;
	
	public:
	
	int getWidth() const { return width; }
	int getHeight() const { return height; }
	
	~HdrImage() {
		clearBuffer();
	}
	
	HdrImage& operator=(const HdrImage& imageToCopy) {
		clearBuffer();
		width = imageToCopy.getWidth();
		height = imageToCopy.getHeight();
		initializeBuffers();
		for(int i = 0; i < width; i ++) {
			for(int j = 0; j < height; j ++) {
				setPixel(i,j,imageToCopy.getPixel(i,j));
			}
		}
		return *this;
		//return HdrImage(imageToCopy);
	}
	
	HdrImage operator-(Vector3 v) {
		return (*this) + (Vector3() - v);
	}
	
	void clearBuffer() {
	for(int i = 0; i < width; i ++) {
			delete [] pixels[i];
		}
		delete [] pixels;
	}
	
	HdrImage operator+(Vector3 v) {
		HdrImage newImage(this -> getWidth(), this -> getHeight());
		for(int i = 0; i < width; i ++) {
			for(int j = 0; j < height; j ++) {
				Vector3 res = getPixel(i, j) + v;
				float x = res.x, y = res.y, z = res.z;
				if(res.x < 0.0) x = 0.0;// else std::cout << "AAAH!";
				if(res.y < 0.0) y = 0.0;//else std::cout << "AAAH!";
				if(res.z < 0.0) z = 0.0;//else std::cout << "AAAH!";
				newImage.setPixel(i, j, Vector3(x,y,z));
			}
		}
		return newImage;
	}
	
	HdrImage operator-(const HdrImage& otherImage) {
		HdrImage newImage(this -> getWidth(), this -> getHeight());
		for(int i = 0; i < width; i ++) {
			for(int j = 0; j < height; j ++) {
				Vector3 res = getPixel(i, j) - otherImage.getPixel(i,j);
				float x = res.x, y = res.y, z = res.z;
				if(res.x < 0.0) x = 0.0;// else std::cout << "AAAH!";
				if(res.y < 0.0) y = 0.0;//else std::cout << "AAAH!";
				if(res.z < 0.0) z = 0.0;//else std::cout << "AAAH!";
				newImage.setPixel(i, j, Vector3(x, y, z));
			}
		}
		return newImage;
	}
	
	HdrImage operator+(const HdrImage& otherImage) {
		HdrImage newImage(this -> getWidth(), this -> getHeight());
		for(int i = 0; i < width; i ++) {
			for(int j = 0; j < height; j ++) {
				Vector3 res = getPixel(i, j) + otherImage.getPixel(i,j);
				float x = res.x, y = res.y, z = res.z;
				if(res.x < 0.0) x = 0.0;// else std::cout << "AAAH!";
				if(res.y < 0.0) y = 0.0;//else std::cout << "AAAH!";
				if(res.z < 0.0) z = 0.0;//else std::cout << "AAAH!";
				newImage.setPixel(i, j, Vector3(x, y, z));
			}
		}
		return newImage;
	}
	
	HdrImage operator*(float f) {
		HdrImage newImage(this -> getWidth(), this -> getHeight());
		for(int i = 0; i < width; i ++) {
			for(int j = 0; j < height; j ++) {
				newImage.setPixel(i, j, getPixel(i, j) * f);
			}
		}
		return newImage;
	}
	
	HdrImage(const HdrImage& imageToCopy) {
		width = imageToCopy.getWidth();
		height = imageToCopy.getHeight();
		initializeBuffers();
		for(int i = 0; i < width; i ++) {
			for(int j = 0; j < height; j ++) {
				setPixel(i,j,imageToCopy.getPixel(i,j));
			}
		}
	}
	
	void initializeBuffers() {
		pixels = new Vector3*[width];
		for(int i = 0; i < width; i ++) {
			pixels[i] = new Vector3[height]();
		}
	}
	
	HdrImage(const int width, const int height) {
		this -> width = width;
		this -> height = height;
		initializeBuffers();
	}
	
	Vector3 getPixel(int x, int y) const {
		if(x >= width) {
			return Vector3();
		} else if(x < 0) {
			return Vector3();
		} else if(y >= height) {
			return Vector3();
		} else if(y < 0) {
			return Vector3();
		}
		return pixels[x][y];
	}
	
	void setPixel(int x, int y, Vector3 v) {
		pixels[x][y] = v;
	}
	
	HdrImage smooth(int r) {
		HdrImage smoothedImage(this -> getWidth(), this -> getHeight());
		//#pragma omp parallel for schedule(dynamic, 1)
		for(int i = 0; i < width; i ++) {
			for(int j = 0; j < height; j ++) {
				smoothedImage.setPixel(i, j, smoothPixel(i, j, r));
			}
		}
		return smoothedImage;
	}
	
	Vector3 smoothPixel(int x, int y, int r) {
		int samples = 0;
		Vector3 sum;
		for(int i = -r; i <= r; i ++) {
			for(int j = -r; j <= r; j ++) {
				if(i * i  + j * j  <=  r * r) {
					samples++;
					sum = sum + getPixel(x + i, y + j);
				}
			}
		}
		sum = sum * (1.0f / (float) samples);
		return sum;
	}
	
	Vector3 sobelPixel(int x, int y) {
		Vector3 sum;
		float weights [3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
		Vector3 derX, derY;
		for(int i = -1; i <= 1; i ++) {
			for(int j = -1; j <= 1; j ++) {
					//samples++;
					//sum = sum + getPixel(x + i, y + j) * weights;
				derX = derX + getPixel(x + i, y + j) * weights[i + 1][j + 1];
				derY = derY + getPixel(x + i, y + j) * weights[j + 1][i + 1];
			}
		}
		//sum = sum * (1.0f / (float) samples);
		Vector3 grad = derX * derX + derY * derY;
		return Vector3(sqrtf(grad.x), sqrtf(grad.y), sqrtf(grad.z));
	}
	
	HdrImage sobel() {
		HdrImage sobelImage(this -> getWidth(), this -> getHeight());
		//#pragma omp parallel for schedule(dynamic, 1)
		for(int i = 0; i < width; i ++) {
			for(int j = 0; j < height; j ++) {
				Vector3 res = sobelPixel(i, j);
				sobelImage.setPixel(i, j, res);
			}
		}
		return sobelImage;
	}
	
	HdrImage bloom(int smoothSize, float weight) {
		std::cout << "Adding bloom" << std::endl;
		return (*this - Vector3(1,1,1)).smooth(smoothSize) * weight + *this;
	}
	
	Image toImage() {
		std::cout << "Converting to Image" << std::endl;
		Image imageOut(width, height);
		for(int i = 0; i < width; i ++) {
			for(int j = 0; j < height; j ++) {
				imageOut.putPixel(i, j, this -> getPixel(i, j), 1.8f);
			}
		}
		return imageOut;
	}
};

#endif
