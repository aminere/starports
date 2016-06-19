/* 

Amine Rehioui
Created: September 4th 2012

*/

#include "Shoot.h"

#include "Vector4.h"

namespace shoot
{
	//! static variables initialization
	Vector4 Vector4::Zero = Vector4::Create(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4 Vector4::One = Vector4::Create(1.0f, 1.0f, 1.0f, 1.0f);

	//! normalizes the vector
	Vector4 Vector4::Normalize() const
	{
		Vector4 result = *this;
		float length = GetLength();
		if(length > Math::Epsilon)
		{
			result.X /= length;
			result.Y /= length;
			result.Z /= length;
			result.W /= length;
		}
		return result;
	}

	bool Vector4::operator == (const Vector4& other) const
	{
		return Math::FEqual(X, other.X) 
			&& Math::FEqual(Y, other.Y) 
			&& Math::FEqual(Z, other.Z)
			&& Math::FEqual(W, other.W);
	}
}

