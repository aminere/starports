/* 

Amine Rehioui
Created: March 13th 2011

*/

#pragma once


#include <wx/aui/auibook.h>

namespace shoot
{
	class DependencyViewer;
	class ObjectViewer;

	//! Main Tab Control
	class MainTabControl : public wxAuiNotebook							   
	{
		typedef wxAuiNotebook super;

	public:
		
		//! Constructor
		MainTabControl(wxWindow* pParent,
					   long style = wxAUI_NB_DEFAULT_STYLE,
					   wxWindowID id = wxID_ANY, 
					   const wxPoint& pos = wxDefaultPosition, 
					   const wxSize& size = wxSize(800, 600));
	
		//! Initializes the control
		void Init();

		//! Destructor
		virtual ~MainTabControl()
		{
		}		

		//! SelectObject
		void SelectObject(Object* object);

		//! GetDependencyViewer
		inline DependencyViewer* GetDependencyViewer() { return m_pDependencyViewer; }

	private:

		ObjectViewer* m_pObjectViewer;
		DependencyViewer* m_pDependencyViewer;

		// event handlers
		void OnTabChanging(wxAuiNotebookEvent& evt);
		void OnTabChanged(wxAuiNotebookEvent& evt);
		void OnTabClose(wxAuiNotebookEvent& evt);

		DECLARE_EVENT_TABLE();
	};
}



