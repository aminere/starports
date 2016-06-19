/* 

Amine Rehioui
Created: April 1st 2013

*/

#pragma once


#include "Plane.h"

#include "AABBox3D.h"

namespace shoot
{
	//! Frustum class
	class Frustum
	{
	public:

		//! plane types
		enum E_Plane
		{
			P_Near,
			P_Far,
			P_Left,
			P_Right,
			P_Bottom,
			P_Top,
			P_Count
		};

		//! frustum test result
		enum E_TestResult
		{
			TR_In,
			TR_Out,
			TR_Intersect
		};

		//! constructor
		Frustum()
		{
		}

		//! constructor
		Frustum(float fFov, float fRatio, float fNear, float fFar, const Vector3& vPosition, const Vector3& vLookAt, const Vector3& vUp);

		//! constructor
		Frustum(const Vector2& orthoSize, float fNear, float fFar, const Vector3& vPosition, const Vector3& vLookAt, const Vector3& vUp);

		//! test a box against the frustum
		E_TestResult Test(const AABBox3D& bbox) const;

		//! test a sphere against the frustum
		E_TestResult Test(const Vector3& vCenter, float fRadius) const;

		//! checks if a point is inside the frustum
		bool Contains(const Vector3& vPoint) const;

		//! returns a plane
		inline const Plane& GetPlane(E_Plane ePlane) const { return m_Planes[ePlane]; }

#ifdef SHOOT_EDITOR
		Vector3 m_FTopLeft;
		Vector3 m_FTopRight;
		Vector3 m_FBottomLeft;
		Vector3 m_FBottomRight;
		Vector3 m_NTopLeft;
		Vector3 m_NTopRight;
		Vector3 m_NBottomLeft;
		Vector3 m_NBottomRight;
#endif // SHOOT_EDITOR

	private:

		//! Init
		void Init(const Vector2& nearSize, const Vector2& farSize, float fNear, float fFar, const Vector3& vPosition, const Vector3& vLookAt, const Vector3& vUp);

		Plane m_Planes[P_Count];
	};
}



