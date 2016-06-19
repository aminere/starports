/* 

Amine Rehioui
Created: July 14th 2012

*/

#pragma once


#include "Visitor.h"

namespace shoot
{
	//! RemoveEntityVisitor class
	class RemoveEntityVisitor : public Visitor
	{
	public:

		DECLARE_OBJECT(RemoveEntityVisitor, Visitor);

		//! visits a particular entity
		void Visit(Entity* pTarget)
		{
			pTarget->GetParent()->RemoveChild(pTarget);			
		}
	};
}



