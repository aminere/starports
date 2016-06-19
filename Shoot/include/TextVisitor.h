/* 

Amine Rehioui
Created: June 1st 2013

*/

#pragma once


#include "Visitor.h"

namespace shoot
{
	// forwards 
	class Text;

	//! TextVisitor class
	class TextVisitor : public Visitor
	{
	public:

		DECLARE_OBJECT(TextVisitor, Visitor);

		//! constructor
		TextVisitor();

		//! destructor
		virtual ~TextVisitor();

		//! Reads/Writes struct properties from/to a stream
		virtual void Serialize(PropertyStream& stream);		

		//! visits a particular entity
		virtual void Visit(Entity* pTarget);

		//! updates the visitor
		virtual void Update();

		//! Leaves the visited entity
		void Leave();

		//! SetText
		inline void SetText(const std::wstring& text) { m_Text = text; }

	protected:

		Text* m_pText;
		float m_fCurrentTime;

		// properties
		std::wstring m_Text;
		float m_fDuration;
	};
}




