/* 

Amine Rehioui
Created: September 4th 2012

*/

#pragma once

namespace shoot
{
	//! 4D vector class
	class Vector4
	{
	public:

		inline static Vector4 Create(float x, float y, float z, float w)
		{
			Vector4 vec4 = { x, y, z, w };
			return vec4;
		}

		inline static Vector4 Create(const Vector3& vec3, float w)
		{
			Vector4 vec4 = { vec3.X, vec3.Y, vec3.Z, w };
			return vec4;
		}

		//! Sets the coordinates
		inline void Set(float x, float y, float z, float w) 
		{
			X = x;
			Y = y;
			Z = z;
			W = w;
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

		//! returns the dot product
		inline float DotProduct(const Vector4& other) const
		{
			return (X*other.X) + (Y*other.Y) + (Z*other.Z) + (W*other.W);
		}

		//! returns the cross product
		inline Vector4 CrossProduct(const Vector4& other) const
		{
			return Vector4::Create(Y*other.Z - Z*other.Y, Z*other.X - X*other.Z, X*other.Y - Y*other.X, 0.0f);
		}

		//! normalizes the vector
		inline Vector4& Normalize()
		{
			return (*this = static_cast<const Vector4*>(this)->Normalize());
		}

		//! normalizes the vector
		Vector4 Normalize() const;

		// operators
		inline Vector4 operator + (const Vector4& other) const
		{			
			return Vector4::Create(X+other.X, Y+other.Y, Z+other.Z, W+other.W);
		}

		inline Vector4 operator - (const Vector4& other) const
		{			
			return Vector4::Create(X-other.X, Y-other.Y, Z-other.Z, W-other.W);
		}

		inline Vector4 operator * (const float fValue) const
		{			
			return Vector4::Create(X*fValue, Y*fValue, Z*fValue, W*fValue);
		}

		inline Vector4 operator / (const float fValue) const
		{			
			return Vector4::Create(X/fValue, Y/fValue, Z/fValue, W/fValue);
		}

		inline Vector4& operator += (const Vector4& other)
		{
			X += other.X; Y += other.Y;	Z += other.Z; W += other.W;
			return *this;
		}

		inline Vector4& operator -= (const Vector4& other)
		{
			X -= other.X; Y -= other.Y; Z -= other.Z; W -= other.W;
			return *this;
		}

		inline Vector4& operator *= (const float fValue)
		{
			X *= fValue; Y *= fValue; Z *= fValue; W *= fValue; return *this;
		}

		inline Vector4 operator -() const
		{			
			return Vector4::Create(-X, -Y, -Z, -W);
		}

		inline float& operator[] (int index)
		{
			SHOOT_ASSERT(index < 4, "Invalid index");
			return *(&v[index]);
		}

		inline const float& operator[] (int index) const
		{
			SHOOT_ASSERT(index < 4, "Invalid index");
			return *(&v[index]);
		}

		bool operator == (const Vector4& other) const;

		inline bool operator != (const Vector4& other) const
		{
			return !this->operator==(other);
		}

		union
		{
			struct
			{
				float X, Y, Z, W;
			};

			struct
			{
				Vector3 XYZ;
				float W2;
			};

			float v[4];
		};

		static Vector4 Zero;
		static Vector4 One;
	};
}

