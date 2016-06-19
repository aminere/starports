/* 

Amine Rehioui
Created: April 6th 2014

*/

#include "Shoot.h"

#include "Text3D.h"

namespace shoot
{
	DEFINE_OBJECT(Text3D);

	//! returns the affine transformation matrix
	/** TODO: avoid recalculating when nothing changed */
	Matrix44 Text3D::GetWorldTransform()
	{
		if(Camera* pCamera = EntityRenderer::Instance()->Get3DCamera())
		{
			if(Entity3D* p3DEntity = GetAncestor<Entity3D>())
			{
				Vector3 vScreenPos;
				if(pCamera->GetScreenPos(p3DEntity->GetAbsolutePosition(), vScreenPos))
				{
					m_LocalTransformationMatrix = Matrix44::Identity;
					m_LocalTransformationMatrix.Translate(-Vector3::Create(m_vCenter.X, m_vCenter.Y, 0.0f));
					m_LocalTransformationMatrix.Scale(Vector3::Create(m_vScale.X, m_vScale.Y, 1.0f));
					m_LocalTransformationMatrix.Rotate(Vector3::Create(0.0f, 0.0f, m_fRotation*Math::DegToRadFactor));
					m_LocalTransformationMatrix.Translate(Vector3::Create(m_vPosition.X+vScreenPos.X, m_vPosition.Y+vScreenPos.Y, 0.0f));					
				}
				else
				{
					// MEGA HACK, find a better way to alter the visibility
					m_LocalTransformationMatrix.Translate(Vector3::One*9999.0f);
				}
				return m_LocalTransformationMatrix;
			}
		}
		return super::GetWorldTransform();		
	}
}

