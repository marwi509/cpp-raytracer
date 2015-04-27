/** Class that contains a polygon mesh and a light color
 * 		currently kind of hard coded to the square light in the ceiling (at least when emitter sampling is used)*/

class Light : public Object
{
	public:
	Vector3 scaleSaved;
	void scale(const Vector3& scale);
	Vector3 color;
	Vector3 getRandomPoint();
	Vector3 normal;
	Light(const char* filename, const Vector3& colorIn);
	Light(){normal = Vector3(0,1,0);}
	float getArea();
};
