
#include "Camera.hpp"

class RenderSettings {

private:
	int width;
	int height;
	int msaa;
	Camera* camera;
	const char* filename;
	int passes;
	int frameSmooth;

public:
	RenderSettings() {
		width = 1024;
		height = 600;
		msaa = 2;
		camera = new Camera(msaa * width, msaa * height, 14.0, 0.6, Vector3());
	}

	RenderSettings(
		int width,
		int height,
		int msaa,
		double focalDistance,
		double apertureSize,
		const Vector3& cameraPosition,
		const char* filename,
		int passes,
		int frameSmooth) {
		this->width = width;
		this->height = height;
		this->msaa = msaa;
		this->camera = new Camera(width * msaa, height * msaa, focalDistance, apertureSize, cameraPosition);
		this->filename = filename;
		this->passes = passes;
		this->frameSmooth = frameSmooth;
	}

	int getWidth() const {
		return width;
	}

	int getHeight() const {
		return height;
	}

	int getMsaa() const {
		return msaa;
	}

	Camera* getCamera() const {
		return camera;
	}

	const char* getFileName() const {
		return filename;
	}

	int getPasses() const {
		return passes;
	}

	void rotateCamera(int axis, float degrees) {
		camera->rotate(axis, degrees);
	}

	int getFrameSmooth() const {
		return frameSmooth;
	}
};
