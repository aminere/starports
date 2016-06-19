/* 

Amine Rehioui
Created: March 13th 2011

*/

#include "ShootEditorCommon.h"

#include "TabContainer.h"
#include "IconManager.h"

#include "ToolBarManager.h"
#include "EditorUtils.h"

#include "EventManager.h"
#include "EditorEvents.h"
#include "UndoManager.h"
#include "EditorSettings.h"

#include "wx/sizer.h"
#include <wx/artprov.h> // wxArtProvider
#include <wx/bmpbuttn.h>

namespace shoot
{
	// Define event table
	BEGIN_EVENT_TABLE(TabContainer, wxPanel)
		EVT_COMMAND(ID_Menu_New, wxEVT_COMMAND_BUTTON_CLICKED, OnNew)
		EVT_COMMAND(ID_Menu_Open, wxEVT_COMMAND_BUTTON_CLICKED, OnOpen)
		EVT_COMMAND(ID_Menu_OpenPrevious, wxEVT_COMMAND_BUTTON_CLICKED, OnOpenPrevious)
		EVT_COMMAND(ID_Menu_Save, wxEVT_COMMAND_BUTTON_CLICKED, OnSave)
		EVT_COMMAND(ID_Menu_SaveAs, wxEVT_COMMAND_BUTTON_CLICKED, OnSaveAs)
		EVT_COMMAND(ID_Menu_ReloadLastSave, wxEVT_COMMAND_BUTTON_CLICKED, OnReloadLastSave)
		EVT_COMBOBOX(ID_3DCameraSelector, On3DCameraChanged)
		EVT_COMBOBOX(ID_2DCameraSelector, On2DCameraChanged)
		EVT_COMMAND(ID_Tab_ResetView, wxEVT_COMMAND_BUTTON_CLICKED, OnResetView)
		EVT_COMMAND(ID_Tab_ClearFade, wxEVT_COMMAND_BUTTON_CLICKED, OnClearFade)
		EVT_COMMAND(ID_Tab_ToggleBlackBars, wxEVT_COMMAND_BUTTON_CLICKED, OnToggleBlackBars)		
	END_EVENT_TABLE()

	//! Constructor
	TabContainer::TabContainer(wxWindow* pParent,
							   wxWindowID id /*= wxID_ANY*/, 
							   const wxPoint& pos /*= wxDefaultPosition*/, 
							   const wxSize& size /*= wxSize(800, 600)*/, 
							   long style /*= 0*/) 
	: super(pParent, id, pos, size, style)	
	{
		wxSize iconSize = wxSize(24, 22);

		wxBoxSizer *pMenuSizer = new wxBoxSizer( wxHORIZONTAL );

		wxBoxSizer *pToolBarSizer = new wxBoxSizer( wxHORIZONTAL );
		pToolBarSizer->Add(new wxBitmapButton(this, ID_Menu_New, wxArtProvider::GetBitmap(wxART_NEW, wxART_OTHER, iconSize), wxDefaultPosition, iconSize));
		pToolBarSizer->Add(new wxBitmapButton(this, ID_Menu_Open, wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_OTHER, iconSize), wxDefaultPosition, iconSize));
		pToolBarSizer->Add(new wxBitmapButton(this, ID_Menu_OpenPrevious, wxArtProvider::GetBitmap(wxART_GO_BACK, wxART_OTHER, iconSize), wxDefaultPosition, iconSize));
		pToolBarSizer->Add(new wxBitmapButton(this, ID_Menu_Save, wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_OTHER, iconSize), wxDefaultPosition, iconSize));
		pToolBarSizer->Add(new wxBitmapButton(this, ID_Menu_SaveAs, wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS, wxART_OTHER, iconSize), wxDefaultPosition, iconSize));		
		pToolBarSizer->Add(new wxButton(this, ID_Menu_ReloadLastSave, "R", wxDefaultPosition, iconSize), 0, wxALIGN_CENTRE_VERTICAL);
		pToolBarSizer->AddSpacer(8);

		// create the camera selectors
		wxString cameras[] = { "Editor" };
		E_EventID aCameraSelectorIDs[] = { ID_3DCameraSelector, ID_2DCameraSelector };
		const char* aCameraSeletorCaptions[] = { "3D Camera ", "2D Camera " };
		for(int i=CS_3D; i<CS_Count; ++i)
		{
			m_pCameraSelectors[i] = new wxComboBox(this, aCameraSelectorIDs[i], "Camera", wxDefaultPosition, wxSize(80, 22), 1, cameras, wxCB_DROPDOWN | wxCB_READONLY);
			m_pCameraSelectors[i]->SetValue(cameras[0]);
			pToolBarSizer->Add(new wxStaticText(this, wxID_ANY, aCameraSeletorCaptions[i]), 0, wxALIGN_CENTRE_VERTICAL);			
			pToolBarSizer->Add(m_pCameraSelectors[i]);
			pToolBarSizer->AddSpacer(8);
		}

		pToolBarSizer->Add(new wxButton(this, ID_Tab_ResetView, "RV", wxDefaultPosition, iconSize), 0, wxALIGN_CENTRE_VERTICAL);
		pToolBarSizer->Add(new wxButton(this, ID_Tab_ClearFade, "CF", wxDefaultPosition, iconSize), 0, wxALIGN_CENTRE_VERTICAL);
		pToolBarSizer->Add(new wxButton(this, ID_Tab_ToggleBlackBars, "TB", wxDefaultPosition, iconSize), 0, wxALIGN_CENTRE_VERTICAL);		
		
		m_pViewPortContainer = new ViewPortContainer(this);

		wxBoxSizer *pSizer = new wxBoxSizer( wxVERTICAL );
		pSizer->Add(pMenuSizer);
		pSizer->Add(pToolBarSizer);		
		pSizer->Add(m_pViewPortContainer, wxSizerFlags(1).Expand());		

		SetSizer(pSizer);
		pSizer->FitInside(this);
	}

	//! Destructor
	TabContainer::~TabContainer()
	{
	}

	void TabContainer::OnNew(wxCommandEvent& event)
	{
		Engine::Instance()->ClearContextStack();

		m_CurrentProjectPath = "";
		ShootEditor::Instance()->SelectEntity(NULL);

		ShootEditor::Instance()->GetTreeView()->DeleteAllItems();

		// clear undo history
		UndoManager::Instance()->ClearHistory();		

		UpdateCameraSelectors();

		SetTabName("New");
	}

	void TabContainer::OnOpen(wxCommandEvent& event)
	{		
		if(EditorUtils::OpenFileDialog(this, m_CurrentProjectPath))		
			Load(m_CurrentProjectPath);
	}

	void TabContainer::OnOpenPrevious(wxCommandEvent& event)
	{
		if(!EditorSettings::Instance()->LastLoadedProject.GetString().empty())
			Load(EditorSettings::Instance()->LastLoadedProject);
	}

	void TabContainer::OnSave(wxCommandEvent& event)
	{
		if(ShootEditor::Instance()->IsEngineStarted())
		{
			if(wxMessageBox("The engine was started. Are you sure you want to save?", "Comfirm save", wxYES_NO | wxICON_QUESTION) == wxNO)
				return;
		}
		
		if(m_CurrentProjectPath == "" && !EditorUtils::OpenFileDialog(this, m_CurrentProjectPath, "XML Files (*.xml)|*.xml", "Save File", wxFD_SAVE))
			return;

		EditorUtils::FormatObjectPath(Engine::Instance()->GetContextStack(), m_CurrentProjectPath);
		Engine::Instance()->GetContextStack()->SaveToXML(m_CurrentProjectPath.c_str());
	}

	void TabContainer::OnSaveAs(wxCommandEvent& event)
	{
		std::string path;
		if(EditorUtils::OpenFileDialog(this, path, "XML Files (*.xml)|*.xml", "Save File As", wxFD_SAVE))
		{
			EditorUtils::FormatObjectPath(Engine::Instance()->GetContextStack(), path);
			Engine::Instance()->GetContextStack()->SaveToXML(path.c_str());
			SetProjectPath(path);
		}
	}
	
	void TabContainer::OnReloadLastSave(wxCommandEvent& event)
	{
		if(!m_CurrentProjectPath.empty())
			Reload();
		else
			wxMessageBox("No save available, cannot reload level", "No save available", wxOK | wxICON_INFORMATION, this);
	}

	void TabContainer::On3DCameraChanged(wxCommandEvent& event)
	{
		std::string strCameraName(m_pCameraSelectors[CS_3D]->GetStringSelection().c_str());		
		bool bGameCamera = (m_pCameraSelectors[CS_3D]->GetSelection() != 0);
		if(bGameCamera)
		{
			Camera* pCamera = DYNAMIC_CAST(Engine::Instance()->GetContextStack()->GetChildByName(strCameraName), Camera);
			SHOOT_ASSERT(pCamera, "Camera not found or not of type Camera");
			EntityRenderer::Instance()->Set3DCamera(pCamera);
		}
		else
		{
			if(ViewPort* pViewPort = ShootEditor::Instance()->GetViewPort())
				EntityRenderer::Instance()->Set3DCamera(pViewPort->Get3DCamera());
		}
	}

	void TabContainer::On2DCameraChanged(wxCommandEvent& event)
	{
		std::string strCameraName(m_pCameraSelectors[CS_2D]->GetStringSelection().c_str());
		bool bGameCamera = (m_pCameraSelectors[CS_2D]->GetSelection() != 0);		
		if(bGameCamera)
		{
			Camera* pCamera = DYNAMIC_CAST(Engine::Instance()->GetContextStack()->GetChildByName(strCameraName), Camera);
			SHOOT_ASSERT(pCamera, "Camera not found or not of type Camera");
			EntityRenderer::Instance()->Set2DCamera(pCamera);
		}
		else
		{
			if(ViewPort* pViewPort = ShootEditor::Instance()->GetViewPort())
				EntityRenderer::Instance()->Set2DCamera(pViewPort->Get2DCamera());
		}
	}

	void TabContainer::OnResetView(wxCommandEvent& event)
	{
		m_pViewPortContainer->GetViewPort()->ResetView();
		EntityRenderer::Instance()->Set3DCamera(m_pViewPortContainer->GetViewPort()->Get3DCamera());
		EntityRenderer::Instance()->Set2DCamera(m_pViewPortContainer->GetViewPort()->Get2DCamera());
		UpdateCameraSelectors();
	}

	void TabContainer::OnClearFade(wxCommandEvent& event)
	{
		FadeRenderer::Instance()->SetColor(Color::Zero);
	}

	void TabContainer::OnToggleBlackBars(wxCommandEvent& event)
	{
		bool bRenderBlackBars = EntityRenderer::Instance()->GetRenderViewField();
		EntityRenderer::Instance()->SetRenderViewField(!bRenderBlackBars);
	}

	//! called when the tab is about to close, return false to forbid closing
	bool TabContainer::OnClose()
	{		
		ShootEditor::Instance()->SelectEntity(NULL);

		ShootEditor::Instance()->GetTreeView()->DeleteAllItems();

		return true;
	}

	//! updates a camera selector		
	void TabContainer::UpdateCameraSelectors()
	{
		std::vector< Handle<Camera> > aCameras;
		Engine::Instance()->GetContextStack()->GetChildrenByType<Camera>(aCameras);
		for(int SelectorIndex = CS_3D; SelectorIndex < CS_Count; SelectorIndex++)
		{
			m_pCameraSelectors[SelectorIndex]->Clear();
			m_pCameraSelectors[SelectorIndex]->Append("Editor");
			for (size_t i=0; i<aCameras.size(); ++i)
			{
				if(SelectorIndex == CS_3D 
				&& (aCameras[i]->GetType() == Projection::Perspective
				||  aCameras[i]->GetType() == Projection::Ortho3D))
				{
					m_pCameraSelectors[SelectorIndex]->Append(aCameras[i]->GetName());
				}
				else if(SelectorIndex == CS_2D
				&& aCameras[i]->GetType() == Projection::Ortho2D)
				{
					m_pCameraSelectors[SelectorIndex]->Append(aCameras[i]->GetName());
				}
			}

			// set current selection
			Camera* pCamera = NULL;
			if(SelectorIndex == CS_3D)			
				pCamera = EntityRenderer::Instance()->Get3DCamera();
			else
				pCamera = EntityRenderer::Instance()->Get2DCamera();
			
			bool bGameCamera = (pCamera
							 && pCamera != m_pViewPortContainer->GetViewPort()->Get3DCamera()
							 && pCamera != m_pViewPortContainer->GetViewPort()->Get2DCamera());
			if(bGameCamera)
			{
				int index = m_pCameraSelectors[SelectorIndex]->FindString(pCamera->GetName());
				if (index != wxNOT_FOUND)
					m_pCameraSelectors[SelectorIndex]->Select(index);
			}
			else
			{
				m_pCameraSelectors[SelectorIndex]->Select(0);
			}
		}
	}

	//! reloads the current level
	void TabContainer::Reload()
	{
		if(m_CurrentProjectPath.empty())
			return;
		
		if(!ShootEditor::Instance()->IsEngineStarted())
			Load(m_CurrentProjectPath);
		else
			ShootEditor::Instance()->StopEngine();
	}

	//! loads a project
	void TabContainer::Load(const std::string& strPath, bool replay /*= false*/)
	{
		Engine::Instance()->ClearContextStack();
		ShootEditor::Instance()->SelectEntity(NULL);
		ShootEditor::Instance()->GetTreeView()->DeleteAllItems();
		UndoManager::Instance()->ClearHistory();
		EditorSettings::Instance()->LastLoadedProject = strPath;
		EntityRenderer::Instance()->Set3DCamera(NULL);
		EntityRenderer::Instance()->Set2DCamera(NULL);

		if (!replay)
			SetProjectPath(strPath);

		ShootEditor::Instance()->StartBackgroundThread(strPath, replay);
	}

	//! sets tab name
	void TabContainer::SetTabName(const std::string& name)
	{
		MainTabControl* pTabControl = ShootEditor::Instance()->GetMainTabControl();
		pTabControl->SetPageText(pTabControl->GetSelection(), name);
	}

	//! sets the current project path
	void TabContainer::SetProjectPath(const std::string& path)
	{
		// extract filename
		size_t fileStartIndex = path.rfind("/");
		size_t fileEndIndex = path.rfind(".");
		std::string fileName = path.substr(fileStartIndex+1, (fileEndIndex-fileStartIndex-1));
		SetTabName(fileName);

		m_CurrentProjectPath = path;
	}
}

