/* 

Amine Rehioui
Created: April 1st 2013

*/

#include "Shoot.h"

#include "Frustum.h"

namespace shoot
{
	//! constructor
	Frustum::Frustum(float fFov, float fRatio, float fNear, float fFar, const Vector3& vPosition, const Vector3& vLookAt, const Vector3& vUp)
	{
		float Hnear = Math::Tan(fFov * Math::DegToRadFactor / 2) * fNear;
		float Wnear = Hnear * fRatio;
		float Hfar = Math::Tan(fFov * Math::DegToRadFactor / 2) * fFar;
		float Wfar = Hfar * fRatio;
		
		Init(Vector2::Create(Wnear, Hnear), Vector2::Create(Wfar, Hfar), fNear, fFar, vPosition, vLookAt, vUp);
	}

	//! constructor
	Frustum::Frustum(const Vector2& orthoSize, float fNear, float fFar, const Vector3& vPosition, const Vector3& vLookAt, const Vector3& vUp)
	{
		Init(orthoSize, orthoSize, fNear, fFar, vPosition, vLookAt, vUp);
	}

	//! Init
	void Frustum::Init(const Vector2& nearSize, const Vector2& farSize, float fNear, float fFar, const Vector3& vPosition, const Vector3& vLookAt, const Vector3& vUp)
	{
		Vector3 vDirection = (vLookAt-vPosition).Normalize();
		Vector3 vRight = vDirection.CrossProduct(vUp).Normalize();
		Vector3 vLocalUp = vRight.CrossProduct(vDirection).Normalize();

		Vector3 FCenter = vPosition + vDirection * fFar;
		Vector3 FTopLeft = FCenter + (vLocalUp * farSize.Y) - (vRight * farSize.X);
		Vector3 FTopRight = FCenter + (vLocalUp * farSize.Y) + (vRight * farSize.X);
		Vector3 FBottomLeft = FCenter - (vLocalUp * farSize.Y) - (vRight * farSize.X);
		Vector3 FBottomRight = FCenter - (vLocalUp * farSize.Y) + (vRight * farSize.X);
		Vector3 NCenter = vPosition + vDirection * fNear;
		Vector3 NTopLeft = NCenter + (vLocalUp * nearSize.Y) - (vRight * nearSize.X);
		Vector3 NTopRight = NCenter + (vLocalUp * nearSize.Y) + (vRight * nearSize.X);
		Vector3 NBottomLeft = NCenter- (vLocalUp * nearSize.Y) - (vRight * nearSize.X);
		Vector3 NBottomRight = NCenter - (vLocalUp * nearSize.Y) + (vRight * nearSize.X);

		m_Planes[P_Top] = Plane(NTopLeft, FTopLeft, FTopRight);
		m_Planes[P_Bottom] = Plane(NBottomRight, FBottomRight, FBottomLeft);
		m_Planes[P_Left] = Plane(FBottomLeft, FTopLeft, NTopLeft);
		m_Planes[P_Right] = Plane(NBottomRight, NTopRight, FTopRight);
		m_Planes[P_Near] = Plane(NBottomLeft, NTopLeft, NTopRight);
		m_Planes[P_Far] = Plane(FBottomRight, FTopRight, FTopLeft);

#ifdef SHOOT_EDITOR
		m_FTopLeft = FTopLeft;
		m_FTopRight = FTopRight;
		m_FBottomLeft = FBottomLeft;
		m_FBottomRight = FBottomRight;
		m_NTopLeft = NTopLeft;
		m_NTopRight = NTopRight;
		m_NBottomLeft = NBottomLeft;
		m_NBottomRight = NBottomRight;
#endif // SHOOT_EDITOR
	}

	//! test a box against the frustum
	Frustum::E_TestResult Frustum::Test(const AABBox3D& bbox) const
	{
		Vector3 corners[8] =
		{
			Vector3::Create(bbox.Min().X, bbox.Min().Y, bbox.Min().Z),
			Vector3::Create(bbox.Min().X, bbox.Min().Y, bbox.Max().Z),
			Vector3::Create(bbox.Min().X, bbox.Max().Y, bbox.Min().Z),
			Vector3::Create(bbox.Min().X, bbox.Max().Y, bbox.Max().Z),
			Vector3::Create(bbox.Max().X, bbox.Min().Y, bbox.Min().Z),
			Vector3::Create(bbox.Max().X, bbox.Min().Y, bbox.Max().Z),
			Vector3::Create(bbox.Max().X, bbox.Max().Y, bbox.Min().Z),
			Vector3::Create(bbox.Max().X, bbox.Max().Y, bbox.Max().Z)
		};

		int planesIn = 0;
		for(int i=0; i<P_Count; ++i)
		{
			int pointsIn = 8;
			int planeIn = 1;

			for(int j=0; j<8; ++j)
			{
				if(m_Planes[i].ClassifyPoint(corners[j]) == Plane::C_Front)
				{
					planeIn = 0;
					--pointsIn;
				}
			}
			
			if(pointsIn == 0)
				return TR_Out;

			planesIn += planeIn;
		}
		
		if(planesIn == P_Count)
			return TR_In;
		
		return TR_Intersect;
	}

	//! test a sphere against the frustum
	Frustum::E_TestResult Frustum::Test(const Vector3& vCenter, float fRadius) const
	{
		for(int i=0; i<P_Count; ++i)
		{
			float fDist = vCenter.DotProduct(m_Planes[i].Normal) - m_Planes[i].D;
			if(fDist > fRadius)
				return TR_Out;

			if(Math::FAbs(fDist) < fRadius)
				return TR_Intersect;
		}

		return TR_In;
	}

	//! checks if a point is inside the frustum
	bool Frustum::Contains(const Vector3& vPoint) const
	{
		for(int i=0; i<P_Count; ++i)
		{
			if(m_Planes[i].ClassifyPoint(vPoint) == Plane::C_Front)
				return false;
		}
		return true;
	}
}

