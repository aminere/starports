/* 

Amine Rehioui
Created: April 7th 2011

*/

#include "Shoot.h"	

#include "Translation2DVisitor.h"

namespace shoot
{
	DEFINE_OBJECT(Translation2DVisitor);

	//! constructor
	Translation2DVisitor::Translation2DVisitor()	
		: m_p2DEntity(NULL)

		// properties		
		, m_fDuration(1.0f)		
	{
	}

	//! destructor
	Translation2DVisitor::~Translation2DVisitor()
	{
	}

	//! Reads/Writes struct properties from/to a stream
	void Translation2DVisitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);	
		
		stream.Serialize("Src", &m_Src);
		stream.Serialize("Dest", &m_Dest);
		stream.Serialize("Duration", &m_fDuration);				
	}	

	//! visits a particular entity
	void Translation2DVisitor::Visit(Entity* pTarget)
	{
		SHOOT_ASSERT(pTarget->IsA(Entity2D::TypeID), "Translation2DVisitor target is not of type Entity2D");
		m_p2DEntity = static_cast<Entity2D*>(pTarget);
		m_fCurrentTime = 0.0f;
		ESP<Vector2>::Compute<Entity2D>(m_Src, m_Dest, m_vSrcPos, m_vDestPos, m_p2DEntity, &Entity2D::GetPosition);
		super::Visit(pTarget);
	}

	//! updates the visitor	
	void Translation2DVisitor::Update()
	{
		if(!m_bActive)
		{
			return;
		}
		
		if(m_fCurrentTime < m_fDuration)
		{
			Vector2 pos = Math::Lerp(m_vSrcPos, m_vDestPos, m_fCurrentTime/m_fDuration);			
			m_p2DEntity->SetPosition(pos);

			m_fCurrentTime += g_fDeltaTime;
		}		
		else
		{
			m_p2DEntity->SetPosition(m_vDestPos);
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
					(m_ePlaybackType == PT_Toggle) ? Math::Swap(m_vSrcPos, m_vDestPos) : void();
					m_fCurrentTime = 0.0f;
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

