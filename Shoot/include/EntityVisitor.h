/* 

Amine Rehioui
Created: November 3rd 2013

*/

#pragma once


#include "Visitor.h"

namespace shoot
{
	//! EntityVisitor class
	class EntityVisitor : public Visitor
	{
	public:

		DECLARE_OBJECT(EntityVisitor, Visitor);

		//! constructor
		EntityVisitor();

		//! Reads/Writes struct properties from/to a stream
		void Serialize(PropertyStream& stream);		

		//! visits a particular entity
		virtual void Visit(Entity* pTarget);
	
	protected:
		
		// properties
		bool m_bEntityVisible;
		bool m_bCollisionEnabled;
	};
}




