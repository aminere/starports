/* 

Amine Rehioui
Created: May 8th 2010

*/

#include "Shoot.h"

namespace shoot
{
	DEFINE_OBJECT(Entity3D);

	//! constructor
	Entity3D::Entity3D()
		: m_bLocalTransformationMatrixDirty(true)
		, m_bUseRotationMatrix(false)
		// properties
		, m_bAlwaysActive(false)
	{
		m_vScale = Vector3::One;
		m_vPosition = Vector3::Zero;
		m_vRotation = Vector3::Zero;
		m_vCenter = Vector3::Zero;
		m_RotationMatrix = Matrix44::Identity;
	}

	//! serializes the entity to/from a PropertyStream
	void Entity3D::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("Position", &m_vPosition);
		stream.Serialize("Rotation", &m_vRotation);
		stream.Serialize("Scale", &m_vScale);
		stream.Serialize("Center", &m_vCenter);
		stream.Serialize("AlwaysActive", &m_bAlwaysActive);

		m_bLocalTransformationMatrixDirty = true;
	}

	void Entity3D::SetAbsolutePosition(const Vector3& vAbsPos)
	{
		Vector3 vPos = vAbsPos;
		if(Entity3D* pParent = GetAncestor<Entity3D>())
		{
			Matrix44 inverse;
			if(pParent->GetWorldTransform().GetInverse(inverse))
			{
				vPos = inverse.TransformVect(vPos);
			}
		}
		SetPosition(vPos + (m_vCenter*m_vScale));
	}

	//! returns the absolute scale
	Vector3 Entity3D::GetAbsoluteScale() const
	{
		Vector3 vScale = m_vScale;
		if(Entity3D* p3DEntity = GetAncestor<Entity3D>())
		{
			vScale = vScale * p3DEntity->GetAbsoluteScale();
		}
		return vScale;
	}

	//! returns the affine transformation matrix
	Matrix44 Entity3D::GetWorldTransform()
	{
		if(m_bLocalTransformationMatrixDirty)
		{
			UpdateLocalTransformationMatrix();
		}

		Matrix44 matrix = m_LocalTransformationMatrix;		
		if(Entity3D* p3DEntity = GetAncestor<Entity3D>())
		{
			matrix *= p3DEntity->GetWorldTransform();
		}
		return matrix;
	}

	//! update the local affine transformation matrix
	void Entity3D::UpdateLocalTransformationMatrix()
	{
		m_LocalTransformationMatrix = Matrix44::Identity;
		m_LocalTransformationMatrix.Translate(-m_vCenter);
		m_LocalTransformationMatrix.Scale(m_vScale);		
		m_LocalTransformationMatrix.Rotate(m_vRotation*Math::DegToRadFactor);
		if(m_bUseRotationMatrix)
			m_LocalTransformationMatrix *= m_RotationMatrix;		
		m_LocalTransformationMatrix.Translate(m_vPosition);
		m_bLocalTransformationMatrixDirty = false;
	}

#ifdef SHOOT_EDITOR
	//! returns the affine transformation matrix for the center of the entity
	/** TODO: avoid recalculating when nothing changed */
	Matrix44 Entity3D::GetCenterTransformationMatrix()
	{
		Matrix44 centerMatrix = Matrix44::Identity;
		centerMatrix.Scale(m_vScale);
		centerMatrix.Rotate(m_vRotation*Math::DegToRadFactor);		
		centerMatrix.Translate(m_vPosition);

		// TODO: use type checking at initialization time in AddChild() and only use static_cast here		
		if(Entity3D* p3DEntity = GetAncestor<Entity3D>())
		{
			centerMatrix *= p3DEntity->GetWorldTransform();
		}
		
		Matrix44 centerTranslation = Matrix44::Identity;
		centerTranslation.SetTranslation(centerMatrix.GetTranslation());
		return centerTranslation;
	}
#endif // SHOOT_EDITOR

	//! returns the 2D bounding box used during QuadTree partitioning
	AABBox3D Entity3D::GetBoundingBox() const
	{
		Vector3 vPosition = const_cast<Entity3D*>(this)->GetWorldTransform().GetTranslation();
		return AABBox3D(Vector3::Create(vPosition.X-.5f, vPosition.Y-.5f, vPosition.Z-.5f),
						Vector3::Create(vPosition.X+.5f, vPosition.Y+.5f, vPosition.Z+.5f));
	}
}

