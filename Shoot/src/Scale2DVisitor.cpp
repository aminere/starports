/* 

Amine Rehioui
Created: April 7th 2011

*/

#include "Shoot.h"	

#include "Scale2DVisitor.h"

namespace shoot
{
	DEFINE_OBJECT(Scale2DVisitor);

	//! constructor
	Scale2DVisitor::Scale2DVisitor()	
		: m_p2DEntity(NULL)

		// properties		
		, m_fDuration(1.0f)		
	{
	}

	//! destructor
	Scale2DVisitor::~Scale2DVisitor()
	{
	}

	//! Reads/Writes struct properties from/to a stream
	void Scale2DVisitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);	
		
		stream.Serialize("Src", &m_Src);
		stream.Serialize("Dest", &m_Dest);
		stream.Serialize("Duration", &m_fDuration);				
	}	

	//! visits a particular entity
	void Scale2DVisitor::Visit(Entity* pTarget)
	{
		SHOOT_ASSERT(pTarget->IsA(Entity2D::TypeID), "Scale2DVisitor target is not of type Entity2D");
		m_p2DEntity = static_cast<Entity2D*>(pTarget);
		m_fCurrentTime = 0.0f;
		ESP<Vector2>::Compute<Entity2D>(m_Src, m_Dest, m_vSrcScale, m_vDestScale, m_p2DEntity, &Entity2D::GetScale);			
		super::Visit(pTarget);	
	}

	//! updates the visitor	
	void Scale2DVisitor::Update()
	{
		if(!m_bActive)
		{
			return;
		} 		
		
		if(m_fCurrentTime < m_fDuration)
		{
			auto factor = m_fCurrentTime / m_fDuration;
			Vector2 scale = Math::Lerp(m_vSrcScale, m_vDestScale, factor*factor);
			m_p2DEntity->SetScale(scale);

			m_fCurrentTime += g_fDeltaTime;
		}		
		else
		{
			m_p2DEntity->SetScale(m_vDestScale);
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
					(m_ePlaybackType == PT_Toggle) ? Math::Swap(m_vSrcScale, m_vDestScale) : void();
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

