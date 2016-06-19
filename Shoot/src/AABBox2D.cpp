/* 

Amine Rehioui
Created: March 20th 2010

*/

#include "Shoot.h"

#include "AABBox2D.h"

namespace shoot
{
	//! Constructor
	AABBox2D::AABBox2D(const Vector2& min, const Vector2& max)
	{
		Set(min, max);
	}

	//! Set the boundaries of the box
	void AABBox2D::Set(const Vector2& min, const Vector2& max)
	{
		SHOOT_WARNING((max.X >= min.X) && (max.Y >= min.Y), "2D bounding box with negative size detected");
		m_vMin = min;
		m_vMax = max;
		m_vSize = max - min;
	}

	//! check if a point is in the box
	bool AABBox2D::Contains(const Vector2& point) const
	{
		return (point.X >= m_vMin.X) 
			&& (point.X < m_vMax.X)
			&& (point.Y >= m_vMin.Y)
			&& (point.Y < m_vMax.Y);
	}

	// operators
	AABBox2D AABBox2D::operator + (const Vector2& point)
	{
		AABBox2D result(m_vMin+point, m_vMax+point);
		return result;
	}

	AABBox2D AABBox2D::operator - (const Vector2& point)
	{
		AABBox2D result(m_vMin-point, m_vMax-point);
		return result;
	}

	AABBox2D AABBox2D::operator + (const AABBox2D& other) const
	{
		Vector2 vMin = Vector2::Create(Math::Min(m_vMin.X, other.m_vMin.X), 
									   Math::Min(m_vMin.Y, other.m_vMin.Y));

		Vector2 vMax = Vector2::Create(Math::Max(m_vMax.X, other.m_vMax.X), 
									   Math::Max(m_vMax.Y, other.m_vMax.Y));

		return AABBox2D(vMin, vMax);
	}

	AABBox2D AABBox2D::operator * (const Vector2& vScale) const
	{
		AABBox2D result(m_vMin*vScale, m_vMax*vScale);
		return result;
	}
}

