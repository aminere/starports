/* 

Amine Rehioui
Created: April 10th 2011

*/

#pragma once


#include "AnimationVisitor.h"

namespace shoot
{
	//! SequenceVisitor class
	class SequenceVisitor : public AnimationVisitor
	{
	public:

		DECLARE_OBJECT(SequenceVisitor, AnimationVisitor);
		
		//! constructor
		SequenceVisitor();

		//! destructor
		virtual ~SequenceVisitor();

		//! Reads/Writes struct properties from/to a stream
		virtual void Serialize(PropertyStream& stream);		

		//! visits a particular entity
		virtual void Visit(Entity* pTarget);

		//! updates the visitor
		virtual void Update();

		//! adds a visitor to the sequence
		void AddVisitor(Visitor* pVisitor);

		//! returns a visitor
		Visitor* GetVisitor(int index) const { return m_aVisitors[index]; }

	protected:

		int m_iCurrentActiveVisitor;

		// properties				
		Array < Reference<Visitor> > m_aVisitors;
	};
}



