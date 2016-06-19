/* 

Amine Rehioui
Created: May 5th 2010

*/

#pragma once

namespace shoot
{
	//! 4x4 affine transformation matrix
	class Matrix44
	{
	public:

		//! constructor
		Matrix44()
		{	
		}

		//! constructor
		Matrix44(const float pFloats[16]);

		//! sets the translation
		void SetTranslation(const Vector3& vTranslation);

		//! sets the rotation
		void SetRotation(const Vector3& vRotation);

		//! sets the scale
		void SetScale(const Vector3& vScale);

		//! returns the translation set in this matrix
		Vector3 GetTranslation() const;

		//! returns the scaling
		Vector3 GetScale() const;

		//! translates the matrix
		Matrix44& Translate(const Vector3& vTranslation);

		//! rotates the matrix
		Matrix44& Rotate(const Vector3& vRotation);

		//! scales the matrix
		Matrix44& Scale(const Vector3& vScale);

		//! transforms a 3D vector by this matrix
		Vector3 TransformVect(const Vector3& vect) const;

		//! calculates the inverse matrix
		/** returns false if inverse could not be calculated */
		bool GetInverse(Matrix44& out) const;
		
		// calculates a transposed matrix
		Matrix44 GetTranspose() const;

		//! returns a pointer to the internal float data
		inline  const float* GetFloats() const { return m_f; }

		//! builds a perspective projection matrix
		static Matrix44 MakePerspectiveProjection(float fFOVDegrees, float fRatio, float fZNear, float fZFar);

		//! builds a 3D orthographic projection matrix
		static Matrix44 Make3DOrthographicProjection(float fWidth, float fHeight, float fZNear, float fZFar);

		//! builds a 2D orthographic projection matrix
		static Matrix44 Make2DOrthographicProjection(float fWidth, float fHeight);
		
		//! builds an LH look at matrix
		static Matrix44 MakeLookAtLH(const Vector3& vEye, const Vector3& vDir, const Vector3& vUpVector);

		// operators
		Matrix44 operator * (const Matrix44& other) const;
		Vector4 operator * (const Vector4& other) const;

		inline Matrix44& operator *= (const Matrix44& other)
		{			
			return (*this = (*this) * other);
		}

		inline float& operator()(const int row, const int column)
		{
			return m_f[row * 4 + column];
		}

		inline const float& operator()(const int row, const int column) const
		{
			return m_f[row * 4 + column];
		}

		inline float& operator[](int index)
		{
			return m_f[index];
		}

		inline const float& operator[](int index) const
		{					
			return m_f[index];
		}

		//! static variables
		static Matrix44 Identity;

	private:

		float m_f[16];
	};
}

