/* 

Amine Rehioui
Created: May 8th 2010

*/

#pragma once

namespace shoot
{
	//! Axis-aligned 3D Bounding box class
	class AABBox3D
	{
	public:

		//! Constructor
		AABBox3D();

		//! Constructor
		AABBox3D(const Vector3& vMin, const Vector3& vMax);

		//! Set the boundaries of the box
		void Set(const Vector3& vMin, const Vector3& vMax);

		//! Returns the min corner
		inline const Vector3& Min() const { return m_vMin; }

		//! Returns the max corner
		inline const Vector3& Max() const { return m_vMax; }

		//! Returns the size of the box
		inline const Vector3& Size() const { return m_vSize; }

		//! Check if a point is in the box
		bool Contains(const Vector3& point) const;
		
		// operators
		AABBox3D operator + (const Vector3& vTranslation) const;
		AABBox3D operator - (const Vector3& vTranslation) const;		
		AABBox3D operator * (const Vector3& vScale) const;
		AABBox3D operator * (float scale) const;
		AABBox3D& operator = (const AABBox3D& other);
		AABBox3D operator + (const AABBox3D& other) const;

		inline bool operator == (const AABBox3D& other) const
		{
			return (m_vMin == other.m_vMin) && (m_vMax == other.m_vMax);
		}

		inline bool operator != (const AABBox3D& other) const
		{
			return !this->operator==(other);
		}

	private:

		Vector3 m_vMin, m_vMax;
		Vector3 m_vSize;
	};
};

