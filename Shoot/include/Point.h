/* 

Amine Rehioui
Created: April 11th 2010

*/

#pragma once


namespace shoot
{
	// forwards
	class Vector2; // operator = (const Vector2&)

	//! 2D point class
	class Point
	{
	public:

		//! constructor
		Point() : X(0), Y(0)
		{
		}

		//! constructor
		Point(int x, int y) : X(x), Y(y)
		{
		}
		
		// operators
		Point operator + (const Point& other) const;		
		Point operator - (const Point& other) const;		
		Point& operator += (const Point& other);
		Point& operator -= (const Point& other);
		Point& operator = (const Vector2& vec2D);

		inline bool operator == (const Point& other) const
		{
			return (X == other.X) && (Y == other.Y);
		}

		inline bool operator != (const Point& other) const
		{
			return !this->operator==(other);
		}

		int X, Y;
	};
}



