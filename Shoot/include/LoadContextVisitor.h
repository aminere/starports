/* 

Amine Rehioui
Created: June 16th 2013

*/

#pragma once


#include "Visitor.h"

namespace shoot
{
	//! LoadContextVisitor
	class LoadContextVisitor : public Visitor
	{
	public:

		DECLARE_OBJECT(LoadContextVisitor, Visitor);
		
		//! Reads/Writes struct properties from/to a stream
		void Serialize(PropertyStream& stream);		

		//! visits a particular entity
		void Visit(Entity* pTarget);

	protected:

		// properties
		std::string m_ContextStack;		
	};
}




