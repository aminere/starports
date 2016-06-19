/* 

Amine Rehioui
Created: March 13th 2011

*/

#include "ShootEditorCommon.h"

#include "MainTabControl.h"

#include "TabContainer.h"

#include "ObjectViewer.h"
#include "DependencyViewer.h"

#include <wx/splitter.h>

namespace shoot
{
	// Define event table
	BEGIN_EVENT_TABLE(MainTabControl, wxAuiNotebook)
		EVT_AUINOTEBOOK_PAGE_CHANGING(wxID_ANY, OnTabChanging)
		EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY, OnTabChanged)
		EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, OnTabClose)
	END_EVENT_TABLE()

	//! Constructor
	MainTabControl::MainTabControl(wxWindow* pParent,
								   long style /*= wxAUI_NB_DEFAULT_STYLE*/,
								   wxWindowID id /*= wxID_ANY*/, 
								   const wxPoint& pos /*= wxDefaultPosition*/, 
								   const wxSize& size /*= wxSize(800, 600)*/) 
	: super(pParent, id, pos, size, style)
	, m_pObjectViewer(NULL)
	, m_pDependencyViewer(NULL)
	{
	}

	//! Initializes the control
	void MainTabControl::Init()
	{
		// create a default tab		
		TabContainer* pTabContainer = new TabContainer(this);
		AddPage(pTabContainer, "New", true);
		
		auto splitter = new wxSplitterWindow(this);
		m_pObjectViewer = new ObjectViewer(splitter);
		m_pDependencyViewer = new DependencyViewer(splitter);
		splitter->SplitVertically(m_pObjectViewer, m_pDependencyViewer);
		splitter->SetSashGravity(0.72);
		AddPage(splitter, "Objects", false);
	}

	//! SelectObject
	void MainTabControl::SelectObject(Object* object)
	{
		m_pObjectViewer->SelectObject(object);
		m_pDependencyViewer->Fill(object);
	}

	// event handlers
	void MainTabControl::OnTabChanging(wxAuiNotebookEvent& evt)
	{
		evt.Skip();
	}

	void MainTabControl::OnTabChanged(wxAuiNotebookEvent& evt)
	{
		int tabIndex = GetSelection();
		auto page = GetPage(tabIndex);
		switch (tabIndex)
		{
		case 1:
			m_pObjectViewer->OnSelected();
		break;
		}
	}

	void MainTabControl::OnTabClose(wxAuiNotebookEvent& evt)
	{		
		evt.Veto();		
	}
}
