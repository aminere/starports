/* 

Amine Rehioui
Created: October 30th 2013

*/

#pragma once


#include "Visitor.h"

namespace shoot
{
	//! FadeVisitor class
	class FadeVisitor : public Visitor
	{
	public:

		DECLARE_OBJECT(FadeVisitor, Visitor);

		//! constructor
		FadeVisitor();

		//! destructor
		virtual ~FadeVisitor();

		//! Reads/Writes struct properties from/to a stream
		virtual void Serialize(PropertyStream& stream);		

		//! visits a particular entity
		virtual void Visit(Entity* pTarget);
	
	protected:
		
		// properties
		Color m_SrcColor;
		Color m_DestColor;
		float m_fDuration;
		bool m_Additive;
	};
}




