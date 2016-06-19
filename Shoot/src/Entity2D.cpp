/* 

Amine Rehioui
Created: February 22th 2010

*/

#include "Shoot.h"

#include "LayoutComponent.h"

namespace shoot
{
	DEFINE_OBJECT(Entity2D);

	//! constructor
	Entity2D::Entity2D()
		// properties
		: m_vPosition(Vector2::Zero)
		, m_fRotation(0.0f)
		, m_vScale(Vector2::One)
		, m_vCenter(Vector2::Zero)
		, m_bHFlip(false)
		, m_bVFlip(false)
		, m_bLocalTransformationMatrixDirty(true)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void Entity2D::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("Position", &m_vPosition);
		stream.Serialize("Rotation", &m_fRotation);
		stream.Serialize("Scale", &m_vScale);
		stream.Serialize("Center", &m_vCenter);
		stream.Serialize("BBox", &m_BoundingBox);	

		stream.Serialize("HFlip", &m_bHFlip);
		stream.Serialize("VFlip", &m_bVFlip);

		m_bLocalTransformationMatrixDirty = true;
	}

	//! Sets the absolute position
	void Entity2D::SetAbsolutePosition(const Vector2& vAbsPos)
	{
		Vector3 vPos = Vector3::Create(vAbsPos.X, vAbsPos.Y, 0.0f);
		if(Entity2D* pParent = GetAncestor<Entity2D>())
		{
			Matrix44 inverse;
			if(pParent->GetWorldTransform().GetInverse(inverse))
				vPos = inverse.TransformVect(vPos);
		}
		SetPosition(vPos.XY);
	}

	//! returns the absolute scale
	Vector2 Entity2D::GetAbsoluteScale() const
	{
		auto scale = m_vScale;

		if(Entity2D* p2DEntity = GetAncestor<Entity2D>())
			scale *= p2DEntity->GetAbsoluteScale();

		return scale;
	}

	//! Translates the entity
	void Entity2D::Translate(const Vector2& amount)
	{
		m_vPosition += amount;
		m_bLocalTransformationMatrixDirty = true;
	}

	//! returns the affine transformation matrix
	/** TODO: avoid recalculating when nothing changed */
	Matrix44 Entity2D::GetWorldTransform()
	{
		if(m_bLocalTransformationMatrixDirty)
			UpdateLocalTransformationMatrix();

		auto matrix = m_LocalTransformationMatrix;
		if(Entity2D* p2DEntity = GetAncestor<Entity2D>())
			matrix *= p2DEntity->GetWorldTransform();

		return matrix;
	}	

	//! update the local affine transformation matrix
	void Entity2D::UpdateLocalTransformationMatrix()
	{
#ifndef SHOOT_EDITOR
		m_bLocalTransformationMatrixDirty = false;
#endif // SHOOT_EDITOR

		auto layout = GetComponent<LayoutComponent>();
		if(layout && !GetAncestor<Entity2D>())
		{
			m_LocalTransformationMatrix = layout->Align(this, m_vPosition);
			return;
		}

		m_LocalTransformationMatrix = Matrix44::Identity;
		m_LocalTransformationMatrix.Translate(-Vector3::Create(m_vCenter.X, m_vCenter.Y, 0.0f));
		m_LocalTransformationMatrix.Scale(Vector3::Create(m_vScale.X, m_vScale.Y, 1.0f));
		m_LocalTransformationMatrix.Rotate(Vector3::Create(0.0f, 0.0f, m_fRotation*Math::DegToRadFactor));
		m_LocalTransformationMatrix.Translate(Vector3::Create(Math::Floor(m_vPosition.X), Math::Floor(m_vPosition.Y), 0.0f));
	}

#ifdef SHOOT_EDITOR
	//! returns the affine transformation matrix for the center of the entity
	/** TODO: avoid recalculating when nothing changed */
	Matrix44 Entity2D::GetCenterTransformationMatrix()
	{
		auto layout = GetComponent<LayoutComponent>();
		if(layout && !GetAncestor<Entity2D>())
			return m_LocalTransformationMatrix;

		Matrix44 matrix = Matrix44::Identity;
		matrix.Scale(Vector3::Create(m_vScale.X, m_vScale.Y, 1.0f));
		matrix.Rotate(Vector3::Create(0.0f, 0.0f, m_fRotation*Math::DegToRadFactor));		
		matrix.Translate(Vector3::Create(m_vPosition.X, m_vPosition.Y, 0.0f));

		if(Entity2D* pEntity2D = GetAncestor<Entity2D>())
			matrix *= pEntity2D->GetWorldTransform();
		
		return matrix;
	}
#endif // SHOOT_EDITOR
}

