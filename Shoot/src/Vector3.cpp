/* 

Amine Rehioui
Created: March 20th 2010

*/

#include "Shoot.h"

#include "Quaternion.h"

namespace shoot
{
	//! static variables initialization
	Vector3 Vector3::Up = Vector3::Create(0.0f, 0.0, 1.0f);
	Vector3 Vector3::Zero = Vector3::Create(0.0f, 0.0f, 0.0f);
	Vector3 Vector3::One = Vector3::Create(1.0f, 1.0f, 1.0f);

	//! returns a normalized vector
	Vector3 Vector3::Normalized() const
	{
		Vector3 result = *this;
		float length = GetLength();
		if(length > Math::Epsilon)
		{
			float invLength = 1.0f/length;
			result.X *= invLength;
			result.Y *= invLength;
			result.Z *= invLength;
		}
		return result;
	}

	//! Limits the components to maximum and resizes proportionally if needed
	const Vector3& Vector3::Limit(float fMax)
	{
		float fMaxXYZ = Math::Max(Math::FAbs(X), Math::FAbs(Y), Math::FAbs(Z));
		if(fMaxXYZ > fMax)
		{
			float fRatio = fMax/fMaxXYZ;
			*this *= fRatio;
		}
		return *this;
	}

	//! clamps the vector
	Vector3 Vector3::Clamp(const Vector3& vMin, const Vector3& vMax) const
	{
		Vector3 vOut;
		vOut.X = Math::Clamp(X, vMin.X, vMax.X);
		vOut.Y = Math::Clamp(Y, vMin.Y, vMax.Y);
		vOut.Z = Math::Clamp(Z, vMin.Z, vMax.Z);
		return vOut;
	}

	//! rotates the vector
	Vector3 Vector3::Rotate(const Vector3& vRotationRadians) const
	{		
		Matrix44 matrix;
		matrix.SetRotation(vRotationRadians);
		Vector3 result = *this;
		result = matrix.TransformVect(result);
		return result;
	}

	//! rotates the vector around an axis
	Vector3 Vector3::Rotate(const Vector3& vAxis, float fAngleRadians) const
	{
		return Quaternion(vAxis, fAngleRadians).Rotate(*this);
	}

	//! differentiates this vector from the Up vector
	Vector3 Vector3::DifferentiateFromUp() const
	{
		// Assumes UP is (0, 0, 1)
		Vector3 vOut = *this;
		static const float treshold = 0.005f;
		if(vOut.X + vOut.Y < treshold && vOut.Z > 1.0f-treshold)
		{
			(vOut.X > vOut.Y) ? (vOut.X = treshold) : (vOut.Y = treshold);
			vOut.Z = 1.0f-treshold;
		}
		return vOut;
	}

	bool Vector3::operator == (const Vector3& other) const
	{
		return Math::FEqual(X, other.X) 
			&& Math::FEqual(Y, other.Y) 
			&& Math::FEqual(Z, other.Z);
	}
}

