/* 

Amine Rehioui
Created: April 11th 2010

*/

#include "Shoot.h"

#include "Point.h"

#include "Vector2.h"

namespace shoot
{	
	// operators
	Point Point::operator + (const Point& other) const
	{
		Point result(X+other.X, Y+other.Y);
		return result;
	}

	Point Point::operator - (const Point& other) const
	{
		Point result(X-other.X, Y-other.Y);
		return result;
	}

	Point& Point::operator += (const Point& other)
	{
		X += other.X;
		Y += other.Y;
		return *this;
	}

	Point& Point::operator -= (const Point& other)
	{
		X -= other.X;
		Y -= other.Y;
		return *this;
	}

	Point& Point::operator = (const Vector2& vec2D)
	{
		X = int(vec2D.X);
		X = int(vec2D.Y);
		return *this;
	}
}

