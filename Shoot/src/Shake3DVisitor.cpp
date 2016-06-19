/* 

Amine Rehioui
Created: May 23rd 2013

*/

#include "Shoot.h"	

#include "Shake3DVisitor.h"

namespace shoot
{
	DEFINE_OBJECT(Shake3DVisitor);

	//! constructor
	Shake3DVisitor::Shake3DVisitor()
		: m_fCurrentTime(0.0f)
		// properties
		, m_fDuration(1.0f)
	{
	}

	//! destructor
	Shake3DVisitor::~Shake3DVisitor()
	{
	}

	//! Reads/Writes struct properties from/to a stream
	void Shake3DVisitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);	
		
		stream.Serialize("Duration", &m_fDuration);				
	}	

	//! visits a particular entity
	void Shake3DVisitor::Visit(Entity* pTarget)
	{
		SHOOT_ASSERT(pTarget->IsA(Entity3D::TypeID), "Shake3DVisitor target is not of type Entity3D");
		m_p3DEntity = static_cast<Entity3D*>(pTarget);
		m_fCurrentTime = 0.0f;
		super::Visit(pTarget);
	}

	//! updates the visitor	
	void Shake3DVisitor::Update()
	{
		if(!m_bActive)
		{
			return;
		}
		
		if(m_fCurrentTime < m_fDuration)
		{
			// TODO shake
			m_fCurrentTime += g_fDeltaTime;	
		}		
		else
		{
			m_iPlayCount++;

			switch(m_ePlaybackType)
			{
			case PT_Once:
				Leave();
				break;

			case PT_Loop:
				if((m_iMaxPlayCount < 0) || (m_iPlayCount < m_iMaxPlayCount))
				{					
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

