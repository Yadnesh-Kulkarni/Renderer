// Vec2 header

class Vector2D{
private:
	float x,y;
public:
	Vector2D(float a, float b)
	{
		x = a;
		y = b;
	}

	inline float x() const {	return x;	}
	inline float y() const {	return y;	}


	float& operator [](int i)
	{
		return ((&x)[i]);
	}

	const float& operator [] (int i) const
	{
		return ((&x)[i]);
	}
};