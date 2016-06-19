/* 

Amine Rehioui
Created: January 9th 2014

*/

#pragma once


#include "MainFrame.h"

namespace shoot
{
	//! GameFrame
	class GameFrame : public MainFrame
	{
		typedef MainFrame super;

	public:

		//! Constructor
		GameFrame();

		// Events
		void OnToggleHexMode(wxCommandEvent& event);

	private:

		DECLARE_EVENT_TABLE()
	};
}

