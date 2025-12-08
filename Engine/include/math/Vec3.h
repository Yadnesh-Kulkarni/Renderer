// Vec3 header
class Vector3D {
private :
	float x,y,z;

protected :

public:
	Vector3D()
	{
		x=0;
		y=0;
		z=0;
	}

	Vector3D(float a, float b, float c)
	{
		x = a;
		y = b;
		z = c;
	}

	inline float x() const { return x; } 
	inline float y() const { return y; } 
	inline float z() const { return z; } 
	
	float& operator [](int i)
	{
		return ((&x)[i]);
	}

	const float& operator [](int i) const
	{
		return ((&x)[i]);
	}
};