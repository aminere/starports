/* 

Amine Rehioui
Created: August 14th 2011

*/

#include "Shoot.h"	

#include "FollowCameraPathVisitor.h"

#include "Camera.h"
#include "CameraPath.h"

namespace shoot
{
	DEFINE_OBJECT(FollowCameraPathVisitor);

	//! constructor
	FollowCameraPathVisitor::FollowCameraPathVisitor()
		: m_pCamera(NULL)
		, m_fTimer(0.0f)
		, m_fDuration(0.0f)
		, m_SrcElement(0)
		, m_DestElement(1)
	{
	}

	//! Reads/Writes struct properties from/to a stream
	void FollowCameraPathVisitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("CameraPath", &m_Path);
	}

	//! visits a particular entity
	void FollowCameraPathVisitor::Visit(Entity* pTarget)
	{
		m_pCamera = DYNAMIC_CAST(pTarget, Camera);		
		SHOOT_ASSERT(m_pCamera, "FollowCameraPathVisitor target is not of type Camera");

		m_Path.Init(pTarget);

		if(m_pCamera && m_Path.Get() && (m_Path->GetChildCount() > 1))
		{
			m_SrcElement = 0;
			m_DestElement = 1;
			m_fTimer = 0.0f;
			m_fDuration = m_Path->GetElement(m_DestElement)->GetTime();

			super::Visit(pTarget);
		}
	}

	//! updates the visitor	
	void FollowCameraPathVisitor::Update()
	{
		if(!m_bActive)
			return;		

		if(m_fTimer > m_fDuration)
		{
			if ((m_SrcElement < int(m_Path->GetChildCount()) - 2)
				|| (m_Path->IsWrapAround() && m_SrcElement < int(m_Path->GetChildCount()) - 1))
			{
				++m_SrcElement;
				m_DestElement = (m_SrcElement + 1) % m_Path->GetChildCount();

				float overTime = m_fTimer - m_fDuration;
				m_fTimer = overTime;
				m_fDuration = (m_Path->GetElement(m_DestElement)->GetTime() - m_Path->GetElement(m_SrcElement)->GetTime());
			}
			else
			{
				m_iPlayCount++;
				switch (m_ePlaybackType)
				{
				case PT_Once:
					Leave();
					break;

				case PT_Loop:
					if ((m_iMaxPlayCount < 0) || (m_iPlayCount < m_iMaxPlayCount))
					{
						m_SrcElement = 0;
						m_DestElement = 1;

						float overTime = m_fTimer - m_fDuration;
						m_fTimer = overTime;
						m_fDuration = m_Path->GetElement(m_DestElement)->GetTime();
					}
					else
					{
						Leave();
					}
					break;

				case PT_Toggle:
					SHOOT_WARNING(false, "FollowCameraPathVisitor Unsupported PlaybackType: PT_Toggle");
					Leave();
					break;

				default:
					Leave();
				}
			}
		}		

		float fRatio = m_fTimer / m_fDuration;
		m_pCamera->SetPosition(m_Path->GetPathPosition(m_SrcElement, m_DestElement, fRatio));
		m_pCamera->SetLookAt(m_Path->GetLookAt(m_SrcElement, m_DestElement, fRatio));

		m_fTimer += g_fDeltaTime;
	}
}

