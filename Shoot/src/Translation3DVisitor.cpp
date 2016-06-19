/* 

Amine Rehioui
Created: April 7th 2011

*/

#include "Shoot.h"	

#include "Translation3DVisitor.h"

namespace shoot
{
	DEFINE_OBJECT(Translation3DVisitor);

	//! constructor
	Translation3DVisitor::Translation3DVisitor()	
		: m_p3DEntity(NULL)
		, m_vSrcPos(Vector3::Zero)
		, m_vDestPos(Vector3::Zero)
		// properties		
		, m_fDuration(1.0f)		
	{
	}

	//! destructor
	Translation3DVisitor::~Translation3DVisitor()
	{
	}

	//! Reads/Writes struct properties from/to a stream
	void Translation3DVisitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);	
		
		stream.Serialize("Src", &m_Src);
		stream.Serialize("Dest", &m_Dest);
		stream.Serialize("Duration", &m_fDuration);				
	}	

	//! visits a particular entity
	void Translation3DVisitor::Visit(Entity* pTarget)
	{
		SHOOT_ASSERT(pTarget->IsA(Entity3D::TypeID), "Translation3DVisitor target is not of type Entity3D");
		m_p3DEntity = static_cast<Entity3D*>(pTarget);
		m_fCurrentTime = 0.0f;
		ESP<Vector3>::Compute<Entity3D>(m_Src, m_Dest, m_vSrcPos, m_vDestPos, m_p3DEntity, &Entity3D::GetPosition);
		super::Visit(pTarget);
	}

	//! updates the visitor	
	void Translation3DVisitor::Update()
	{
		if(!m_bActive)
		{
			return;
		}
		
		if(m_fCurrentTime < m_fDuration)
		{
			Vector3 pos = Math::Lerp(m_vSrcPos, m_vDestPos, m_fCurrentTime/m_fDuration);			
			m_p3DEntity->SetPosition(pos);
			
			m_fCurrentTime += g_fDeltaTime;	
		}		
		else
		{
			m_p3DEntity->SetPosition(m_vDestPos);
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

