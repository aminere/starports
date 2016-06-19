/* 

Amine Rehioui
Created: April 1st 2011

*/

#include "ShootEditorCommon.h"

#include "Console.h"

#include "EditorIDs.h"

// used by game code to print to the editor output window
void EditorOutput(const char* strMessage)
{
	shoot::Console* pConsole = shoot::Console::Instance();
	if(pConsole)
	{
		shoot::Console::Instance()->Print(strMessage);
	}		
}

namespace shoot
{
	// static variables instantiation
	Console* Console::ms_pInstance = NULL;
	
	// Define event table
	BEGIN_EVENT_TABLE(Console, wxFrame)
		EVT_COMMAND(ID_ConsoleClear, wxEVT_COMMAND_BUTTON_CLICKED, OnClear)
	END_EVENT_TABLE()

	//! Constructor
	Console::Console()
		: super(NULL, wxID_ANY, "Console", wxDefaultPosition, wxSize(1024, 768))
	{
		ms_pInstance = this;

		wxBoxSizer *pCommands = new wxBoxSizer( wxHORIZONTAL );
		pCommands->Add(new wxButton(this, ID_ConsoleClear, "Clear", wxDefaultPosition, wxDefaultSize), 0, wxALIGN_CENTRE_VERTICAL);

		m_pTextControl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
		
		wxBoxSizer *pSizer = new wxBoxSizer( wxVERTICAL );
		pSizer->Add(pCommands);
		pSizer->Add(m_pTextControl, wxSizerFlags(1).Expand());

		SetSizer(pSizer); // use the sizer for layout

		pSizer->FitInside(this);

		// TODO: initialize console with log text
	}

	// Destructor
	Console::~Console()
	{
		ms_pInstance = NULL;
	}

	//! prints a string into the console		
	void Console::Print(const char* strMessage)
	{
		m_pTextControl->AppendText(strMessage);
	}	

	// event handlers
	void Console::OnClear(wxCommandEvent& event)
	{
		m_pTextControl->Clear();
	}
}

