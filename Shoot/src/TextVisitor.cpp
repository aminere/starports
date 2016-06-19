/* 

Amine Rehioui
Created: June 1st 2013

*/

#include "Shoot.h"	

#include "TextVisitor.h"

#include "Text.h"

namespace shoot
{
	DEFINE_OBJECT(TextVisitor);

	//! constructor
	TextVisitor::TextVisitor()
		: m_fCurrentTime(0.0f)
		// properties
		, m_fDuration(0.4f)
	{
	}

	//! destructor
	TextVisitor::~TextVisitor()
	{
	}

	//! Reads/Writes struct properties from/to a stream
	void TextVisitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
				
		stream.Serialize("Text", &m_Text);
		stream.Serialize("Duration", &m_fDuration);
	}

	//! visits a particular entity
	void TextVisitor::Visit(Entity* pTarget)
	{
		SHOOT_ASSERT(pTarget->IsA(Text::TypeID), "TextVisitor target is not of type Text");
		m_pText = static_cast<Text*>(pTarget);
	
		m_fCurrentTime = 0.0f;
		m_pText->SetText(m_Text.c_str());
		m_pText->SetDesiredVertexCount(0);
		m_pText->SetVisible(true);

		super::Visit(pTarget);
	}

	//! updates the visitor	
	void TextVisitor::Update()
	{
		if(!m_bActive)
		{
			return;
		}
		
		if(m_fCurrentTime < m_fDuration)
		{
			if (VertexBuffer* pVB = m_pText->GetComponent<GraphicComponent>()->GetVertexBuffer())
			{
				float fRatio = m_fCurrentTime / m_fDuration;
				int maxCharacters = pVB->GetMaxVertices() / 6;
				int characterCount = int(fRatio*maxCharacters);
				m_pText->SetDesiredVertexCount(characterCount * 6);

				m_fCurrentTime += g_fDeltaTime;
			}			
		}
		else
		{
			Leave();
		}
	}

	//! Leaves the visited entity
	void TextVisitor::Leave()
	{
		m_pText->SetDesiredVertexCount(-1);
		super::Leave();
	}
}

