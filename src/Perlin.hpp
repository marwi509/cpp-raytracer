

class Perlin
{
	public:
	float eps1, eps2, eps3, eps4, eps5, eps6, eps7, eps8;
	Perlin()
	{
		eps1 = randf() * 3.0f;
		eps2 = randf() * 3.0f;
		eps3 = randf() * 3.0f;
		eps4 = randf() * 3.0f;
		eps5 = randf() * 3.0f;
		eps6 = randf() * 3.0f;
		eps7 = randf() * 3.0f;
		eps8 = randf() * 3.0f;
	}
	
	float getValue(float u, float v)
	{
		float val = 
				powf(
				(fabsf(sin(eps1 * 2.0f * M_PI * u)))*0.1f +
				(fabsf(sin(eps2 * 2.0f * M_PI * u)))*0.1f +
				(fabsf(sin(eps3 * 2.0f * M_PI * u)))*0.1f +
				(fabsf(sin(eps4 * 2.0f * M_PI * u)))*0.1f +
				(fabsf(sin(eps5 * 2.0f * M_PI * v)))*0.1f +
				(fabsf(sin(eps6 * 2.0f * M_PI * v)))*0.1f +
				(fabsf(sin(eps7 * 2.0f * M_PI * v)))*0.1f +
				(fabsf(sin(eps8 * 2.0f * M_PI * v)))*0.1f
				,0.5f
				);
				//fabsf(sin(eps3 * 2.0f * M_PI * v))*0.3f;
		if(val < 0.0f)
			val = 0.0f;
		else if(val > 1.0f)
			val = 1.0f;
		//cout << val << endl;
		return val;
	}
};




