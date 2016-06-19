/* 

Amine Rehioui
Created: October 27th 2013

*/

#include "Shoot.h"	

#include "FadeVisitor.h"

#include "FadeRenderer.h"

namespace shoot
{
	DEFINE_OBJECT(FadeVisitor);

	//! constructor
	FadeVisitor::FadeVisitor()
		// properties
		: m_SrcColor(Color::White)
		, m_DestColor(Color::Black)
		, m_fDuration(1.0f)
		, m_Additive(false)
	{
	}

	//! destructor
	FadeVisitor::~FadeVisitor()
	{
	}

	//! Reads/Writes struct properties from/to a stream
	void FadeVisitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		
		stream.Serialize("SrcColor", &m_SrcColor);
		stream.Serialize("SrcAlpha", &m_SrcColor.A);
		stream.Serialize("DestColor", &m_DestColor);
		stream.Serialize("DestAlpha", &m_DestColor.A);
		stream.Serialize("Duration", &m_fDuration);
		stream.Serialize("Additive", &m_Additive);		
	}

	//! visits a particular entity
	void FadeVisitor::Visit(Entity* pTarget)
	{
		FadeRenderer::Instance()->StartFade(m_SrcColor, m_DestColor, m_fDuration, m_Additive);
		Leave();
	}
}

