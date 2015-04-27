
class RenderSettings {
	
	private:
	int width;
	int height;
	double focalDistance;
	double apertureSize;
	
	public:
	RenderSettings() {
		width = 1024;
		height = 600;
		focalDistance = 14.0;
		apertureSize = 0.6;
	}
	
	RenderSettings(int width, int height, double focalDistance, double apertureSize) {
		this -> width = width;
		this -> height = height;
		this -> focalDistance = focalDistance;
		this -> apertureSize = apertureSize;
	}
	
	int getWidth() const {
		return width;
	}
	
	int getHeight() const {
		return height;
	}
	
	double getFocalDistance() const {
		return focalDistance;
	}
	
	double getApertureSize() const {
		return apertureSize;
	}
};
