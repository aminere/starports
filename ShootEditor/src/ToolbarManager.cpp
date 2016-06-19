/* 

Amine Rehioui
Created: August 22nd 2011

*/

#include "ShootEditorCommon.h"

#include "ToolbarManager.h"

#include <wx/artprov.h> // wxArtProvider

#include "IconManager.h"
#include "MenuManager.h"

namespace shoot
{
	//! constructor
	ToolbarManager::ToolbarManager()
	{	
	}

	//! destructor
	ToolbarManager::~ToolbarManager()
	{
	}
		
	//! Initializes the tool bar manager
	void ToolbarManager::Init()
	{
		wxFrame* pFrame = static_cast<wxFrame*>(wxTheApp->GetTopWindow());
		IconManager* pIconMgr = IconManager::Instance();
		MenuManager* pMenuMgr = MenuManager::Instance();

		// create the tool bar
		//m_pMainToolbar = new wxToolBar(pFrame, wxID_ANY);
		wxSize tbSize(24, 22);
		m_pMainToolbar = new wxAuiToolBar(pFrame, wxID_ANY);		
		m_pMainToolbar->SetToolBitmapSize(tbSize);
		//m_pMainToolbar->AddTool(ID_Tab_New, wxT("New Tab"), wxArtProvider::GetBitmap(wxART_NEW));		
		//m_pMainToolbar->AddSeparator();
		m_pMainToolbar->AddTool(ID_Menu_Duplicate, wxT("Duplicate Entity"), wxArtProvider::GetBitmap(wxART_COPY, wxART_OTHER, tbSize), wxT("Duplicate Entity"));
		m_pMainToolbar->AddTool(ID_Menu_Remove, wxT("Delete Entity"), wxArtProvider::GetBitmap(wxART_DELETE, wxART_OTHER, tbSize), wxT("Delete Entity"));
		m_pMainToolbar->AddSeparator();
		m_pMainToolbar->AddTool(ID_Edit_Translate, wxT("Translate Entity"), pIconMgr->GetBitmap(IconManager::Icon_Translate), wxT("Translate Entity"), wxITEM_RADIO);
		m_pMainToolbar->AddTool(ID_Edit_Rotate, wxT("Rotate Entity"), pIconMgr->GetBitmap(IconManager::Icon_Rotate), wxT("Rotate Entity"), wxITEM_RADIO);
		m_pMainToolbar->AddTool(ID_Edit_Scale, wxT("Scale Entity"), pIconMgr->GetBitmap(IconManager::Icon_Scale), wxT("Scale Entity"), wxITEM_RADIO);
		m_pMainToolbar->AddTool(ID_Edit_Pin, wxT("Pin"), pIconMgr->GetBitmap(IconManager::Icon_Pin), wxT("Move to Pin / Toggle Pin"));
		m_pMainToolbar->AddSeparator();
		m_pMainToolbar->AddTool(ID_Edit_Undo, wxT("Undo"), pIconMgr->GetBitmap(IconManager::Icon_Undo), wxT("Undo"));
		m_pMainToolbar->AddTool(ID_Edit_Redo, wxT("Redo"), pIconMgr->GetBitmap(IconManager::Icon_Redo), wxT("Redo"));
		m_pMainToolbar->AddSeparator();
		m_pMainToolbar->AddTool(ID_Engine_Start, wxT("Start Engine"), pIconMgr->GetBitmap(IconManager::Icon_Play), wxT("Start Engine"), wxITEM_RADIO);
		m_pMainToolbar->AddTool(ID_Engine_Stop, wxT("Stop Engine"), pIconMgr->GetBitmap(IconManager::Icon_Stop), wxT("Stop Engine"), wxITEM_RADIO);
		m_pMainToolbar->AddTool(ID_Engine_Pause, wxT("Pause Engine"), pIconMgr->GetBitmap(IconManager::Icon_Pause), wxT("Pause Engine"), wxITEM_RADIO);
		m_pMainToolbar->AddTool(ID_Engine_Record, wxT("Record Engine"), pIconMgr->GetBitmap(IconManager::Icon_Record), wxT("Record Engine"), wxITEM_RADIO);
		m_pMainToolbar->AddTool(ID_Engine_Replay, wxT("Replay Engine"), pIconMgr->GetBitmap(IconManager::Icon_Replay), wxT("Replay Engine"), wxITEM_RADIO);
		m_pMainToolbar->AddSeparator();
		m_pMainToolbar->AddTool(ID_Menu_Insert_FirstEntity+pMenuMgr->GetTypeIndex("EntityTemplate"), wxT("Insert Template"), pIconMgr->GetBitmap("EntityTemplate"), wxT("Insert Template"));
		m_pMainToolbar->AddTool(ID_Menu_Insert_FirstEntity+pMenuMgr->GetTypeIndex("MeshEntity"), wxT("Insert MeshEntity"), pIconMgr->GetBitmap("MeshEntity"), wxT("Insert MeshEntity"));		
		m_pMainToolbar->AddSeparator();
		m_pMainToolbar->AddTool(ID_Menu_ToggleEditorPostRendering, wxT("Editor Post Rendering"), pIconMgr->GetBitmap(IconManager::Icon_PostRendering), wxT("Editor Post Rendering"), wxITEM_CHECK);
		m_pMainToolbar->AddTool(ID_Menu_ToggleAudio, wxT("Audio"), pIconMgr->GetBitmap(IconManager::Icon_Audio), wxT("Toggle Audio"), wxITEM_CHECK);
		m_pMainToolbar->AddTool(ID_Menu_Toggle3DControl, wxT("3D Control"), pIconMgr->GetBitmap(IconManager::Icon_3D), wxT("3D Control"), wxITEM_CHECK);
		m_pMainToolbar->AddTool(ID_Menu_ToggleGameMouseControl, wxT("Game Mouse Control"), pIconMgr->GetBitmap(IconManager::Icon_GameMouse), wxT("Game Mouse Control"), wxITEM_CHECK);

		m_pMainToolbar->ToggleTool(ID_Menu_ToggleEditorPostRendering, true);
		m_pMainToolbar->ToggleTool(ID_Menu_ToggleAudio, true);
		m_pMainToolbar->ToggleTool(ID_Menu_Toggle3DControl, true);
		m_pMainToolbar->ToggleTool(ID_Menu_ToggleGameMouseControl, true);
		m_pMainToolbar->ToggleTool(ID_Edit_Translate+ShootEditor::Instance()->GetControlMode(), true);
		m_pMainToolbar->EnableTool(ID_Edit_Pin, false);
		m_pMainToolbar->Realize();

		m_pMainToolbar->ToggleTool(ID_Engine_Stop, true);
		static_cast<wxFrame*>(wxTheApp->GetTopWindow())->SetStatusText("Engine Stopped", 2);

		SetUndoEnabled(false);
		SetRedoEnabled(false);
	}

	void ToolbarManager::SetUndoEnabled(bool enable)
	{
		m_pMainToolbar->EnableTool(ID_Edit_Undo, enable);
		m_pMainToolbar->Refresh();
	}

	void ToolbarManager::SetRedoEnabled(bool enable)
	{
		m_pMainToolbar->EnableTool(ID_Edit_Redo, enable);
		m_pMainToolbar->Refresh();
	}
}

