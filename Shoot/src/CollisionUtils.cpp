/* 

Amine Rehioui
Created: October 15th 2011

*/

#include "Shoot.h"

#include "CollisionUtils.h"

#ifdef SHOOT_EDITOR
#include "Plane.h"
#include "Triangle.h"
#endif // SHOOT_EDITOR

namespace shoot
{
	//! 3D ray intersection check
	bool CollisionUtils::AABBox3DIntersectsWithRay(const AABBox3D& box, const Vector3& rayStart, const Vector3& rayEnd, Vector3* pIntersection /*= 0*/)
	{
#ifdef SHOOT_EDITOR
		return	pIntersection
			? AABBox3DSlowRayIntersectionCheck(box, rayStart, rayEnd, pIntersection) 
			: AABBox3DFastRayIntersectionCheck(box, rayStart, rayEnd);
#else
		return AABBox3DFastRayIntersectionCheck(box, rayStart, rayEnd);
#endif // SHOOT_EDITOR
	}

	//! AABBox3D vs AABBox3D intersection check
	bool CollisionUtils::AABBox3DIntersectsWithAABBox3D(const AABBox3D& box1, const AABBox3D& box2)
	{
		for(int i=0; i<3; ++i)
		{
			if(box1.Min()[i] > box2.Max()[i])
			{
				return false;
			}
		}

		for(int i=0; i<3; ++i)
		{
			if(box2.Min()[i] > box1.Max()[i])
			{
				return false;
			}
		}

		return true;
	}

	//! AABBox2D vs AABBox2D intersection check
	bool CollisionUtils::AABBox2DIntersectsWithAABBox2D(const AABBox2D& box1, const AABBox2D& box2)
	{
		if(box1.Min().X >= box2.Max().X
		|| box1.Min().Y >= box2.Max().Y)
		{
			return false;
		}

		if(box2.Min().X >= box1.Max().X
		|| box2.Min().Y >= box1.Max().Y)
		{
			return false;
		}

		return true;
	}

	//! AABBox3D vs 3D sphere intersection check
	bool CollisionUtils::AABBox3DIntersectsWithSphere(const AABBox3D& box, const Vector3& sphereCenter, float sphereRadius)
	{
		float s, d = 0.0f;

		//find the square of the distance from the sphere to the box
		for(int i=0; i<3; ++i) 
		{
			if(sphereCenter[i] < box.Min()[i])
			{
				s = sphereCenter[i] - box.Min()[i];
				d += s*s;
			}
			else if(sphereCenter[i] > box.Max()[i])
			{
				s = sphereCenter[i] - box.Max()[i];
				d += s*s;
			}
		}

		return d <= (sphereRadius*sphereRadius);
	}

	//! 3D sphere vs 3D sphere intersection check
	bool CollisionUtils::SphereIntersectsWithSphere(const Vector3& sphere1Center, float sphere1Radius, const Vector3& sphere2Center, float sphere2Radius)
	{
		float distSquared = (sphere1Center-sphere2Center).GetLengthSquared();
		float radii = sphere1Radius+sphere2Radius;
		return (distSquared < (radii*radii));
	}

	//! 3D sphere vs ray intersection check
	bool CollisionUtils::SphereIntersectsWithRay(const Vector3& vCenter, float fRadius, const Vector3& vRayStart, const Vector3& vRayDir, float* fRayLength /*= 0*/, float* fDistToIntersection /*= 0*/)
	{
		Vector3 vToSphere = vCenter-vRayStart;
		float fProj = vRayDir.DotProduct(vToSphere); // project vToSphere onto vRayDir
		if(fProj < 0.0f)
			return false;

		Vector3 vProj = vRayStart + vRayDir*fProj;
		float fToProjSq = (vCenter-vProj).GetLengthSquared();
		float fRadiusSq = fRadius*fRadius;
		if(fToProjSq >= fRadiusSq)
			return false; // projection is outside the sphere		

		float fD = Math::SquareRoot(fRadiusSq - fToProjSq); // Uses pythagore's A2 = B2 + C2
		float _fDistToIntersection = -1.0f;

		if(vToSphere.GetLengthSquared() > fRadiusSq)
			_fDistToIntersection = fProj - fD; // vRayStart is outside the sphere		
		else
			_fDistToIntersection = fProj + fD; // vRayStart is inside the sphere

		SHOOT_ASSERT(!(_fDistToIntersection < 0.0f), "SphereIntersectsWithRay _fDistToIntersection is Negative");
		if(fRayLength && (*fRayLength < _fDistToIntersection))
			return false;

		if(fDistToIntersection)
			*fDistToIntersection = _fDistToIntersection;
		return true;
	}

	//! Fast Ray intersection check (no intersection point calculated)
	bool CollisionUtils::AABBox3DFastRayIntersectionCheck(const AABBox3D& box, const Vector3& rayStart, const Vector3& rayEnd)
	{
		// Code taken from the Irrlicht engine:
		float rayHalfLength = (rayEnd - rayStart).GetLength() / 2.0f;
		Vector3 rayMiddle = (rayStart + rayEnd) / 2.0f;
		Vector3 rayDir = (rayEnd - rayStart).Normalize();
		const Vector3 e = box.Size() * 0.5f;
		const Vector3 t = ((box.Min() + box.Max())/2.0f) - rayMiddle;

		if ((Math::FAbs(t.X) > e.X + rayHalfLength * Math::FAbs(rayDir.X)) ||
			(Math::FAbs(t.Y) > e.Y + rayHalfLength * Math::FAbs(rayDir.Y)) ||
			(Math::FAbs(t.Z) > e.Z + rayHalfLength * Math::FAbs(rayDir.Z)) )
		{
			return false;
		}

		float r = e.Y * Math::FAbs(rayDir.Z) + e.Z * Math::FAbs(rayDir.Y);
		if (Math::FAbs(t.Y*rayDir.Z - t.Z*rayDir.Y) > r )
			return false;

		r = e.X * Math::FAbs(rayDir.Z) + e.Z * Math::FAbs(rayDir.X);
		if (Math::FAbs(t.Z*rayDir.X - t.X*rayDir.Z) > r )
			return false;

		r = e.X * Math::FAbs(rayDir.Y) + e.Y * Math::FAbs(rayDir.X);
		if (Math::FAbs(t.X*rayDir.Y - t.Y*rayDir.X) > r)
			return false;

		return true;
	}

#ifdef SHOOT_EDITOR
	//! Slower Ray intersection check, can return intersection point
	bool CollisionUtils::AABBox3DSlowRayIntersectionCheck(const AABBox3D& box, const Vector3& rayStart, const Vector3& rayEnd, Vector3* pIntersection)
	{
		Vector3 vTop1 = Vector3::Create(box.Min().X, box.Max().Y, box.Min().Z); 
		Vector3 vTop2 = Vector3::Create(box.Max().X, box.Max().Y, box.Min().Z); 
		Vector3 vTop3 = Vector3::Create(box.Min().X, box.Max().Y, box.Max().Z); 
		Vector3 vTop4 = Vector3::Create(box.Max().X, box.Max().Y, box.Max().Z); 

		Vector3 vBottom1 = Vector3::Create(box.Min().X, box.Min().Y, box.Min().Z); 
		Vector3 vBottom2 = Vector3::Create(box.Max().X, box.Min().Y, box.Min().Z); 
		Vector3 vBottom3 = Vector3::Create(box.Min().X, box.Min().Y, box.Max().Z); 
		Vector3 vBottom4 = Vector3::Create(box.Max().X, box.Min().Y, box.Max().Z); 

		Vector3 rayDir = (rayEnd - rayStart).Normalize();

		Plane Planes[6] = 
		{
			Plane(vTop1, vTop2, vTop3),
			Plane(vBottom1, vBottom2, vBottom3),			
			Plane(vTop3, vTop4, vBottom3),
			Plane(vTop1, vTop2, vBottom1),
			Plane(vTop1, vTop3, vBottom1),
			Plane(vTop2, vTop4, vBottom2)
		};

		Triangle Triangles[12] =
		{
			Triangle(vTop1, vTop2, vTop3),
			Triangle(vTop2, vTop3, vTop4),
			Triangle(vBottom1, vBottom2, vBottom3),
			Triangle(vBottom2, vBottom3, vBottom4),
			Triangle(vTop3, vTop4, vBottom3),
			Triangle(vBottom3, vTop4, vBottom4),
			Triangle(vTop1, vTop2, vBottom1),
			Triangle(vBottom1, vTop2, vBottom2),
			Triangle(vTop1, vTop3, vBottom1),
			Triangle(vBottom1, vTop3, vBottom3),
			Triangle(vTop2, vTop4, vBottom2),
			Triangle(vBottom2, vTop4, vBottom4)
		};

		// TODO: register all intersections, sort them, and pick closest to ray start
		for(int i=0; i<6; ++i)
		{
			if(Planes[i].IntersectWithRay(rayStart, rayDir, pIntersection))
			{
				if(Triangles[i*2+0].IsPointInside(*pIntersection)
				|| Triangles[i*2+1].IsPointInside(*pIntersection))
				{
					return true;
				}
			}
		}
		return false;
	}
#endif // SHOOT_EDITOR
}

