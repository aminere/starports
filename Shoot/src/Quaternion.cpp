/* 

Amine Rehioui
Created: April 10th 2012

*/

#include "Shoot.h"

#include "Quaternion.h"

namespace shoot
{
	//! constructor
	Quaternion::Quaternion(const Vector3& vUnitAxis, float fAngle)
	{ 
		float halfAngle = fAngle/2.0f;       
		W = Math::Cos(halfAngle);
		V = vUnitAxis*Math::Sin(halfAngle);
	}

	//! normalizes the Quaternion
	Quaternion Quaternion::Normalize() const
	{
		Quaternion result = *this;
		float length = GetLength();
		if(length > Math::Epsilon)
		{
			float invLength = 1.0f/length;
			result.X *= invLength;
			result.Y *= invLength;
			result.Z *= invLength;
			result.W *= invLength;
		}
		return result;
	}

	//! rotates a vector
	Vector3 Quaternion::Rotate(const Vector3& v) const
    {
		// Irrlicht / nVidia SDK implementation
		Vector3 qvec = V;
		Vector3 uv = qvec.CrossProduct(v);
		Vector3 uuv = qvec.CrossProduct(uv);
		uv *= (2.0f * W);
		uuv *= 2.0f;
		return v + uv + uuv;
    }

	//! returns a matrix representation
	Matrix44 Quaternion::GetMatrix()
	{
		float xx = X * X;
        float yy = Y * Y;
        float zz = Z * Z;
        float xy = X * Y;
        float xz = X * Z;
        float yz = Y * Z;
        float wx = W * X;
        float wy = W * Y;
        float wz = W * Z;   
        Matrix44 m;
        m[0] = 1.0f - (yy + zz)*2.0f;
        m[1] = (xy + wz)*2.0f;
        m[2] = (xz - wy)*2.0f;
        m[3] = 0.0f;
        m[4] = (xy - wz)*2.0f;
        m[5] = 1.0f - (xx + zz)*2.0f;
        m[6] = (yz + wx)*2.0f;
        m[7] = 0.0f;
        m[8] = (xz + wy)*2.0f;
        m[9] = (yz - wx)*2.0f;
        m[10] = 1.0f - (xx + yy)*2.0f;
        m[11] = 0.0f;
        m[12] = 0.0f;
        m[13] = 0.0f;
        m[14] = 0.0f;
        m[15] = 1.0f;
        return m;
	}

	// operators
	Quaternion Quaternion::operator * (const Quaternion& other) const
	{
		float w = W*other.W - V.DotProduct(other.V);
        Vector3 v = other.V*W + V*other.W + V.CrossProduct(other.V);
        return Quaternion(v.X, v.Y, v.X, w);
	}	
}

