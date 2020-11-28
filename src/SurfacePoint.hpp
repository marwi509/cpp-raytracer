#ifndef _SurfacePoint_HPP
#define _SurfacePoint_HPP

#include "Vector3.hpp"
#include "Polygon.hpp"
#include "Object.hpp"



class SurfacePoint
{
	public:
		const Vector3
			position,
			normal,
			tangent,
		incomingDirection;
		
	const Material* material;
	const Polygon* polygon;
	const Object* object;
	bool light;
	int lightIndex = -1;
	
	SurfacePoint(
		const Vector3& _position,
		const Vector3& _normal,
		const Vector3& _incomingDirection,
		const Material* _material,
		const Polygon* _polygon,
		const Object* _object,
		const bool _light
	): position(_position), 
		normal(_normal), 
		tangent(normal.cross(Vector3(randf(), randf(), randf()).norm())), 
		incomingDirection(_incomingDirection), 
		material(_material), 
		object(_object), 
		light(_light) {
		/*position = _position;
		normal = _normal;
		tangent = normal.cross(Vector3(randf(), randf(), randf()).norm());
		incomingDirection = _incomingDirection;
		material = _material;
		light = _light;*/
	}

	static SurfacePoint* SurfacePoint::LIGHT(const Material* _material) {
		return new SurfacePoint(Vector3(), Vector3(), Vector3(), _material, NULL, NULL, true);
	}

};
#endif