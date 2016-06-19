/* 

Amine Rehioui
Created: May 8th 2010

*/

#include "Shoot.h"

#include "AABBox3D.h"

namespace shoot
{
	//! Constructor
	AABBox3D::AABBox3D()
	{
		Set(Vector3::Zero, Vector3::Zero);
	}

	//! Constructor
	AABBox3D::AABBox3D(const Vector3& min, const Vector3& max)
	{
		Set(min, max);
	}

	//! Set the boundaries of the box
	void AABBox3D::Set(const Vector3& min, const Vector3& max)
	{		
		m_vMin = Vector3::Create(Math::Min(min.X, max.X), Math::Min(min.Y, max.Y), Math::Min(min.Z, max.Z));
		m_vMax = Vector3::Create(Math::Max(min.X, max.X), Math::Max(min.Y, max.Y), Math::Max(min.Z, max.Z));
		m_vSize = m_vMax - m_vMin;
	}

	//! check if a point is in the box
	bool AABBox3D::Contains(const Vector3& point) const
	{
		return (point.X >= m_vMin.X) 
			&& (point.X < m_vMax.X)
			&& (point.Y >= m_vMin.Y)
			&& (point.Y < m_vMax.Y)
			&& (point.Z >= m_vMin.Z)
			&& (point.Z < m_vMax.Z);
	}

	// operators
	AABBox3D AABBox3D::operator + (const Vector3& vTranslation) const
	{
		AABBox3D result(m_vMin+vTranslation, m_vMax+vTranslation);
		return result;
	}

	AABBox3D AABBox3D::operator - (const Vector3& vTranslation) const
	{
		AABBox3D result(m_vMin-vTranslation, m_vMax-vTranslation);
		return result;
	}

	AABBox3D AABBox3D::operator * (const Vector3& vScale) const
	{
		AABBox3D result(m_vMin*vScale, m_vMax*vScale);	
		return result;
	}

	AABBox3D AABBox3D::operator * (float scale) const
	{
		AABBox3D result(m_vMin*scale, m_vMax*scale);
		return result;
	}

	AABBox3D& AABBox3D::operator = (const AABBox3D& other)
	{
		Set(other.Min(), other.Max());
		return *this;
	}

	AABBox3D AABBox3D::operator + (const AABBox3D& other) const
	{
		Vector3 vMin = Vector3::Create(Math::Min(m_vMin.X, other.m_vMin.X), 
									   Math::Min(m_vMin.Y, other.m_vMin.Y), 
									   Math::Min(m_vMin.Z, other.m_vMin.Z));

		Vector3 vMax = Vector3::Create(Math::Max(m_vMax.X, other.m_vMax.X), 
									   Math::Max(m_vMax.Y, other.m_vMax.Y), 
									   Math::Max(m_vMax.Z, other.m_vMax.Z));

		return AABBox3D(vMin, vMax);
	}
}


