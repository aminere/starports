/* 

Amine Rehioui
Created: April 4th 2010

*/

#pragma once


namespace shoot
{
	// forwards
	class ShootEditor;
	class ToolbarManager;
	class IconManager;

	//! ShootEditor main frame
	class MainFrame : public wxFrame
	{
		typedef wxFrame super;

	public:

		//! Constructor
		MainFrame(const wxString& title, 
			      const wxSize& size = wxSize(1024, 768), 
				  ShootEditor* pCustomEditor = NULL,
				  ToolbarManager* pCustomToolbar = NULL,
				  IconManager* pCustomIcons = NULL);

		// Destructor
		virtual ~MainFrame();	

		//! Shows the frame
		void Show();

		//! returns the UI Manager
		wxAuiManager& GetUIManager() { return m_UIManager; }

		// event handlers (these functions should _not_ be virtual)
		//void OnNewTab(wxCommandEvent& event);
		void OnQuit(wxCommandEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnAbout(wxCommandEvent& event);
		void OnInsert(wxCommandEvent& event);
		void OnDuplicate(wxCommandEvent& event);
		void OnRemove(wxCommandEvent& event);		
		void OnUndo(wxCommandEvent& event);
		void OnRedo(wxCommandEvent& event);
		void OnStartEngine(wxCommandEvent& event);
		void OnRecordEngine(wxCommandEvent& event);
		void OnReplayEngine(wxCommandEvent& event);
		void OnStopEngine(wxCommandEvent& event);
		void OnPauseEngine(wxCommandEvent& event);
		void OnToggleGameCameraControl(wxCommandEvent& event);
		void OnShowSettings(wxCommandEvent& event);
		void OnShowConsole(wxCommandEvent& event);
		void OnChangeEntityControlMode(wxCommandEvent& event);
		void OnPin(wxCommandEvent& event);
		void OnToggleEditorPostRendering(wxCommandEvent& event);
		void OnToggleAudio(wxCommandEvent& event);
		void OnToggle3DControl(wxCommandEvent& event);
		void OnIdle(wxIdleEvent& event);
		void OnToggleGameMouseControl(wxCommandEvent& event);

	private:
		
		wxAuiManager m_UIManager;

		DECLARE_EVENT_TABLE()
	};
}



