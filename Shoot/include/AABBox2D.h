/* 

Amine Rehioui
Created: March 20th 2010

*/

#pragma once

#include "Vector2.h"

namespace shoot
{
	//! Axis-aligned 2D Bounding box class
	class AABBox2D
	{
	public:

		//! Constructor
		AABBox2D()
			: m_vMin(Vector2::Zero)
			, m_vMax(Vector2::Zero)
			, m_vSize(Vector2::Zero)
		{
		}

		//! Constructor
		AABBox2D(const Vector2& vMin, const Vector2& vMax);

		//! Set the boundaries of the box
		void Set(const Vector2& vMin, const Vector2& vMax);

		//! Returns the min corner
		inline const Vector2& Min() const { return m_vMin; }

		//! Returns the max corner
		inline const Vector2& Max() const { return m_vMax; }

		//! Returns the size of the box
		inline const Vector2& Size() const { return m_vSize; }

		//! Check if a point is in the box
		bool Contains(const Vector2& point) const;
		
		// operators
		AABBox2D operator + (const Vector2& point);
		AABBox2D operator - (const Vector2& point);
		AABBox2D operator * (const Vector2& vScale) const;
		AABBox2D operator + (const AABBox2D& other) const;

		inline bool operator == (const AABBox2D& other) const
		{
			return (m_vMin == other.m_vMin) && (m_vMax == other.m_vMax);
		}

		inline bool operator != (const AABBox2D& other) const
		{
			return !this->operator==(other);
		}

	private:

		Vector2 m_vMin, m_vMax;
		Vector2 m_vSize;
	};
};

