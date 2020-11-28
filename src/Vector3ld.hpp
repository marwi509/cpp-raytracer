#include "constants.hpp"


#ifndef _Vector3ldLD_HPP
#define _Vector3ldLD_HPP

/** Vektor-klass som innehåller trfoate floats och lite vanliga vektorberäkningar **/
class Vector3ld
{
	public:
	long double x,y,z;
	
	/* Tom konstruktor */
	Vector3ld()
	{
		x = y = z = 0.0;
	}
	
	/* Konstruktor som tar tre floats */
	Vector3ld(double xIn, double yIn, double zIn)
	{
		x = xIn;
		y = yIn;
		z = zIn;
	}
	
	/* Skalärprodukt */
	double dot(const Vector3ld& v) const
	{
		return x*v.x + y*v.y + z*v.z;
	}
	
	/* Kryssprodukt */
	Vector3ld cross(const Vector3ld& v) const
	{
		return Vector3ld(y*v.z - z*v.y, - x*v.z + z*v.x, x*v.y - y*v.x);
	}
	
	/* Multiplikation, term för term */
	Vector3ld operator*(const Vector3ld& v) const
	{
		return Vector3ld(x*v.x, y*v.y, z*v.z);
	}
	
	/* Multiplikation med flyttal */
	Vector3ld operator*(double f) const
	{
		return Vector3ld(x*f, y*f, z*f);
	}
	
	/* Subtraktion */
	Vector3ld operator-(const Vector3ld& v) const
	{
		return Vector3ld(x-v.x, y-v.y, z-v.z);
	}
	
	/* Addition */
	Vector3ld operator+(const Vector3ld& v) const
	{
		return Vector3ld(x+v.x, y+v.y, z+v.z);
	}
	
	/* Hämta längden på vektorn i kvadrat */
	long double square() const
	{
		return (x*x + y*y + z*z);
	}
	
	/* Hämta längden på vektorn */
	long double length() const
	{
		return sqrtf(square());
	}
	
	/* Normera vektorn */
	Vector3ld norm() const
	{
		long double len = 1.0 / length();
		//x = x * len;
		//y = y * len;
		//z = z * len;
		return Vector3ld(x * len, y * len, z * len);;
	}
	
	long double sum() const
	{
		return x + y + z;
	}
	
	Vector3ld floor()
	{
		return Vector3ld(std::floor(x), std::floor(y), std::floor(z));
	}
	
	Vector3ld operator=(double f)
	{
		return Vector3ld(f,f,f);
	}
	long double operator[](unsigned int i) const
	{
		if(i == 0)
			return x;
		if(i == 1)
			return y;
		return z;
		
	}

};


#endif

