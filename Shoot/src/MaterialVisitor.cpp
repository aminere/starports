/* 

Amine Rehioui
Created: October 27th 2013

*/

#include "Shoot.h"	

#include "MaterialVisitor.h"

namespace shoot
{
	DEFINE_OBJECT(MaterialVisitor);

	//! constructor
	MaterialVisitor::MaterialVisitor()
		// properties
		: m_fDuration(1.0f)
		, m_bRandomize(false)
	{
	}

	//! destructor
	MaterialVisitor::~MaterialVisitor()
	{
	}

	//! Reads/Writes struct properties from/to a stream
	void MaterialVisitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		
		stream.Serialize("SrcColor", &m_SrcColor);
		stream.Serialize("SrcAlpha", &m_SrcColor.A);
		stream.Serialize("DestColor", &m_DestColor);
		stream.Serialize("DestAlpha", &m_DestColor.A);
		stream.Serialize("Duration", &m_fDuration);
		stream.Serialize("Randomize", &m_bRandomize);
	}

	//! visits a particular entity
	void MaterialVisitor::Visit(Entity* pTarget)
	{
		m_fInterpolator = 0.0f;
		m_fCurrentDuration = m_bRandomize ? (Random::GetFloat(0.0f, m_fDuration)) : (m_fDuration);

		super::Visit(pTarget);
	}

	//! updates the visitor	
	void MaterialVisitor::Update()
	{
		if(!m_bActive)
			return;

		if(m_fInterpolator < 1.0f)
		{
			Color color = Math::Lerp(m_SrcColor, m_DestColor, m_fInterpolator);
			UpdateColor(color);

			m_fInterpolator += (g_fDeltaTime/m_fCurrentDuration);
		}		
		else
		{
			UpdateColor(m_DestColor);

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

	//! updates the color
	void MaterialVisitor::UpdateColor(const Color& color)
	{
		GraphicComponent* pGraphic = m_pTarget->GetComponent<GraphicComponent>();
		pGraphic->GetMaterial()->SetColor(color);		
	}
}

