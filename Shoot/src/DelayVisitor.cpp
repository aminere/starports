/* 

Amine Rehioui
Created: April 10th 2011

*/

#include "Shoot.h"

#include "DelayVisitor.h"

namespace shoot
{
	DEFINE_OBJECT(DelayVisitor);

	//! constructor
	DelayVisitor::DelayVisitor()
		: m_fCurrentTime(0.0f)
		, m_fCurrentDuration(0.0f)
		// properties		
		, m_fDuration(0.0f)
		, m_bRandomize(false)
	{
	}

	//! destructor
	DelayVisitor::~DelayVisitor()
	{
	}

	//! Reads/Writes struct properties from/to a stream
	void DelayVisitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		
		stream.Serialize("Duration", &m_fDuration);
		stream.Serialize("Randomize", &m_bRandomize);
	}

	//! visits a particular entity
	void DelayVisitor::Visit(Entity* pTarget)
	{
		m_fCurrentTime = 0.0f;
		m_fCurrentDuration = m_bRandomize ? (Random::GetFloat(0.0f, m_fDuration)) : (m_fDuration);

		super::Visit(pTarget);
	}

	//! updates the visitor	
	void DelayVisitor::Update()
	{
		if(!m_bActive)
		{
			return;
		}

		if(m_fCurrentTime < m_fCurrentDuration)
		{
			m_fCurrentTime += g_fDeltaTime;
		}
		else
		{
			Leave();
		}
	}
}

