#ifndef _MATERIAL_HPP
#define _MATERIAL_HPP

#include "Noise3.hpp"
#include <cstdlib>

/** Class containing material information for an object, like color, reflectivity, transparency and refraction index */

class Material
{
	public:
	
	static Material* PORCELAIN(const Vector3& diffuseColor) {
		Material* materialOut = new Material();
		materialOut -> specularColor = Vector3(1,1,1);
		materialOut -> reflective = 0.1f;
		materialOut -> diffuseColor = diffuseColor;
		return materialOut;
	}
	
	static Material* MIRROR() {
		Material* materialOut = new Material();
		materialOut -> specularColor = Vector3(1,1,1);
		materialOut -> reflective = 1.0f;
		return materialOut;
	}
	
	static Material* GLASS(const Vector3& color) {
		Material* materialOut = new Material();
		materialOut -> specularColor = color;
		materialOut -> refractive = 1.0f;
		materialOut -> refrI = 1.5f;
		return materialOut;
	}
	
	static Material* GOLD() {
		Material* materialOut = new Material();
		materialOut -> specularColor = Vector3(12.0f * 16.0f + 13.0f,7.0f * 16.0f + 16.0f,3.0f * 16.0f + 2.0f) * (1.0f / 255.0f);
		materialOut -> reflective = 1.0f;
		return materialOut;
	}
	
	static Material* DIFFUSE(const Vector3 diffuseColor) {
		Material* materialOut = new Material(diffuseColor);
		return materialOut;
	}
	
	bool check, brick;
	float checkfreq;
	
	Vector3 
	
		/* The diffuse color of the object */
		diffuseColor,
		
		/* The specular color of the object */
		specularColor;
		
	float
	
		/* How reflective the object is */
		reflective,
		
		/* How rough the specular surface is, 0 being a perfect mirror and 0.5 being really diffuse */
		blur,
		
		/* refraction-index */
		refrI,
		
		/* How transparent the object is */
		refractive;
	
	
	bool metal, texture;
	
	/* Noise texture for the object, not always used */
	Noise3* noise;
	float noiseStrength;
	
	private:
	
	/** simple constructor with a diffuse color as input */
	Material(const Vector3& diffColor)
	{
		noise = NULL;
		diffuseColor = diffColor;
		specularColor = Vector3(1,1,1);
		reflective = 0.0f;
		blur = 0.001f;
		refractive = 0.0f;
		refrI = 1.0f;
		metal = texture = check = brick = false;
		checkfreq = 0.1f;
	}
	
	/** Empty constructor */
	Material()
	{
		noise = NULL;
		specularColor = Vector3(1,1,1);
		reflective = refractive = 0.0f;
		blur = 0.001f;
		refrI = 1.0f;
		metal = texture = check = brick = false;
		checkfreq = 0.1f;
	}
};


#endif
