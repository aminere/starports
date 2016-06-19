/* 

Amine Rehioui
Created: April 18th 2010

*/

#include "ShootEditorCommon.h"

#include "ResourceWindow.h"

#include <wx/textctrl.h>
#include <wx/sizer.h>

namespace shoot
{
	// Define event table
	BEGIN_EVENT_TABLE(ResourceWindow, wxNotebook)
	END_EVENT_TABLE()

	//! Constructor
	ResourceWindow::ResourceWindow(wxWindow* pParent,	
								   wxWindowID id /*= wxID_ANY*/, 
								   const wxPoint& pos /*= wxDefaultPosition*/, 
								   const wxSize& size /*= wxSize(250, 300)*/, 
								   long style /*= 0*/) 
	: super(pParent, id, pos, size, style)
	{
		wxPanel* pTextures = new wxPanel(this);
		AddPage(pTextures, "Textures", true);		

		wxTextCtrl* pTextControl = new wxTextCtrl(pTextures, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
		pTextControl->SetBackgroundColour(wxColour(180, 180, 180, 255));

		wxBoxSizer *pSizer = new wxBoxSizer( wxVERTICAL );
		pSizer->Add(pTextControl, 
					1, // make vertically stretchable
					wxEXPAND | // make horizontally stretchable
					wxALL, //   and make border all around
					0); // set border width to 10		

		pTextures->SetSizer(pSizer); // use the sizer for layout
		pSizer->FitInside(pTextures);

		AddPage(new wxPanel(this), "Scripts");
	}	
}

