/* 

Amine Rehioui
Created: March 20th 2010

*/

#include "Shoot.h"

#include "Vector2.h"

namespace shoot
{
	//! static variables initialization
	Vector2 Vector2::Zero = Vector2::Create(0.0f, 0.0f);
	Vector2 Vector2::One = Vector2::Create(1.0f, 1.0f);

	//! returns the length
	float Vector2::GetLength() const
	{
		float length = Math::SquareRoot((X*X) + (Y*Y));
		return length;
	}

	//! returns the length squared
	float Vector2::GetLengthSquared() const
	{
		float lengthSquared = (X*X) + (Y*Y);
		return lengthSquared;
	}

	//! returns the dot product
	float Vector2::DotProduct(const Vector2& other) const
	{
		float dotProduct = (X*other.X) + (Y*other.Y);
		return dotProduct;
	}

	//! returns a rotated vector
	Vector2 Vector2::Rotate(float fAngleRadians, const Vector2& vOrigin /*= Vector2::Create(0.0f, 0.0f)*/) const
	{
		// TODO: handle non zero origin
		Vector2 result;
		result.X = X*Math::Cos(fAngleRadians) - Y*Math::Sin(fAngleRadians);
		result.Y = X*Math::Sin(fAngleRadians) + Y*Math::Cos(fAngleRadians);
		return result;
	}

	//! returns the angle in radians with respect to another vector
	float Vector2::GetAngle(const Vector2& axe) const
	{
		auto axeAngle = Math::ATan2(axe.Y, axe.X);
		if(axeAngle < 0.0f)
			axeAngle += Math::PITimes2;

		auto thisAngle = Math::ATan2(Y, X);
		if(thisAngle < 0.0f)
			thisAngle += Math::PITimes2;

		return thisAngle - axeAngle;
	}

	//! normalizes the vector
	Vector2& Vector2::Normalize()
	{
		const Vector2& constPtr = *this;
		*this = constPtr.Normalize();
		return *this;
	}

	//! normalizes the vector
	Vector2 Vector2::Normalize() const
	{
		Vector2 result = *this;
		float length = GetLength();
		if(length > Math::Epsilon)
		{
			result.X /= length;
			result.Y /= length;
		}
		return result;
	}

	//! clamps the vector
	Vector2 Vector2::Clamp(const Vector2& vMin, const Vector2& vMax) const
	{
		Vector2 vOut;
		vOut.X = Math::Clamp(X, vMin.X, vMax.X);
		vOut.Y = Math::Clamp(Y, vMin.Y, vMax.Y);
		return vOut;
	}

	// operators
	bool Vector2::operator == (const Vector2& other) const
	{
		return Math::FEqual(X, other.X) && Math::FEqual(Y, other.Y);
	}
}

