/* 

Amine Rehioui
Created: May 5th 2010

*/

#include "Shoot.h"

#ifdef DX11
#include <directxmath.h>
#endif

namespace shoot
{
	// static variables initialization
	const float g_Matrix44IdentityFloats[16] = { 1.0f, 0.0f, 0.0f, 0.0f,
											   0.0f, 1.0f, 0.0f, 0.0f,
											   0.0f, 0.0f, 1.0f, 0.0f,
											   0.0f, 0.0f, 0.0f, 1.0f };

	Matrix44 Matrix44::Identity = Matrix44(g_Matrix44IdentityFloats);

	//! constructor
	Matrix44::Matrix44(const float pFloats[16])
	{
		memcpy(m_f, pFloats, 16*sizeof(float));
	}

	//! sets the translation
	void Matrix44::SetTranslation(const Vector3& vTranslation)
	{
		m_f[12] = vTranslation.X;
		m_f[13] = vTranslation.Y;
		m_f[14] = vTranslation.Z;
	}

	//! sets the rotation
	void Matrix44::SetRotation(const Vector3& vRotation)
	{
		float sinX = Math::Sin(vRotation.X);
		float cosX = Math::Cos(vRotation.X);
		float sinY = Math::Sin(vRotation.Y);
		float cosY = Math::Cos(vRotation.Y);
		float sinZ = Math::Sin(vRotation.Z);
		float cosZ = Math::Cos(vRotation.Z);

		// Rotation around X axis - pitch
		Matrix44 RX = Identity;
		RX[5] = cosX;
		RX[6] = -sinX;
		RX[9] = sinX;
		RX[10] = cosX;

		// Rotation around Y axis - yaw
		Matrix44 RY = Identity;
		RY[0] = cosY;
		RY[2] = sinY;
		RY[8] = -sinY;
		RY[10] = cosY;

		// Rotation around Z axis - roll
		Matrix44 RZ = Identity;
		RZ[0] = cosZ;
		RZ[1] = -sinZ;
		RZ[4] = sinZ;
		RZ[5] = cosZ;

		// Apply all rotations in Rx Ry Rz order
		*this = RX * RY * RZ;
	}

	//! sets the scale
	void Matrix44::SetScale(const Vector3& vScale)
	{
		m_f[0] = vScale.X;
		m_f[5] = vScale.Y;
		m_f[10] = vScale.Z;
	}

	//! returns the translation set in this matrix
	Vector3 Matrix44::GetTranslation() const
	{
		return Vector3::Create(m_f[12], m_f[13], m_f[14]);
	}

	//! returns the scaling
	Vector3 Matrix44::GetScale() const
	{
		return Vector3::Create(m_f[0], m_f[5], m_f[10]);
	}

	//! translates the matrix
	Matrix44& Matrix44::Translate(const Vector3& vTranslation)
	{
		m_f[12] += vTranslation.X;
		m_f[13] += vTranslation.Y;
		m_f[14] += vTranslation.Z;
		return *this;
	}

	//! rotates the matrix
	Matrix44& Matrix44::Rotate(const Vector3& vRotation)
	{
		Matrix44 rotated;
		rotated.SetRotation(vRotation);
		(*this) *= rotated;
		return *this;
	}

	//! scales the matrix
	Matrix44& Matrix44::Scale(const Vector3& vScale)
	{
		m_f[0] *= vScale.X;
		m_f[5] *= vScale.Y;
		m_f[10] *= vScale.Z;

		// scale translation components too
		m_f[12] *= vScale.X;
		m_f[13] *= vScale.Y;
		m_f[14] *= vScale.Z;
		return *this;
	}

	//! transforms a 3D vector by this matrix
	Vector3 Matrix44::TransformVect(const Vector3& vect) const
	{
		Vector3 result;
		result.X = vect.X*m_f[0] + vect.Y*m_f[4] + vect.Z*m_f[8] + m_f[12];
		result.Y = vect.X*m_f[1] + vect.Y*m_f[5] + vect.Z*m_f[9] + m_f[13];
		result.Z = vect.X*m_f[2] + vect.Y*m_f[6] + vect.Z*m_f[10] + m_f[14];
		return result;
	}

	//! calculates the inverse matrix
	/** returns false if inverse could not be calculated */
	bool Matrix44::GetInverse(Matrix44& out) const
	{
		// Code taken from the Irrlicht engine:
		// Calculates the inverse of this Matrix
		// The inverse is calculated using Cramers rule.
		// If no inverse exists then 'false' is returned.
		const Matrix44 &m = *this;

#ifdef DX11
		DirectX::XMFLOAT4X4 dx;
		DirectX::XMVECTOR determinant;
		DirectX::XMMATRIX mx(m(0, 0), m(0, 1), m(0, 2), m(0, 3),
							 m(1, 0), m(1, 1), m(1, 2), m(1, 3),
							 m(2, 0), m(2, 1), m(2, 2), m(2, 3),
							 m(3, 0), m(3, 1), m(3, 2), m(3, 3));
		XMStoreFloat4x4(&dx, DirectX::XMMatrixInverse(&determinant, mx));
		out[0] = dx(0, 0);
		out[1] = dx(0, 1);
		out[2] = dx(0, 2);
		out[3] = dx(0, 3);
		out[4] = dx(1, 0);
		out[5] = dx(1, 1);
		out[6] = dx(1, 2);
		out[7] = dx(1, 3);
		out[8] =  dx(2, 0);
		out[9] =  dx(2, 1);
		out[10] = dx(2, 2);
		out[11] = dx(2, 3);
		out[12] = dx(3, 0);
		out[13] = dx(3, 1);
		out[14] = dx(3, 2);
		out[15] = dx(3, 3);
		return true;
#else
		float d = (m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)) * (m(2, 2) * m(3, 3) - m(2, 3) * m(3, 2)) -
				(m(0, 0) * m(1, 2) - m(0, 2) * m(1, 0)) * (m(2, 1) * m(3, 3) - m(2, 3) * m(3, 1)) +
				(m(0, 0) * m(1, 3) - m(0, 3) * m(1, 0)) * (m(2, 1) * m(3, 2) - m(2, 2) * m(3, 1)) +
				(m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)) * (m(2, 0) * m(3, 3) - m(2, 3) * m(3, 0)) -
				(m(0, 1) * m(1, 3) - m(0, 3) * m(1, 1)) * (m(2, 0) * m(3, 2) - m(2, 2) * m(3, 0)) +
				(m(0, 2) * m(1, 3) - m(0, 3) * m(1, 2)) * (m(2, 0) * m(3, 1) - m(2, 1) * m(3, 0));

		if(Math::FAbs(d) < Math::Epsilon)
		{
			return false;
		}

		d = 1.0f / d;

		out(0, 0) = d * (m(1, 1) * (m(2, 2) * m(3, 3) - m(2, 3) * m(3, 2)) +
						 m(1, 2) * (m(2, 3) * m(3, 1) - m(2, 1) * m(3, 3)) +
						 m(1, 3) * (m(2, 1) * m(3, 2) - m(2, 2) * m(3, 1)));
		out(0, 1) = d * (m(2, 1) * (m(0, 2) * m(3, 3) - m(0, 3) * m(3, 2)) +
						 m(2, 2) * (m(0, 3) * m(3, 1) - m(0, 1) * m(3, 3)) +
						 m(2, 3) * (m(0, 1) * m(3, 2) - m(0, 2) * m(3, 1)));
		out(0, 2) = d * (m(3, 1) * (m(0, 2) * m(1, 3) - m(0, 3) * m(1, 2)) +
						 m(3, 2) * (m(0, 3) * m(1, 1) - m(0, 1) * m(1, 3)) +
						 m(3, 3) * (m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)));
		out(0, 3) = d * (m(0, 1) * (m(1, 3) * m(2, 2) - m(1, 2) * m(2, 3)) +
						 m(0, 2) * (m(1, 1) * m(2, 3) - m(1, 3) * m(2, 1)) +
						 m(0, 3) * (m(1, 2) * m(2, 1) - m(1, 1) * m(2, 2)));
		out(1, 0) = d * (m(1, 2) * (m(2, 0) * m(3, 3) - m(2, 3) * m(3, 0)) +
						 m(1, 3) * (m(2, 2) * m(3, 0) - m(2, 0) * m(3, 2)) +
						 m(1, 0) * (m(2, 3) * m(3, 2) - m(2, 2) * m(3, 3)));
		out(1, 1) = d * (m(2, 2) * (m(0, 0) * m(3, 3) - m(0, 3) * m(3, 0)) +
						 m(2, 3) * (m(0, 2) * m(3, 0) - m(0, 0) * m(3, 2)) +
						 m(2, 0) * (m(0, 3) * m(3, 2) - m(0, 2) * m(3, 3)));
		out(1, 2) = d * (m(3, 2) * (m(0, 0) * m(1, 3) - m(0, 3) * m(1, 0)) +
						 m(3, 3) * (m(0, 2) * m(1, 0) - m(0, 0) * m(1, 2)) +
						 m(3, 0) * (m(0, 3) * m(1, 2) - m(0, 2) * m(1, 3)));
		out(1, 3) = d * (m(0, 2) * (m(1, 3) * m(2, 0) - m(1, 0) * m(2, 3)) +
						 m(0, 3) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0)) +
						 m(0, 0) * (m(1, 2) * m(2, 3) - m(1, 3) * m(2, 2)));
		out(2, 0) = d * (m(1, 3) * (m(2, 0) * m(3, 1) - m(2, 1) * m(3, 0)) +
						 m(1, 0) * (m(2, 1) * m(3, 3) - m(2, 3) * m(3, 1)) +
						 m(1, 1) * (m(2, 3) * m(3, 0) - m(2, 0) * m(3, 3)));
		out(2, 1) = d * (m(2, 3) * (m(0, 0) * m(3, 1) - m(0, 1) * m(3, 0)) +
						 m(2, 0) * (m(0, 1) * m(3, 3) - m(0, 3) * m(3, 1)) +
						 m(2, 1) * (m(0, 3) * m(3, 0) - m(0, 0) * m(3, 3)));
		out(2, 2) = d * (m(3, 3) * (m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)) +
						 m(3, 0) * (m(0, 1) * m(1, 3) - m(0, 3) * m(1, 1)) +
						 m(3, 1) * (m(0, 3) * m(1, 0) - m(0, 0) * m(1, 3)));
		out(2, 3) = d * (m(0, 3) * (m(1, 1) * m(2, 0) - m(1, 0) * m(2, 1)) +
						 m(0, 0) * (m(1, 3) * m(2, 1) - m(1, 1) * m(2, 3)) +
						 m(0, 1) * (m(1, 0) * m(2, 3) - m(1, 3) * m(2, 0)));
		out(3, 0) = d * (m(1, 0) * (m(2, 2) * m(3, 1) - m(2, 1) * m(3, 2)) +
						 m(1, 1) * (m(2, 0) * m(3, 2) - m(2, 2) * m(3, 0)) +
						 m(1, 2) * (m(2, 1) * m(3, 0) - m(2, 0) * m(3, 1)));
		out(3, 1) = d * (m(2, 0) * (m(0, 2) * m(3, 1) - m(0, 1) * m(3, 2)) +
						 m(2, 1) * (m(0, 0) * m(3, 2) - m(0, 2) * m(3, 0)) +
						 m(2, 2) * (m(0, 1) * m(3, 0) - m(0, 0) * m(3, 1)));
		out(3, 2) = d * (m(3, 0) * (m(0, 2) * m(1, 1) - m(0, 1) * m(1, 2)) +
						 m(3, 1) * (m(0, 0) * m(1, 2) - m(0, 2) * m(1, 0)) +
						 m(3, 2) * (m(0, 1) * m(1, 0) - m(0, 0) * m(1, 1)));
		out(3, 3) = d * (m(0, 0) * (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1)) +
						 m(0, 1) * (m(1, 2) * m(2, 0) - m(1, 0) * m(2, 2)) +
						 m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0)));
		return true;
#endif
	}

	// calculates a transposed matrix
	Matrix44 Matrix44::GetTranspose() const
	{
		Matrix44 out;
		out[0] = m_f[0];
		out[ 1] = m_f[ 4];
		out[ 2] = m_f[ 8];
		out[ 3] = m_f[12];

		out[ 4] = m_f[ 1];
		out[ 5] = m_f[ 5];
		out[ 6] = m_f[ 9];
		out[ 7] = m_f[13];

		out[ 8] = m_f[ 2];
		out[ 9] = m_f[ 6];
		out[10] = m_f[10];
		out[11] = m_f[14];

		out[12] = m_f[ 3];
		out[13] = m_f[ 7];
		out[14] = m_f[11];
		out[15] = m_f[15];
		return out;
	}

	//! builds a perspective projection matrix
	shoot::Matrix44 Matrix44::MakePerspectiveProjection(float fFOVDegrees, float fRatio, float fZNear, float fZFar)
	{
		Matrix44 m;

#ifdef DX11
		DirectX::XMFLOAT4X4 dx;
		XMStoreFloat4x4(&dx, DirectX::XMMatrixPerspectiveFovRH(fFOVDegrees * Math::DegToRadFactor, fRatio, fZNear, fZFar));
		m[0] = dx(0, 0);
		m[1] = dx(0, 1);
		m[2] = dx(0, 2);
		m[3] = dx(0, 3);
		m[4] = dx(1, 0);
		m[5] = dx(1, 1);
		m[6] = dx(1, 2);
		m[7] = dx(1, 3);
		m[8] =  dx(2, 0);
		m[9] =  dx(2, 1);
		m[10] = dx(2, 2);
		m[11] = dx(2, 3);
		m[12] = dx(3, 0);
		m[13] = dx(3, 1);
		m[14] = dx(3, 2);
		m[15] = dx(3, 3);
#else
		float f = 1.0f / Math::Tan(fFOVDegrees * Math::DegToRadFactor / 2);		
		m[0] = f/fRatio;
		m[1] = 0.0f;
		m[2] = 0.0f;
		m[3] = 0.0f;

		m[4] = 0.0f;
		m[5] = f;
		m[6] = 0.0f;
		m[7] = 0.0f;

		m[8] = 0.0f;
		m[9] = 0.0f;
		m[10] = (fZFar+fZNear)/(fZNear-fZFar);
		m[11] = -1.0f;

		m[12] = 0.0f;
		m[13] = 0.0f;
		m[14] = (fZNear*fZFar*2)/(fZNear-fZFar);
		m[15] = 0.0f;
#endif

		return m;
	}

	//! builds a 3D orthographic projection matrix
	shoot::Matrix44 Matrix44::Make3DOrthographicProjection(float fWidth, float fHeight, float fZNear, float fZFar)
	{
		Matrix44 m;

#ifdef DX11
		SHOOT_ASSERT(false, "Build3DOrthographicProjection TODO");
#else
		m[0] = (2.0f/fWidth);
		m[1] = 0;
		m[2] = 0;
		m[3] = 0;

		m[4] = 0;
		m[5] = (2.0f/fHeight);
		m[6] = 0;
		m[7] = 0;

		m[8] = 0;
		m[9] = 0;
		m[10] = (1/(fZFar-fZNear));
		m[11] = 0;

		m[12] = 0;
		m[13] = 0;
		m[14] = (fZNear/(fZNear-fZFar));
		m[15] = 1.0;
#endif

		return m;
	}

	//! builds a 2D orthographic projection matrix
	shoot::Matrix44 Matrix44::Make2DOrthographicProjection(float fWidth, float fHeight)
	{
		Matrix44 m;

#ifdef DX11
		DirectX::XMFLOAT4X4 dx;
		XMStoreFloat4x4(&dx, DirectX::XMMatrixOrthographicOffCenterRH(0.0f, fWidth, fHeight, 0.0f, -1.0f, 1.0f));
		m[0] = dx(0, 0);
		m[1] = dx(0, 1);
		m[2] = dx(0, 2);
		m[3] = dx(0, 3);
		m[4] = dx(1, 0);
		m[5] = dx(1, 1);
		m[6] = dx(1, 2);
		m[7] = dx(1, 3);
		m[8] =  dx(2, 0);
		m[9] =  dx(2, 1);
		m[10] = dx(2, 2);
		m[11] = dx(2, 3);
		m[12] = dx(3, 0);
		m[13] = dx(3, 1);
		m[14] = dx(3, 2);
		m[15] = dx(3, 3);
#else
		m[0] = (2.0f/fWidth);
		m[1] = 0.0f;
		m[2] = 0.0f;
		m[3] = 0.0f;

		m[4] = 0.0f;
		m[5] = (-2.0f/fHeight);
		m[6] = 0.0f;
		m[7] = 0.0f;

		m[8] = 0.0f;
		m[9] = 0.0f;
		m[10] = -1.0f;
		m[11] = 0.0f;

		m[12] = -1.0f;
		m[13] = 1.0f;
		m[14] = 0.0f;
		m[15] = 1.0f;
#endif

		return m;
	}

	//! builds an LH look at matrix
	shoot::Matrix44 Matrix44::MakeLookAtLH(const Vector3& vEye, const Vector3& vDir, const Vector3& vUpVector)
	{
		Matrix44 m;

#ifdef DX11
		auto vTarget = vEye + vDir;
		DirectX::XMFLOAT4X4 dx;
		DirectX::XMVECTOR eye = DirectX::XMVectorSet(vEye.X, vEye.Y, vEye.Z, 0.0f);
		DirectX::XMVECTOR at = DirectX::XMVectorSet(vTarget.X, vTarget.Y, vTarget.Z, 0.0f);
		DirectX::XMVECTOR up = DirectX::XMVectorSet(vUpVector.X, vUpVector.Y, vUpVector.Z, 0.0f);
		XMStoreFloat4x4(&dx, DirectX::XMMatrixLookAtRH(eye, at, up));
		m[0] = dx(0, 0);
		m[1] = dx(0, 1);
		m[2] = dx(0, 2);
		m[3] = dx(0, 3);
		m[4] = dx(1, 0);
		m[5] = dx(1, 1);
		m[6] = dx(1, 2);
		m[7] = dx(1, 3);
		m[8] =  dx(2, 0);
		m[9] =  dx(2, 1);
		m[10] = dx(2, 2);
		m[11] = dx(2, 3);
		m[12] = dx(3, 0);
		m[13] = dx(3, 1);
		m[14] = dx(3, 2);
		m[15] = dx(3, 3);
#else 
		Vector3 dir = -vDir;
		Vector3 lateralAxe = vUpVector.CrossProduct(dir);
		lateralAxe.Normalize();
		Vector3 verticalAxe = dir.CrossProduct(lateralAxe);

		m[0] = lateralAxe.X; 
		m[1] = verticalAxe.X; 
		m[2] = dir.X;
		m[3] = 0.0f;

		m[4] = lateralAxe.Y;
		m[5] = verticalAxe.Y; 
		m[6] = dir.Y;
		m[7] = 0.0f;

		m[8] = lateralAxe.Z;
		m[9] = verticalAxe.Z;
		m[10] = dir.Z;
		m[11] = 0.0f;

		m[12] = -lateralAxe.DotProduct(vEye);
		m[13] = -verticalAxe.DotProduct(vEye);
		m[14] = -dir.DotProduct(vEye);
		m[15] = 1.0f;
#endif

		return m;
	}

	// operators
	Matrix44 Matrix44::operator * (const Matrix44& other) const
	{
		Matrix44 result;
		for(int i=0; i<4; ++i)
		{
			for(int j=0; j<4; ++j)
			{
				result(i, j) = (*this)(i, 0) * other(0, j) 
							 + (*this)(i, 1) * other(1, j) 
							 + (*this)(i, 2) * other(2, j)
							 + (*this)(i, 3) * other(3, j);
			}
		}
		return result;
	}

	Vector4 Matrix44::operator * (const Vector4& vect) const
	{
		Vector4 result;
		result.X = vect.X*m_f[0] + vect.Y*m_f[4] + vect.Z*m_f[8]  + vect.W*m_f[12];
		result.Y = vect.X*m_f[1] + vect.Y*m_f[5] + vect.Z*m_f[9]  + vect.W*m_f[13];
		result.Z = vect.X*m_f[2] + vect.Y*m_f[6] + vect.Z*m_f[10] + vect.W*m_f[14];
		result.W = vect.X*m_f[3] + vect.Y*m_f[7] + vect.Z*m_f[11] + vect.W*m_f[15];
		return result;
	}
}