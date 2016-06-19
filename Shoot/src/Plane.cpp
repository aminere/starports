/* 

Amine Rehioui
Created: December 18th 2010

*/

#include "Shoot.h"

#include "Plane.h"

namespace shoot
{
	//! constructor
	Plane::Plane(const Vector3& v1, const Vector3& v2, const Vector3& v3)
	{
		Normal = (v3 - v1).CrossProduct(v2 - v1);
		Normal.Normalize();
		D = v1.DotProduct(Normal);
	}

	//! returns the intersection of ray with the plane		
	bool Plane::IntersectWithRay(const Vector3& vRayStart, const Vector3& vRayDir, Vector3* pIntersection /*= 0*/, E_Classification* pClass /*= 0*/) const
	{
		float nominator;
		E_Classification eClass = ClassifyPoint(vRayStart, &nominator);		

		if(eClass == C_Planar)
		{
			// ray origin is on the plane
			if(pIntersection) *pIntersection = vRayStart;
			if(pClass) *pClass = eClass;
			return true;
		}
		else
		{
			float denominator = Normal.DotProduct(vRayDir);
			if(Math::FIsZero(denominator))
			{
				// ray direction is parallel to the plane
				return false;
			}
			else
			{
				if(pIntersection) *pIntersection = vRayStart + vRayDir*(-nominator/denominator);
				if(pClass) *pClass = eClass;				
				return true;
			}
		}		
	}

	//! classifies a point with respect to the plane
	Plane::E_Classification Plane::ClassifyPoint(const Vector3& vPoint, float* pNominator /*= 0*/) const
	{
		float fNominator = Normal.DotProduct(vPoint) - D;
		if(pNominator) *pNominator = fNominator;

		if(fNominator > Math::Epsilon)
		{
			return C_Front;
		}
		else if(fNominator < -Math::Epsilon)
		{
			return C_Back;
		}
		else 
		{
 			return C_Planar;
		}
	}
}

