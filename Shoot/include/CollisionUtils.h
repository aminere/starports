/* 

Amine Rehioui
Created: October 15th 2011

*/

#pragma once


namespace shoot
{
	//! Collision utils
	class CollisionUtils
	{
	public:

		//! AABBox3D vs 3D ray intersection check
		static bool AABBox3DIntersectsWithRay(const AABBox3D& box, const Vector3& rayStart, const Vector3& rayEnd, Vector3* pIntersection = 0);

		//! AABBox3D vs AABBox3D intersection check
		static bool AABBox3DIntersectsWithAABBox3D(const AABBox3D& box1, const AABBox3D& box2);

		//! AABBox2D vs AABBox2D intersection check
		static bool AABBox2DIntersectsWithAABBox2D(const AABBox2D& box1, const AABBox2D& box2);

		//! AABBox3D vs 3D sphere intersection check
		static bool AABBox3DIntersectsWithSphere(const AABBox3D& box, const Vector3& sphereCenter, float sphereRadius);

		//! 3D sphere vs 3D sphere intersection check
		static bool SphereIntersectsWithSphere(const Vector3& sphere1Center, float sphere1Radius, const Vector3& sphere2Center, float sphere2Radius);

		//! 3D sphere vs ray intersection check
		static bool SphereIntersectsWithRay(const Vector3& vCenter, float fRadius, const Vector3& vRayStart, const Vector3& vRayDir, float* fRayLength = 0, float* fDistToIntersection = 0);

	private:

		//! Fast Ray intersection check (no intersection point calculated)
		static bool AABBox3DFastRayIntersectionCheck(const AABBox3D& box, const Vector3& rayStart, const Vector3& rayEnd);

#ifdef SHOOT_EDITOR
		//! Slower Ray intersection check, can return intersection point
		static bool AABBox3DSlowRayIntersectionCheck(const AABBox3D& box, const Vector3& rayStart, const Vector3& rayEnd, Vector3* pIntersection);
#endif // SHOOT_EDITOR
	};
}




