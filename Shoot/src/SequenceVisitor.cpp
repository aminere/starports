/* 

Amine Rehioui
Created: April 10th 2011

*/

#include "Shoot.h"

#include "SequenceVisitor.h"

namespace shoot
{
	DEFINE_OBJECT(SequenceVisitor);

	//! constructor
	SequenceVisitor::SequenceVisitor()	
		: m_iCurrentActiveVisitor(-1)
	{
	}

	//! destructor
	SequenceVisitor::~SequenceVisitor()
	{
	}

	//! Reads/Writes struct properties from/to a stream
	void SequenceVisitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		
		stream.Serialize("Visitors", &m_aVisitors);		
	}

	//! visits a particular entity
	void SequenceVisitor::Visit(Entity* pTarget)
	{		
		// stop active visitor if any
		if(m_iCurrentActiveVisitor >= 0
		&& m_iCurrentActiveVisitor < int(m_aVisitors.GetSize())
		&& m_aVisitors[m_iCurrentActiveVisitor].IsValid()
		&& m_aVisitors[m_iCurrentActiveVisitor]->IsActive())
		{
			m_aVisitors[m_iCurrentActiveVisitor]->Leave();
		}

		// start first visitor in the sequence
		if(m_aVisitors.GetSize() > 0
		&& m_aVisitors[0].IsValid())
		{
			m_aVisitors[0]->Visit(pTarget);
			m_iCurrentActiveVisitor = 0;

			super::Visit(pTarget);
		}
		else
		{
			m_iCurrentActiveVisitor = -1;
		}
	}

	//! updates the visitor	
	void SequenceVisitor::Update()
	{
		if(!m_bActive)
			return;

		if(m_iCurrentActiveVisitor >= 0)
		{
			SHOOT_ASSERT(m_aVisitors[m_iCurrentActiveVisitor].IsValid(), "SequenceVisitor trying to update invalid visitor");
			m_aVisitors[m_iCurrentActiveVisitor]->Update();
			if(!m_aVisitors[m_iCurrentActiveVisitor]->IsActive())
			{
				// start next visitor if any
				m_iCurrentActiveVisitor++;

				if(m_iCurrentActiveVisitor < int(m_aVisitors.GetSize())
				&& m_aVisitors[m_iCurrentActiveVisitor].IsValid())
				{
					m_aVisitors[m_iCurrentActiveVisitor]->Visit(m_pTarget);
				}
				else
				{
					m_iPlayCount++;
					switch(m_ePlaybackType)
					{
						case PT_Once:
							// sequence is over
							m_iCurrentActiveVisitor = -1;
							Leave();
							break;

						case PT_Loop:
							if((m_iMaxPlayCount < 0) || (m_iPlayCount < m_iMaxPlayCount))
							{
								m_iCurrentActiveVisitor = 0;
								m_aVisitors[0]->Visit(m_pTarget);
							}
							else
							{
								Leave();
							}
							break;

						case PT_Toggle:
							SHOOT_WARNING(false, "PT_Toggle not implemented in SequenceVisitor");
							break;
					}					
				}
			}						
		}
		else
		{
			Leave();
		}
	}

	//! adds a visitor to the sequence
	void SequenceVisitor::AddVisitor(Visitor* pVisitor)
	{
		SHOOT_ASSERT(!m_bActive, "Can't add visitors while the current sequence is active");
		m_aVisitors.Add(snew Reference<Visitor>(pVisitor));
	}
}

