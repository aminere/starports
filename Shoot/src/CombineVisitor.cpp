/* 

Amine Rehioui
Created: August 7th 2011

*/

#include "Shoot.h"

#include "CombineVisitor.h"

namespace shoot
{
	DEFINE_OBJECT(CombineVisitor);

	//! constructor
	CombineVisitor::CombineVisitor()	
	{
	}

	//! destructor
	CombineVisitor::~CombineVisitor()
	{
	}

	//! Reads/Writes struct properties from/to a stream
	void CombineVisitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);		
		
		stream.Serialize("Visitors", &m_aVisitors);		
	}

	//! visits a particular entity
	void CombineVisitor::Visit(Entity* pTarget)
	{		
		// stop active visitor if any
		for (size_t i=0; i<m_aVisitors.GetSize(); ++i)
		{
			if(m_aVisitors[i].IsValid()
			&& m_aVisitors[i]->IsActive())
			{
				m_aVisitors[i]->Leave();
			}
		}

		// starts the visitors
		bool bStarted = false;
		for (size_t i=0; i<m_aVisitors.GetSize(); ++i)
		{
			if(m_aVisitors[i].IsValid())
			{
				m_aVisitors[i]->Visit(pTarget);
				bStarted = true;
			}
		}
		if(bStarted)
		{
			super::Visit(pTarget);
		}
	}

	//! updates the visitor	
	void CombineVisitor::Update()
	{
		if(!m_bActive)
		{
			return;
		}

		bool bActive = false;

		// update active visitors
		for (size_t i=0; i<m_aVisitors.GetSize(); ++i)
		{
			if(m_aVisitors[i].IsValid()
			&& m_aVisitors[i]->IsActive())
			{
				m_aVisitors[i]->Update();
				bActive = true;
			}
		}
		
		m_bActive = bActive;
	}
}

