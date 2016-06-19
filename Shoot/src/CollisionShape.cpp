/*	

Amine Rehioui
Created: December 28th 2010

*/

#include "Shoot.h"

#include "CollisionShape.h"

#include "CollisionUtils.h"

namespace shoot
{
	DEFINE_OBJECT(CollisionShape);
	DEFINE_OBJECT(CollisionShapeBox);
	DEFINE_OBJECT(CollisionShapeSphere);

	//! constructor
	CollisionShapeBox::CollisionShapeBox()
		: m_Box(Vector3::Zero, Vector3::One)
	{
	}

	//! Reads/Writes struct properties from/to a stream
	void CollisionShapeBox::Serialize(PropertyStream& stream)
	{
		stream.Serialize("Box", &m_Box);
	}

	//! collision test
	bool CollisionShapeBox::CollidesWith(CollisionShape* pOther, const CollisionShapeTransform& transform, const CollisionShapeTransform& otherTransform, CollisionInfo& info)
	{
		InitCollisionInfo(pOther, info);

		if(CollisionShapeBox* pBox = DYNAMIC_CAST(pOther, CollisionShapeBox))
		{
			AABBox3D box1 = (GetBox()*transform.GetScale())+transform.GetMatrix().GetTranslation();
			AABBox3D box2 = (pBox->GetBox()*otherTransform.GetScale())+otherTransform.GetMatrix().GetTranslation();
			if(CollisionUtils::AABBox3DIntersectsWithAABBox3D(box1, box2))
			{
				info.vIntersection = ((box1.Min()+box1.Size()/2.0f) + (box2.Min()+box2.Size()/2.0f))/2.0f;
				return true;
			}
		}
		else if(CollisionShapeSphere* pSphere = DYNAMIC_CAST(pOther, CollisionShapeSphere))
		{
			AABBox3D box = (GetBox()*transform.GetScale())+transform.GetMatrix().GetTranslation();
			Vector3 vSphereCenter = pSphere->GetCenter(otherTransform.GetMatrix());
			float fSphereRadius = pSphere->GetRadius()*otherTransform.GetScale().X;
			if(CollisionUtils::AABBox3DIntersectsWithSphere(box, vSphereCenter, fSphereRadius))
			{
				Vector3 vBoxCenter = (box.Min()+box.Size()/2.0f);
				Vector3 vToBox = (vBoxCenter-vSphereCenter).Normalize();
				info.vIntersection = vSphereCenter + vToBox*fSphereRadius;
				return true;
			}
		}

		return false;
	}

	//! constructor
	CollisionShapeSphere::CollisionShapeSphere()
		: m_fRadius(0.0f)
	{
		m_vCenter = Vector3::Zero;
	}

	//! Reads/Writes struct properties from/to a stream
	void CollisionShapeSphere::Serialize(PropertyStream& stream)
	{
		stream.Serialize("Radius", &m_fRadius);
		stream.Serialize("Center", &m_vCenter);
	}

	//! collision test
	bool CollisionShapeSphere::CollidesWith(CollisionShape* pOther, const CollisionShapeTransform& transform, const CollisionShapeTransform& otherTransform, CollisionInfo& info)
	{
		InitCollisionInfo(pOther, info);

		if(CollisionShapeSphere* pSphere = DYNAMIC_CAST(pOther, CollisionShapeSphere))
		{
			Vector3 vSphereCenter1 = GetCenter(transform.GetMatrix());
			Vector3 vSphereCenter2 = pSphere->GetCenter(otherTransform.GetMatrix());
			if(CollisionUtils::SphereIntersectsWithSphere(vSphereCenter1, GetRadius()*transform.GetScale().X, vSphereCenter2, pSphere->GetRadius()*otherTransform.GetScale().X))
			{
				info.vIntersection = (vSphereCenter1 + vSphereCenter2)/2.0f;
				return true;
			}
		}
		else if(CollisionShapeBox* pBox = DYNAMIC_CAST(pOther, CollisionShapeBox))
		{
			AABBox3D box = (pBox->GetBox()*otherTransform.GetScale())+otherTransform.GetMatrix().GetTranslation();
			Vector3 vSphereCenter = GetCenter(transform.GetMatrix());
			float fRadius = GetRadius()*transform.GetScale().X;
			if(CollisionUtils::AABBox3DIntersectsWithSphere(box, vSphereCenter, fRadius))
			{
				Vector3 vBoxCenter = (box.Min()+box.Size()/2.0f);
				Vector3 vToBox = (vBoxCenter-vSphereCenter).Normalize();
				info.vIntersection = vSphereCenter + vToBox*fRadius;
				return true;
			}
		}

		return false;
	}

	//! returns the transformed center
	Vector3 CollisionShapeSphere::GetCenter(const Matrix44& matrix) const
	{
		Matrix44 m = Matrix44::Identity;
		m.Translate(m_vCenter);
		return (m * matrix).GetTranslation();
	}
}

