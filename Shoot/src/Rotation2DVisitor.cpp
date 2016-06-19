/* 

Amine Rehioui
Created: April 7th 2011

*/

#include "Shoot.h"	

#include "Rotation2DVisitor.h"

namespace shoot
{
	DEFINE_OBJECT(Rotation2DVisitor);

	//! constructor
	Rotation2DVisitor::Rotation2DVisitor()	
		: m_p2DEntity(NULL)

		// properties
		, m_fSrcAngle(0.0f)
		, m_fDestAngle(0.0f)
		, m_fDuration(1.0f)
	{
	}

	//! destructor
	Rotation2DVisitor::~Rotation2DVisitor()
	{
	}

	//! Reads/Writes struct properties from/to a stream
	void Rotation2DVisitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);	

		stream.Serialize("Src", &m_Src);
		stream.Serialize("Dest", &m_Dest);
		stream.Serialize("Duration", &m_fDuration);		
	}

	//! visits a particular entity
	void Rotation2DVisitor::Visit(Entity* pTarget)
	{
		SHOOT_ASSERT(pTarget->IsA(Entity2D::TypeID), "Rotation2DVisitor target is not of type Entity2D");
		m_p2DEntity = static_cast<Entity2D*>(pTarget);
		m_fCurrentTime = 0.0f;
		ESP<float>::Compute<Entity2D>(m_Src, m_Dest, m_fSrcAngle, m_fDestAngle, m_p2DEntity, &Entity2D::GetRotation);
		super::Visit(pTarget);
	}

	//! updates the visitor	
	void Rotation2DVisitor::Update()
	{
		if(!m_bActive)
		{
			return;
		}

		if(m_fCurrentTime < m_fDuration)
		{
			float angle = Math::Lerp(m_fSrcAngle, m_fDestAngle, m_fCurrentTime/m_fDuration);			
			m_p2DEntity->SetRotation(angle);

			m_fCurrentTime += g_fDeltaTime;
		}		
		else
		{
			m_p2DEntity->SetRotation(m_fDestAngle);
			m_iPlayCount++;

			switch(m_ePlaybackType)
			{
			case PT_Once:
				Leave();
				break;

			case PT_Loop:
			case PT_Toggle:	
				if((m_iMaxPlayCount < 0) || (m_iPlayCount < m_iMaxPlayCount))
				{
					(m_ePlaybackType == PT_Toggle) ? Math::Swap(m_fSrcAngle, m_fDestAngle) : void();
					m_fCurrentTime = 0.0f;
					m_bActive = true;
				}
				else
				{
					Leave();
				}
				break;
			
			default:
				Leave();							
			}			
		}
	}
}

