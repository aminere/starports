/* 

Amine Rehioui
Created: February 28th 2010

*/

#pragma once

#include "Size.h"

namespace shoot
{
	//! 2D vector class
	class Vector2
	{
	public:

		inline static Vector2 Create(float x, float y)
		{
			Vector2 vec = { x, y };
			return vec;
		}
		
		//! Sets the coordinates
		inline void Set(float x, float y) 
		{
			X = x;
			Y = y;
		}

		//! returns the length
		float GetLength() const;

		//! returns the length squared
		float GetLengthSquared() const;

		//! returns the dot product
		float DotProduct(const Vector2& other) const;

		//! returns a rotated vector
		Vector2 Rotate(float fAngleRadians, const Vector2& vOrigin = Vector2::Zero) const;

		//! returns the angle in radians with respect to another vector
		float GetAngle(const Vector2& axe) const;

		//! normalizes the vector
		Vector2& Normalize();

		//! normalizes the vector
		Vector2 Normalize() const;

		//! clamps the vector
		inline const Vector2& Clamp(const Vector2& vMin, const Vector2& vMax)
		{
			return (*this = static_cast<const Vector2*>(this)->Clamp(vMin, vMax));
		}

		//! clamps the vector
		Vector2 Clamp(const Vector2& vMin, const Vector2& vMax) const;

		// operators
		inline Vector2 operator + (const Vector2& other) const { return Vector2::Create(X+other.X, Y+other.Y); }
		inline Vector2 operator - (const Vector2& other) const { return Vector2::Create(X-other.X, Y-other.Y); }
		inline Vector2 operator * (const Vector2& other) const { return Vector2::Create(X*other.X, Y*other.Y); }
		inline Vector2 operator / (const Vector2& other) const { return Vector2::Create(X/other.X, Y/other.Y); }
		inline Vector2 operator * (const float fValue) const { return Vector2::Create(X*fValue, Y*fValue); }
		inline Vector2 operator / (const float fValue) const { return Vector2::Create(X/fValue, Y/fValue); }
		inline Vector2& operator /= (const float fValue) { X /= fValue; Y /= fValue; return *this; }
		inline Vector2& operator += (const Vector2& other) { X += other.X; Y += other.Y; return *this; }
		inline Vector2& operator -= (const Vector2& other) { X -= other.X; Y -= other.Y; return *this; }
		inline Vector2& operator *= (const Vector2& other) { X *= other.X; Y *= other.Y; return *this; }		
		inline bool operator != (const Vector2& other) const { return !(*this == other); }
		inline Vector2& operator = (const Size& size) { X = float(size.Width); Y = float(size.Height);return *this; }
		inline Vector2& operator *= (const float fValue) { X *= fValue; Y *= fValue; return *this; }		
		inline Vector2 operator -() const {	return Vector2::Create(-X, -Y);	}
		bool operator == (const Vector2& other) const;
		
		float X, Y;

		static Vector2 Zero;
		static Vector2 One;
	};
}

