/* 

Amine Rehioui
Created: May 8th 2010

*/

#pragma once

#include "RenderableEntity.h"

namespace shoot
{
	//! Base 3D entity class
	class Entity3D : public RenderableEntity
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(Entity3D, RenderableEntity);

		//! constructor
		Entity3D();

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! Sets the position relative to the parent's position
		virtual void SetPosition(const Vector3& vPos) 
		{ 
			m_vPosition = vPos;
			m_bLocalTransformationMatrixDirty = true;
		}

		//! Sets the absolute position
		void SetAbsolutePosition(const Vector3& vAbsPos);

		//! Gets the absolute position
		inline Vector3 GetAbsolutePosition() { return GetWorldTransform().GetTranslation(); }

		//! returns the absolute scale
		Vector3 GetAbsoluteScale() const;

		//! Translates the entity
		virtual void Translate(const Vector3& amount)
		{
			m_vPosition += amount;
			m_bLocalTransformationMatrixDirty = true;
		}

		//! sets the scaling vector
		virtual void SetScale(const Vector3& vScale) 
		{ 
			m_vScale = vScale;
			m_bLocalTransformationMatrixDirty = true;
		}

		//! Get the position relative to the parent's position
		inline const Vector3& GetPosition() const { return m_vPosition; }

		//! returns the rotation
		inline const Vector3& GetRotation() const { return m_vRotation; }

		//! returns the scaling vector
		inline const Vector3& GetScale() const { return m_vScale; }

		//! scales the entity
		inline void Scale(const Vector3& vScaleDelta)
		{ 
			m_vScale += vScaleDelta;
			m_bLocalTransformationMatrixDirty = true;
		}

		//! sets the rotation
		inline void SetRotation(const Vector3& vRotation)
		{ 
			m_vRotation = vRotation;
			m_bLocalTransformationMatrixDirty = true;
		}

		//! rotates the entity
		inline void Rotate(const Vector3& vRotationDelta)
		{ 
			m_vRotation += vRotationDelta;
			m_bLocalTransformationMatrixDirty = true;
		}

		//! sets the rotation matrix
		inline void SetRotationMatrix(const Matrix44& matrix)
		{
			m_RotationMatrix = matrix;
			m_bLocalTransformationMatrixDirty = true;
		}

		//! gets the rotation matrix
		inline const Matrix44& GetRotationMatrix() const { return m_RotationMatrix; }

		//! returns the center of local transformation 
		inline const Vector3& GetCenter() const { return m_vCenter; }

		//! sets the center of local transformation
		virtual void SetCenter(const Vector3& vCenter) 
		{ 
			m_vCenter = vCenter;
			m_bLocalTransformationMatrixDirty = true;
		}

		//! sets rotation matrix use flag
		inline void SetUseRotationMatrix(bool bUse) { m_bUseRotationMatrix = bUse; }

		//! returns the affine transformation matrix
		virtual Matrix44 GetWorldTransform();

		//! returns active status
		inline bool IsAlwaysActive() const { return m_bAlwaysActive; }

#ifdef SHOOT_EDITOR
		//! returns the affine transformation matrix for the center of the entity
		/** TODO: avoid recalculating when nothing changed */
		Matrix44 GetCenterTransformationMatrix();
#endif

		//! returns the bounding box
		virtual AABBox3D GetBoundingBox() const;

	protected:

		//! update the local affine transformation matrix
		void UpdateLocalTransformationMatrix();

		Matrix44 m_LocalTransformationMatrix;
		bool m_bLocalTransformationMatrixDirty;
		bool m_bUseRotationMatrix;
		Matrix44 m_RotationMatrix;

		// properties
		Vector3 m_vPosition;
		Vector3 m_vRotation;
		Vector3 m_vScale;
		Vector3 m_vCenter;
		bool m_bAlwaysActive;
	};
}

