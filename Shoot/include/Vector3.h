/* 

Amine Rehioui
Created: February 28th 2010

*/

#pragma once

namespace shoot
{
	//! 3D vector class
	class Vector3
	{
	public:

		inline static Vector3 Create(float x, float y, float z)
		{
			Vector3 vec3 = { x, y, z };
			return vec3;
		}

		inline static Vector3 Create(const Vector2& vec2, float z)
		{
			Vector3 vec3 = { vec2.X, vec2.Y, z };
			return vec3;
		}

		//! Sets the coordinates
		inline void Set(float x, float y, float z) 
		{
			X = x;
			Y = y;
			Z = z;
		}

		//! returns the length
		inline float GetLength() const
		{
			return Math::SquareRoot((X*X) + (Y*Y) + (Z*Z));
		}

		//! returns the length squared
		inline float GetLengthSquared() const
		{
			return (X*X) + (Y*Y) + (Z*Z);
		}

		//! returns the dot product
		inline float DotProduct(const Vector3& other) const
		{
			return (X*other.X) + (Y*other.Y) + (Z*other.Z);
		}

		//! returns the cross product
		inline Vector3 CrossProduct(const Vector3& other) const
		{
			return Vector3::Create(Y*other.Z - Z*other.Y, Z*other.X - X*other.Z, X*other.Y - Y*other.X);
		}

		//! normalizes the vector
		inline const Vector3& Normalize() { return (*this = Normalized()); }

		//! returns a normalized vector
		Vector3 Normalized() const;

		//! Limits the components to maximum and resizes proportionally if needed
		const Vector3& Limit(float fMax);

		//! clamps the vector
		inline const Vector3& Clamp(const Vector3& vMin, const Vector3& vMax)
		{
			return (*this = static_cast<const Vector3*>(this)->Clamp(vMin, vMax));
		}

		//! clamps the vector
		Vector3 Clamp(const Vector3& vMin, const Vector3& vMax) const;

		//! rotates the vector
		inline const Vector3& Rotate(const Vector3& vRotationRadians)
		{
			return (*this = static_cast<const Vector3*>(this)->Rotate(vRotationRadians));
		}

		//! rotates the vector
		Vector3 Rotate(const Vector3& vRotationRadians) const;

		//! rotates the vector around an axis
		inline const Vector3& Rotate(const Vector3& vAxis, float fAngleRadians)
		{
			return (*this = static_cast<const Vector3*>(this)->Rotate(vAxis, fAngleRadians));
		}

		//! rotates the vector around an axis
		Vector3 Rotate(const Vector3& vAxis, float fAngleRadians) const;

		//! differentiates this vector from the Up vector
		Vector3 DifferentiateFromUp() const;

		// operators
		inline Vector3 operator + (const Vector3& other) const
		{			
			return Vector3::Create(X+other.X, Y+other.Y, Z+other.Z);
		}

		inline Vector3 operator - (const Vector3& other) const
		{			
			return Vector3::Create(X-other.X, Y-other.Y, Z-other.Z);
		}

		inline Vector3 operator * (const Vector3& other) const
		{
			return Vector3::Create(X*other.X, Y*other.Y, Z*other.Z);
		}

		inline Vector3 operator / (const Vector3& other) const
		{
			return Vector3::Create(X/other.X, Y/other.Y, Z/other.Z);
		}

		inline Vector3 operator * (const float fValue) const
		{			
			return Vector3::Create(X*fValue, Y*fValue, Z*fValue);
		}

		inline Vector3 operator / (const float fValue) const
		{			
			return Vector3::Create(X/fValue, Y/fValue, Z/fValue);
		}

		inline Vector3& operator += (const Vector3& other)
		{
			X += other.X; Y += other.Y;	Z += other.Z;
			return *this;
		}

		inline Vector3& operator -= (const Vector3& other)
		{
			X -= other.X; Y -= other.Y; Z -= other.Z;
			return *this;
		}

		inline Vector3& operator *= (const float fValue)
		{
			X *= fValue; Y *= fValue; Z *= fValue; return *this;
		}

		inline Vector3& operator /= (const float fValue)
		{
			X /= fValue; Y /= fValue; Z /= fValue; return *this;
		}

		inline Vector3 operator -() const
		{
			return Vector3::Create(-X, -Y, -Z);
		}

		inline float& operator[] (int index)
		{
			SHOOT_ASSERT(index < 3, "Invalid index");
			return *(&v[index]);
		}

		inline const float& operator[] (int index) const
		{
			SHOOT_ASSERT(index < 3, "Invalid index");
			return *(&v[index]);
		}

		bool operator == (const Vector3& other) const;

		inline bool operator != (const Vector3& other) const { return !this->operator==(other); }
		inline bool IsZero() const { return Math::FIsZero(X+Y+Z); }
		inline Vector2 XZ() const { return Vector2::Create(X, Z); }

		union
		{
			struct
			{
				float X, Y, Z;
			};

			struct
			{
				Vector2 XY;
				float Z2;
			};

			float v[3];
		};

		static Vector3 Up;
		static Vector3 Zero;
		static Vector3 One;
	};
}

