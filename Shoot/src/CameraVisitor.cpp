/* 

Amine Rehioui
Created: May 31st 2013

*/

#include "Shoot.h"	

#include "CameraVisitor.h"

#include "Camera.h"

namespace shoot
{
	DEFINE_OBJECT(CameraVisitor);

	//! constructor
	CameraVisitor::CameraVisitor()
		: m_fCurrentTime(0.0f)
		, m_bFollowStarted(false)
		, m_vFollowDirection(Vector3::Zero)
		// properties
		, m_fLookAtDuration(.5f)
		, m_fDuration(5.0f)
		, m_fMaxDistToTarget(-1.0f)
	{
	}

	//! destructor
	CameraVisitor::~CameraVisitor()
	{
	}

	//! Reads/Writes struct properties from/to a stream
	void CameraVisitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		
		stream.Serialize("LookAtTarget", &m_LookAtTarget);		
		stream.Serialize("LookAtDuration", &m_fLookAtDuration);
		stream.Serialize("Duration", &m_fDuration);
		stream.Serialize("MaxDistToTarget", &m_fMaxDistToTarget);
		
	}

	//! visits a particular entity
	void CameraVisitor::Visit(Entity* pTarget)
	{
		SHOOT_ASSERT(pTarget->IsA(Camera::TypeID), "CameraVisitor target is not of type Camera");
		m_pCamera = static_cast<Camera*>(pTarget);

		m_LookAtTarget.Init(pTarget);
		if(m_LookAtTarget.Get())
		{
			m_fCurrentTime = 0.0f;
			super::Visit(pTarget);
		}
	}

	//! updates the visitor	
	void CameraVisitor::Update()
	{
		if(!m_bActive)
		{
			return;
		}
		
		if(m_fCurrentTime < m_fDuration)
		{
			Vector3 vLookAtDest = m_LookAtTarget->GetWorldTransform().GetTranslation();
			Vector3 vLookAt = Math::Damp(m_pCamera->GetLookAt(), vLookAtDest, g_fDeltaTime, m_fLookAtDuration);
			m_pCamera->SetLookAt(vLookAt);

			if(m_fMaxDistToTarget > .0f)
			{
				float fDist = (vLookAtDest-m_pCamera->GetPosition()).GetLength();
				if(fDist > m_fMaxDistToTarget)
				{
					if(!m_bFollowStarted)
					{
						m_vFollowDirection = (m_pCamera->GetPosition()-vLookAtDest).Normalize();
						m_bFollowStarted = true;
					}
					m_pCamera->SetPosition(vLookAtDest+m_vFollowDirection*m_fMaxDistToTarget);
				}
			}

			m_fCurrentTime += g_fDeltaTime;	
		}		
		else
		{
			Leave();								
		}
	}
}

