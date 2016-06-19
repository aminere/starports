/* 

Amine Rehioui
Created: March 31st 2011

*/

#include "ShootEditorCommon.h"

#include "StatsViewer.h"

#include "ObjectViewer.h"

#include <wx/notebook.h>
#include <wx/sizer.h>

namespace shoot
{
	// static variables instantiation
	StatsViewer* StatsViewer::ms_pInstance = NULL;
	
	//! Constructor
	StatsViewer::StatsViewer()
		: super(NULL, wxID_ANY, "StatsViewer", wxDefaultPosition, wxSize(1024, 768))
	{
		ms_pInstance = this;

		wxNotebook* pTabControl = new wxNotebook(this, wxID_ANY);
		ObjectViewer* pObjectViewer = new ObjectViewer(pTabControl);
		pTabControl->AddPage(pObjectViewer, "ObjectViewer", true);

		wxBoxSizer *pSizer = new wxBoxSizer( wxVERTICAL );
		pSizer->Add(pTabControl, wxSizerFlags(1).Expand());

		SetSizer(pSizer); // use the sizer for layout

		pSizer->FitInside(this);
	}

	// Destructor
	StatsViewer::~StatsViewer()
	{
		ms_pInstance = NULL;
	}	
}

