/* 

Amine Rehioui
Created: April 12th 2014

*/

#pragma once


#include "Button.h"

namespace shoot
{
	//! WheelMenuButton
	class WheelMenuButton : public Button				 
	{	
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(WheelMenuButton, Button);

		//! sets the alpha
		void SetAlpha(float fAlpha);
	};
}



