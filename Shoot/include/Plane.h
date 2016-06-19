/* 

Amine Rehioui
Created: December 18th 2010

*/

#pragma once


namespace shoot
{
	//! Plane class
	class Plane
	{
	public:

		//! classification of a point with respect to the plane
		enum E_Classification
		{
			C_Front,
			C_Back,
			C_Planar,
			C_Unknown
		};

		//! constructor
		Plane()
			: Normal(Vector3::Zero)
			, D(0.0f)
		{
		}

		//! constructor
		Plane(const Vector3& vNormal, float fDistFromOrigin)
			: Normal(vNormal)
			, D(fDistFromOrigin)
		{
		}
			
		//! constructor
		Plane(const Vector3& v1, const Vector3& v2, const Vector3& v3);

		//! returns the intersection of ray with the plane		
		bool IntersectWithRay(const Vector3& vRayStart, const Vector3& vRayDir, Vector3* pIntersection = 0, E_Classification* pClass = 0) const;

		//! classifies a point with respect to the plane
		E_Classification ClassifyPoint(const Vector3& vPoint, float* fNominator = 0) const;

		Vector3 Normal;
		float D; // distance from origin
	};
}



