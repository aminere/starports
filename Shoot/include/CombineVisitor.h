/* 

Amine Rehioui
Created: August 7th 2011

*/

#pragma once


#include "Visitor.h"

namespace shoot
{
	//! CombineVisitor class
	class CombineVisitor : public Visitor
	{
	public:

		DECLARE_OBJECT(CombineVisitor, Visitor);
		
		//! constructor
		CombineVisitor();

		//! destructor
		virtual ~CombineVisitor();

		//! Reads/Writes struct properties from/to a stream
		virtual void Serialize(PropertyStream& stream);		

		//! visits a particular entity
		virtual void Visit(Entity* pTarget);

		//! updates the visitor		
		virtual void Update();

	protected:

		// properties
		Array < Reference<Visitor> > m_aVisitors;
	};
}



