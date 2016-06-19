/* 

Amine Rehioui
Created: April 10th 2011

*/

#pragma once


#include "Visitor.h"

namespace shoot
{
	//! DelayVisitor class
	class DelayVisitor : public Visitor
	{
	public:

		DECLARE_OBJECT(DelayVisitor, Visitor);
		
		//! constructor
		DelayVisitor();

		//! destructor
		virtual ~DelayVisitor();

		//! Reads/Writes struct properties from/to a stream
		virtual void Serialize(PropertyStream& stream);		

		//! visits a particular entity
		virtual void Visit(Entity* pTarget);

		//! updates the visitor		
		virtual void Update();

		//! sets the delay duration
		inline void SetDuration(float fDuration) { m_fDuration = fDuration; }

		//! gets the duration
		inline float GetDuration() const { return m_fDuration; }

	protected:		

		float m_fCurrentTime;
		float m_fCurrentDuration;

		// properties				
		float m_fDuration;
		bool m_bRandomize;
	};
}



