

class Vertex
{
	public:
	Vector3 position, normal;
	float u,v;
	
	/* Enkel konstruktor */
	Vertex(const Vector3& p, const Vector3& n)
	{
		position = p;
		normal = n;
	}
	
	/* Enklare konstruktor */
	Vertex(const Vector3& p)
	{
		position = p;
	}
	
	/* Tom konstruktor */
	Vertex(){}
};
