/* 

Amine Rehioui
Created: April 10th 2012

*/

#pragma once


namespace shoot
{
	//! Quaternion
	class Quaternion
	{
	public:

		//! constructor
		Quaternion()
		{
		}

		//! constructor
		Quaternion(float x, float y, float z, float w)
			: X(x)
			, Y(y)
			, Z(z)
			, W(w)
		{
		}

		//! constructor
		/** \param vUnitAxis Axe of rotation 
			\param fAngle Angle of rotation in Radians */
		Quaternion(const Vector3& vUnitAxis, float fAngle);

		//! normalizes the Quaternion
		Quaternion& Normalize()
		{
			return (*this = static_cast<const Quaternion*>(this)->Normalize());
		}

		//! normalizes the Quaternion
		Quaternion Normalize() const;

		//! inverses the quaternion
		inline Quaternion& Inverse()
		{
			X = -X; Y = -Y; Z = -Z;
			return *this;
		}

		//! inverses the quaternion
		inline Quaternion Inverse() const
		{
			return Quaternion(-X, -Y, -Z, W);
		}

		//! returns the length
		inline float GetLength() const
		{
			return Math::SquareRoot((X*X) + (Y*Y) + (Z*Z) + (W*W));
		}

		//! returns the length squared
		inline float GetLengthSquared() const
		{
			return (X*X) + (Y*Y) + (Z*Z) + (W*W);
		}

		//! rotates a vector
		Vector3 Rotate(const Vector3& v) const;

		//! returns a matrix representation
		Matrix44 GetMatrix();

		// operators
		Quaternion operator * (const Quaternion& other) const;
		Vector3 operator * (const Vector3 &v) const;

		inline Quaternion& operator *= (const Quaternion& other)
		{
			return (*this = (*this) * other);
		}

		union
		{
			struct
			{
				float X, Y, Z;
			};

			Vector3 V;
		};

		float W;
	};
}



