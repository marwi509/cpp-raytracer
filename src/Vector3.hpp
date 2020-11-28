#include "constants.hpp"


#ifndef _VECTOR3_HPP
#define _VECTOR3_HPP

/** Vektor-klass som innehåller tre floats och lite vanliga vektorberäkningar **/
class Vector3
{
	public:
	float x,y,z;
	
	Vector3() : x(0.0f), y(0.0f), z(0.0f) {}

	/* Konstruktor som tar tre floats */
	Vector3(float xIn, float yIn, float zIn) :x(xIn), y(yIn), z(zIn)
	{
	}

	/*const Vector3 operator=(const Vector3& right) {
		if (this == &right) return *this;

		// manually call the destructor of the old left-side object
		// (`this`) in the assignment operation to clean it up
		//this->~A();
		// use "placement new" syntax to copy-construct a new `A` 
		// object from `right` into left (at address `this`)
		new (this) Vector3(right);
		return *this;
	}*/
	
	/* Skalärprodukt */
	float dot(const Vector3& v) const
	{
		return x*v.x + y*v.y + z*v.z;
	}
	
	/* Kryssprodukt */
	Vector3 cross(const Vector3& v) const
	{
		return Vector3(y*v.z - z*v.y, - x*v.z + z*v.x, x*v.y - y*v.x);
	}
	
	/* Multiplikation, term för term */
	Vector3 operator*(const Vector3& v) const
	{
		return Vector3(x*v.x, y*v.y, z*v.z);
	}
	
	/* Multiplikation med flyttal */
	Vector3 operator*(float f) const
	{
		return Vector3(x*f, y*f, z*f);
	}
	
	/* Subtraktion */
	Vector3 operator-(const Vector3& v) const
	{
		return Vector3(x-v.x, y-v.y, z-v.z);
	}
	
	/* Addition */
	Vector3 operator+(const Vector3& v) const
	{
		return Vector3(x+v.x, y+v.y, z+v.z);
	}
	
	/* Hämta längden på vektorn i kvadrat */
	float square() const
	{
		return (x*x + y*y + z*z);
	}
	
	/* Hämta längden på vektorn */
	float length() const
	{
		return sqrtf(square());
	}
	
	/* Normera vektorn */
	Vector3 norm() const
	{
		float len = 1.0f / length();
		//x = x * len;
		//y = y * len;
		//z = z * len;
		return Vector3(x * len, y * len, z * len);;
	}
	
	float sum() const
	{
		return x + y + z;
	}
	
	Vector3 floor()
	{
		return Vector3(floorf(x), floorf(y), floorf(z));
	}
	
	Vector3 operator=(float f)
	{
		return Vector3(f,f,f);
	}
	float operator[](unsigned int i) const
	{
		if(i == 0)
			return x;
		if(i == 1)
			return y;
		return z;
		
	}
};




float max(float f1, float f2, float f3);

//http://t1.gstatic.com/images?q=tbn:ANd9GcR61sL6m9z2zqWAVR1VX8SjWn8Dl0SRvuA6N3cGIaDcjmuto5vB9xJlmubF
float clamp(float value, float min, float max);
float fract(float value);
float step(float value, float limit);



float randf();


#endif

