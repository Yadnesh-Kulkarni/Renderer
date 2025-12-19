// Vec3 header

class Vector3D {
private :
	float x;
	float y;
	float z;

protected :

public :
	inline Vector3D()
	{
		x=0;
		y=0;
		z=0;
	}

	inline Vector3D(float a, float b, float c)
	{
		x = a;
		y = b;
		z = c;
	}

	inline float getX() const { return x; } 
	inline float getY() const { return y; } 
	inline float getZ() const { return z; } 
	
	float& operator [](int i)
	{
		return ((&x)[i]);
	}

	const float& operator [](int i) const
	{
		return ((&x)[i]);
	}
};