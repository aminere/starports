/* 

Amine Rehioui
Created: November 11th 2013

*/

#pragma once


#include "Button.h"

namespace shoot
{
	//! VKButton
	class VKButton : public Button				 
	{	
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(VKButton, Button);

		//! Constructor
		VKButton();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		void Init();

		//! sets the letter
		inline void SetLetter(const std::string& letter) { m_Letter = letter; }

	protected:

		//! returns true if the button is touched
		bool IsTouched() const;	

		//! sends the UI event
		void SendUIEvent();

		// properties
		std::string m_Letter;
	};
}



