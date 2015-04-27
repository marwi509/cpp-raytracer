#include <vector>

#include "Vector3.hpp"
#include "Ray.hpp"
#include "Vertex.hpp"
#include "Material.hpp"
#include "Polygon.hpp"
#include "Cell.hpp"
#include "Object.hpp"
#include "Light.hpp"


/** Constructor for a lightsource, currently more or less hardcoded to the square light in the ceiling */
Light::Light(const char* filename, const Vector3& colorIn)
{
	material = Material::DIFFUSE(colorIn);
	readObjFile(filename);
	
}



void Light::scale(const Vector3& scale) {
	scaleSaved = scale;
	material -> diffuseColor = material -> diffuseColor * (1.0f / (scale.x*scale.y + scale.x*scale.z + scale.z*scale.y));
	Object::scale(scale);
}


Vector3 Light::getRandomPoint()
{
	return Vector3(translation.x+(randf() - randf())*scaleSaved.x,translation.y+(randf() - randf())*scaleSaved.y,translation.z +(randf() - randf())*scaleSaved.z);
}

float Light::getArea() {
	return scaleSaved.x*scaleSaved.y + scaleSaved.x*scaleSaved.z + scaleSaved.z*scaleSaved.y;
}
