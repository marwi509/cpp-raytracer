


#ifndef _CAMERA_HPP
#define _CAMERA_HPP

#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Aperture.hpp"
#include <iostream>

class Camera {
public:

	const int sampleWidth;
	const int sampleHeight;
	double focalDistance;
	const double apertureSize;
	Vector3 position;
	Vector3 lookDirection = Vector3(0,0,1);
	Vector3 xDirection = Vector3(1, 0, 0);
	Vector3 yDirection = Vector3(0, 1, 0);

	Camera() : Camera(1024, 600, 14.0, 0.6, Vector3()) {}

	Camera(const int _width, const int _height, const double _focalDistance, const double _apertureSize, const Vector3& _position) : 
		sampleWidth(_width), sampleHeight(_height), focalDistance(_focalDistance), apertureSize(_apertureSize), position(_position) {
	}

	Ray getRay(int i, int j) {
		Ray originalRay = getStartingRay(i, j);
		Vector3 focal_point = originalRay * focalDistance;
		Vector2 randomizedOffset = randomizeAperturePosition(apertureSize);
		Vector3 new_pos = originalRay.position + xDirection * randomizedOffset.x + yDirection * randomizedOffset.y;

		return Ray(new_pos, (focal_point - new_pos).norm());
	}

	Ray getStartingRay(int i, int j) {
		float scale = 1.0f;
		float scaleAngle = 1.0f;
		float ar = (float)sampleHeight / (float)sampleWidth;
		float x = -1.0f * scale + scale * 2.0f * (float)j / (float)sampleWidth;
		float y = -ar * scale + scale * 2.0f * ar * (float)i / (float)sampleHeight;

		Vector3 position = (this -> position) + xDirection *x + yDirection * y;
		Vector3 direction = lookDirection + xDirection * (scaleAngle * x / scale) + yDirection * (scaleAngle * y / scale); //Vector3(x / scale, y / scale, 1).norm();
		return Ray(position, direction.norm());
	}

	void rotate(int axis, float degree) {
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

		lookDirection = multiply(M, lookDirection);
		xDirection = multiply(M, xDirection);
		yDirection = multiply(M, yDirection);
	}

	void lookAt(Object* object) {
		Vector3 lookAt = object->findNearestPoint(Vector3(5, 0, 16.5));
		this->focalDistance = (this->position - lookAt).length();
		Vector3 newLookAt = (lookAt - this->position).norm();
		Vector3 zForYRotation = Vector3(newLookAt.x, 0, newLookAt.z).norm();
		Vector3 xForYRotation = Vector3(lookAt.x, 0, lookAt.z).norm();

		float yRotation = acosf(zForYRotation.dot(xForYRotation));
		

		Vector3 zForXRotation = Vector3(0, newLookAt.y, newLookAt.z).norm();
		Vector3 xForXRotation = Vector3(0, lookAt.y, lookAt.z).norm();

		float xRotation = acosf(zForXRotation.dot(xForXRotation));
		float zDirection = Vector3(0, 0, 1).dot(newLookAt);

		std::cout << xRotation << std::endl;
		std::cout << yRotation << std::endl;
		this->rotate(1, xRotation);
		this->rotate(2, yRotation);
	}

private:

	Vector3 multiply(float M[3][3], Vector3 vector) {
		return Vector3(
			M[0][0] * vector.x + M[1][0] * vector.y + M[2][0] * vector.z,
			M[0][1] * vector.x + M[1][1] * vector.y + M[2][1] * vector.z,
			M[0][2] * vector.x + M[1][2] * vector.y + M[2][2] * vector.z
		);
	}

	Vector2 randomizeAperturePosition(float apertureSize) {
		int err_check = 0;
		float x_random, y_random;
		do
		{
			x_random = (randf() - randf()) * apertureSize;
			y_random = (randf() - randf()) * apertureSize;
		} while (!parametricReject(CIRCLE, apertureSize, x_random, y_random)
			&& err_check++ < 100 && apertureSize > 0.01f);

		return Vector2(x_random, y_random);
	}

	const int CIRCLE = 0;
	const int SQUARE = 1;
	const int SQUIRCLE = 2;
	const int ELIPSOID = 3;

	bool rejectCircle(float radius, float x, float y)
	{
		return x * x + y * y < radius* radius;
	}

	bool rejectSquircle(float radius, float x, float y)
	{
		return x * x * x * x + y * y * y * y < radius* radius* radius* radius;
	}

	bool rejectSquare(float radius, float x, float y)
	{
		return max(fabsf(x), fabsf(y), -10000.0f) < radius;
	}

	bool rejectElips(float radius, float x, float y)
	{
		return x * x + 4.0f * y * y < radius* radius;
	}

	bool parametricReject(int variant, float radius, float x, float y)
	{
		if (variant == CIRCLE)
			return rejectCircle(radius, x, y);
		if (variant == SQUIRCLE)
			return rejectSquircle(radius, x, y);
		if (variant == SQUARE)
			return rejectSquare(radius, x, y);
		if (variant == ELIPSOID)
			return rejectElips(radius, x, y);
		return false;
	}

	bool ngonReject(int npol, float radius, float x, float y)
	{
		//npol += 2;
		float* xpositions = new float[npol];
		float* ypositions = new float[npol];
		int rrr = 0;

		for (float theta2 = 0.0f; rrr < npol; theta2 += 2.0f * 3.14 / float(npol))
		{
			xpositions[rrr] = radius * cos(theta2);
			ypositions[rrr++] = radius * sin(theta2);
		}
		return (pnpoly(npol, xpositions, ypositions, x, y));

	}

};

#endif

