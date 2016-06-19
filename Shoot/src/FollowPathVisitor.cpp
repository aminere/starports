/* 

Amine Rehioui
Created: August 7th 2011

*/

#include "Shoot.h"	

#include "FollowPathVisitor.h"

#include "Path.h"

namespace shoot
{
	DEFINE_OBJECT(FollowPathVisitor);

	//! constructor
	FollowPathVisitor::FollowPathVisitor()
		: m_p3DEntity(NULL)
		, m_fTimer(0.0f)
		, m_fDuration(0.0f)
		, m_SrcElement(0)
		, m_DestElement(1)
		, m_bOrientEntity(true)		
	{
	}

	//! Reads/Writes struct properties from/to a stream
	void FollowPathVisitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("Path", &m_Path);
		stream.Serialize("OrientEntity", &m_bOrientEntity);		
	}

	//! visits a particular entity
	void FollowPathVisitor::Visit(Entity* pTarget)
	{
		m_p3DEntity = DYNAMIC_CAST(pTarget, Entity3D);
		SHOOT_ASSERT(m_p3DEntity, "FollowPathVisitor target is not of type Entity3D");

		m_Path.Init(pTarget);

		if(m_Path.Get() && (m_Path->GetChildCount() > 1))
		{
			m_SrcElement = 0;
			m_DestElement = 1;
			m_fTimer = 0.0f;
			m_fDuration = m_Path->GetElement(m_DestElement)->GetTime();
			m_p3DEntity->SetUseRotationMatrix(m_bOrientEntity);
			super::Visit(pTarget);
		}	
	}

	//! updates the visitor	
	void FollowPathVisitor::Update()
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
					SHOOT_WARNING(false, "FollowPathVisitor Unsupported PlaybackType: PT_Toggle");
					Leave();
					break;

				default:
					Leave();
				}
			}
		}

		float fRatio = m_fTimer / m_fDuration;
		Vector3 vPosition = m_Path->GetPathPosition(m_SrcElement, m_DestElement, fRatio);
		m_p3DEntity->SetAbsolutePosition(vPosition);

		if (m_bOrientEntity)
		{
			Vector3 vPosition2 = m_Path->GetPathPosition(m_SrcElement, m_DestElement, fRatio + (g_fDeltaTime / 2.0f));
			Vector3 vDirection = (vPosition2 - vPosition).DifferentiateFromUp().Normalized();
			m_p3DEntity->SetRotationMatrix(Matrix44::MakeLookAtLH(Vector3::Zero, vDirection, Vector3::Up).GetTranspose());
		}

		m_fTimer += g_fDeltaTime;
	}
}

