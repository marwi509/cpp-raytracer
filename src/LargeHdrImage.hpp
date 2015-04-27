#include "Vector3ld.hpp"
#include "Vector3.hpp"
#include "Image.hpp"
#include "HdrImage.hpp"
#include <iostream>

class LargeHdrImage {
	
	private:
	Vector3ld** pixels;
	int** samples;
	int resultingWidth, resultingHeight;
	int width, height;
	int MSAA;
	
	public:
	
	HdrImage toHdrImage() {
		HdrImage newImage(this -> resultingWidth, this -> resultingHeight);
		for(int i = 0; i < resultingWidth; i ++) {
			for(int j = 0; j < resultingHeight; j ++) {
				Vector3 res = getResultingPixel(i, j);
				newImage.setPixel(i, j, res);
			}
		}
		return newImage;
	}
	
	LargeHdrImage(const int width, const int height, const int MSAA) {
		this -> resultingWidth = width;
		this -> resultingHeight = height;
		this -> width = width * MSAA;
		this -> height = height * MSAA;
		this -> MSAA = MSAA;
		initializeBuffers();
	}
	
	void addSample(const Vector3& sample, int x, int y, int msaaX, int msaaY) {
		Vector3ld preciseSample(sample.x, sample.y, sample.z);
		addSample(preciseSample, x * MSAA + msaaX, y * MSAA + msaaY);
	}
	
	int getWidth() const { return width; }
	int getHeight() const { return height; }
	
	~LargeHdrImage() {
		clearBuffer();
	}
	
	LargeHdrImage& operator=(const LargeHdrImage& imageToCopy) {
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
	}
	
	LargeHdrImage operator-(Vector3ld v) {
		return (*this) + (Vector3ld() - v);
	}
	
	void clearBuffer() {
	for(int i = 0; i < width; i ++) {
			delete [] pixels[i];
			delete [] samples[i];
		}
		delete [] pixels;
		delete [] samples;
	}
	
	LargeHdrImage operator+(Vector3ld v) {
		LargeHdrImage newImage(this -> getWidth(), this -> getHeight(), this -> MSAA);
		for(int i = 0; i < width; i ++) {
			for(int j = 0; j < height; j ++) {
				Vector3ld res = getPixel(i, j) + v;
				if(res.x < 0.0) res.x = 0.0;
				if(res.y < 0.0) res.y = 0.0;
				if(res.z < 0.0) res.z = 0.0;
				newImage.setPixel(i, j, res);
			}
		}
		return newImage;
	}
	
	LargeHdrImage operator-(const LargeHdrImage& otherImage) {
		LargeHdrImage newImage(this -> resultingWidth, this -> resultingHeight, this -> MSAA);
		for(int i = 0; i < width; i ++) {
			for(int j = 0; j < height; j ++) {
				Vector3ld res = getPixel(i, j) - otherImage.getPixel(i,j);
				if(res.x < 0.0) res.x = 0.0;// else std::cout << "AAAH!";
				if(res.y < 0.0) res.y = 0.0;//else std::cout << "AAAH!";
				if(res.z < 0.0) res.z = 0.0;//else std::cout << "AAAH!";
				newImage.setPixel(i, j, res);
			}
		}
		return newImage;
	}
	
	LargeHdrImage operator+(const LargeHdrImage& otherImage) {
		LargeHdrImage newImage(this -> resultingWidth, this -> resultingHeight, this -> MSAA);
		for(int i = 0; i < width; i ++) {
			for(int j = 0; j < height; j ++) {
				Vector3ld res = getPixel(i, j) + otherImage.getPixel(i,j);
				if(res.x < 0.0) res.x = 0.0;// else std::cout << "AAAH!";
				if(res.y < 0.0) res.y = 0.0;//else std::cout << "AAAH!";
				if(res.z < 0.0) res.z = 0.0;//else std::cout << "AAAH!";
				newImage.setPixel(i, j, res);
			}
		}
		return newImage;
	}
	
	LargeHdrImage operator*(long double f) {
		LargeHdrImage newImage(this -> resultingWidth, this -> resultingHeight, this -> MSAA);
		for(int i = 0; i < width; i ++) {
			for(int j = 0; j < height; j ++) {
				newImage.setPixel(i, j, getPixel(i, j) * f);
			}
		}
		return newImage;
	}
	
	LargeHdrImage(const LargeHdrImage& imageToCopy) {
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
		pixels = new Vector3ld*[width];
		samples = new int*[width];
		for(int i = 0; i < width; i ++) {
			pixels[i] = new Vector3ld[height]();
			samples[i] = new int[height]();
		}
	}
	
	
	LargeHdrImage smooth(int r) {
		LargeHdrImage smoothedImage(this -> resultingWidth, this -> resultingHeight, this -> MSAA);
		for(int i = 0; i < width; i ++) {
			for(int j = 0; j < height; j ++) {
				//smoothedImage.setPixel(i, j, smoothPixel(i, j, r));
				smoothedImage.addSample(smoothPixel(i, j, r), i, j);
			}
		}
		return smoothedImage;
	}
	
	
	
	LargeHdrImage sobel() {
		LargeHdrImage sobelImage(this -> resultingWidth, this -> resultingHeight, this -> MSAA);
		for(int i = 0; i < width; i ++) {
			for(int j = 0; j < height; j ++) {
				Vector3ld res = sobelPixel(i, j);
				sobelImage.addSample(res,i,j);
			}
		}
		return sobelImage;
	}
	
	Vector3 getResultingPixel(int x, int y) {
		Vector3ld pixelValue;
		for(int i = 0; i < MSAA; i ++) {
			for(int j = 0; j < MSAA; j ++) {
				int innerX = x * MSAA + i;
				int innerY = y * MSAA + j;
				pixelValue = pixelValue + getPixel(innerX, innerY) * (1.0 / (double) samples[innerX][innerY]);
			}
		}
		pixelValue =  pixelValue * (1.0 / (double)(MSAA * MSAA));
		Vector3 result(pixelValue.x, pixelValue.y, pixelValue.z);
		return result;
	}
	
	
	
	private:
	
	void addSample(const Vector3ld& sample, int x, int y) {
		samples[x][y]++;
		setPixel(x,y,sample + getPixel(x,y));
	}
	
	void setPixel(int x, int y, Vector3ld v) {
		pixels[x][y] = v;
	}
	
	Vector3ld getPixel(int x, int y) const {
		if(x >= width) {
			return Vector3ld();
		} else if(x < 0) {
			return Vector3ld();
		} else if(y >= height) {
			return Vector3ld();
		} else if(y < 0) {
			return Vector3ld();
		}
		return pixels[x][y];
	}
	
	int getPixelSamples(int x, int y) const {
		if(x >= width) {
			return 1;
		} else if(x < 0) {
			return 1;
		} else if(y >= height) {
			return 1;
		} else if(y < 0) {
			return 1;
		}
		return samples[x][y];
	}
	
	Vector3ld smoothPixel(int x, int y, int r) {
		int samples = 0;
		Vector3ld sum;
		for(int i = -r; i <= r; i ++) {
			for(int j = -r; j <= r; j ++) {
				if(i * i  + j * j  <=  r * r) {
					samples++;
					sum = sum + getPixel(x + i, y + j) * (1.0 / (long double)getPixelSamples(x + i, y + j));
				}
			}
		}
		sum = sum * (1.0f / (float) samples);
		return sum;
	}
	
	Vector3ld sobelPixel(int x, int y) {
		Vector3ld sum;
		float weights [3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
		Vector3ld derX, derY;
		for(int i = -1; i <= 1; i ++) {
			for(int j = -1; j <= 1; j ++) {
				Vector3ld pixel = getPixel(x + i, y + j) * (1.0 / (long double)getPixelSamples(x + i, y + j));
				derX = derX + pixel * weights[i + 1][j + 1];
				derY = derY + pixel * weights[j + 1][i + 1];
			}
		}
		Vector3ld grad = derX * derX + derY * derY;
		return Vector3ld(sqrtf(grad.x), sqrtf(grad.y), sqrtf(grad.z));
	}
	
};
