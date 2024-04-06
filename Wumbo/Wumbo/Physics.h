#pragma once
#include <stdlib.h>
#include <cmath>

class Coord2 {
public:
	float x = 0.0f;
	float y = 0.0f;

	Coord2()
	{}

	Coord2(float a, float b) : x(a), y(b)
	{}

	// Vector addition (a + b)
	friend Coord2 operator+(const Coord2& a, const Coord2& b)
	{
		return Coord2(a.x + b.x, a.y + b.y);
	}

	// Vector subtraction (a - b)
	friend Coord2 operator-(const Coord2& a, const Coord2& b)
	{
		return Coord2(a.x - b.x, a.y - b.y);
	}

	// Scalar multiplication
	Coord2& operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}


	// Member variable
	float length()
	{
		return static_cast<float>(sqrt(pow(x, 2) + (x, 2)));
	}

	void normalize()
	{
		float len = length();
		x /= len;
		y /= len;
	}

	static Coord2 normalize(const Coord2& coord)
	{
		Coord2 temp = coord;
		temp.normalize();
		return temp;
	}

	static float distance(Coord2 a, Coord2 b)
	{
		return sqrt(pow(a.x - b.x, 2.0f) + pow(b.x - b.y, 2.0f));
	}
};
