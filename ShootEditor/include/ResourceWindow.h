/* 

Amine Rehioui
Created: April 18th 2010

*/

#pragma once


#include <wx/notebook.h>

namespace shoot
{
	//! A class to manage the resources allocated by the engine
	class ResourceWindow : public wxNotebook
	{
		typedef wxNotebook super;

	public:
		
		//! Constructor
		ResourceWindow(wxWindow* pParent,	
					   wxWindowID id = wxID_ANY, 
					   const wxPoint& pos = wxDefaultPosition, 
					   const wxSize& size = wxSize(250, 300), 
					   long style = 0);

		//! Destructor
		virtual ~ResourceWindow()
		{
		}

	private:

		DECLARE_EVENT_TABLE();  
	};
}




