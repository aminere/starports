/* 

Amine Rehioui
Created: May 5th 2014

*/

#include "Shoot.h"	

#include "LightVisitor.h"
#include "Light.h"

namespace shoot
{
	DEFINE_OBJECT(LightVisitor);

	//! constructor
	LightVisitor::LightVisitor()
		: m_SrcColor(Color::White)
		// properties
		, m_fDuration(1.0f)
		, m_DestColor(Color::Black)
	{
	}

	//! destructor
	LightVisitor::~LightVisitor()
	{
	}

	//! Reads/Writes struct properties from/to a stream
	void LightVisitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);		
		
		stream.Serialize("DestColor", &m_DestColor);
		stream.Serialize("DestAlpha", &m_DestColor.A);
		stream.Serialize("Duration", &m_fDuration);
	}

	//! visits a particular entity
	void LightVisitor::Visit(Entity* pTarget)
	{
		SHOOT_ASSERT(pTarget->IsA(Light::TypeID), "LightVisitor target not of type Light");
		m_fInterpolator = 0.0f;		
		m_SrcColor = static_cast<Light*>(pTarget)->GetColor();

		super::Visit(pTarget);
	}

	//! updates the visitor	
	void LightVisitor::Update()
	{
		if(!m_bActive)
		{
			return;
		}

		if(m_fInterpolator < 1.0f)
		{
			Color color = Math::Lerp(m_SrcColor, m_DestColor, m_fInterpolator);
			static_cast<Light*>(m_pTarget)->SetColor(color);

			m_fInterpolator += (g_fDeltaTime/m_fDuration);
		}		
		else
		{
			static_cast<Light*>(m_pTarget)->SetColor(m_DestColor);

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
					(m_ePlaybackType == PT_Toggle) ? Math::Swap(m_SrcColor, m_DestColor) : void();
					m_fInterpolator = 0.0f;
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

