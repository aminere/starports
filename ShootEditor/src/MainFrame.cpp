/* 

Amine Rehioui
Created: April 4th 2010

*/

#include "ShootEditorCommon.h"

#include "EditorSettingsDialog.h"
#include "Console.h"
#include "EditorUtils.h"
#include "CameraManager.h"
#include "Entity3DController.h"
#include "EditorRenderer.h"
#include "MenuManager.h"
#include "ToolbarManager.h"
#include "IconManager.h"
#include "UndoManager.h"

#include "MaterialProvider.h"
#include "SkyBoxEntity.h"
#include "ParticleGenerator.h"
#include "Image.h"
#include "Sprite.h"
#include "Text.h"
#include "MeshEntity.h"
#include "File.h"
#include "wxCommonProperties.h" // Vector3ToVariant

namespace shoot
{
	// define the event table
	BEGIN_EVENT_TABLE(MainFrame, wxFrame)
		//EVT_MENU(ID_Tab_New, OnNewTab)
		EVT_MENU(wxID_EXIT, MainFrame::OnQuit)
		EVT_CLOSE(MainFrame::OnClose)
		EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
		EVT_MENU_RANGE(ID_Menu_Insert_FirstEntity, ID_Menu_Insert_LastEntity, MainFrame::OnInsert)
		EVT_MENU(ID_Menu_Duplicate, MainFrame::OnDuplicate)
		EVT_MENU(ID_Menu_Remove, MainFrame::OnRemove)
		EVT_MENU(ID_Edit_Undo, MainFrame::OnUndo)
		EVT_MENU(ID_Edit_Redo, MainFrame::OnRedo)
		EVT_MENU(ID_Engine_Start, MainFrame::OnStartEngine)
		EVT_MENU(ID_Engine_Record, MainFrame::OnRecordEngine)
		EVT_MENU(ID_Engine_Replay, MainFrame::OnReplayEngine)		
		EVT_MENU(ID_Engine_Stop, MainFrame::OnStopEngine)
		EVT_MENU(ID_Engine_Pause, MainFrame::OnPauseEngine)
		EVT_MENU(ID_Menu_ToggleGameCameraControl, MainFrame::OnToggleGameCameraControl)	
		EVT_MENU(ID_Menu_ShowSettings, MainFrame::OnShowSettings)
		EVT_MENU(ID_Menu_ShowConsole, MainFrame::OnShowConsole)
		EVT_MENU_RANGE(ID_Edit_Translate, ID_Edit_Scale, MainFrame::OnChangeEntityControlMode)
		EVT_MENU(ID_Edit_Pin, MainFrame::OnPin)
		EVT_MENU(ID_Menu_ToggleEditorPostRendering, MainFrame::OnToggleEditorPostRendering)
		EVT_MENU(ID_Menu_ToggleAudio, MainFrame::OnToggleAudio)
		EVT_MENU(ID_Menu_Toggle3DControl, MainFrame::OnToggle3DControl)
		EVT_MENU(ID_Menu_ToggleGameMouseControl, MainFrame::OnToggleGameMouseControl)
		EVT_IDLE(OnIdle)
	END_EVENT_TABLE()

	MainFrame::MainFrame(const wxString& title, 
						 const wxSize& size /*= wxSize(800, 600)*/, 
						 ShootEditor* pCustomEditor /*= NULL*/,
						 ToolbarManager* pCustomToolbar /*= NULL*/,
						 IconManager* pCustomIcons /*= NULL*/)
	: wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, size)	
	{
		// tell wxAuiManager to manage this frame
		m_UIManager.SetManagedWindow(this);

		// create singletons
		MenuManager::CreateInstance();
		pCustomIcons ? IconManager::OverrideInstance(pCustomIcons) : IconManager::CreateInstance();
		pCustomToolbar ? ToolbarManager::OverrideInstance(pCustomToolbar) : ToolbarManager::CreateInstance();
		pCustomEditor ? ShootEditor::OverrideInstance(pCustomEditor) : ShootEditor::CreateInstance();
	}

	// Destructor
	MainFrame::~MainFrame()
	{
		// destroy singletons
		MenuManager::DestroyInstance();
		ToolbarManager::DestroyInstance();
		IconManager::DestroyInstance();

		m_UIManager.UnInit();

		ShootEditor::DestroyInstance();
	}

	//! Shows the frame
	void MainFrame::Show()
	{
		ShootEditor::Instance()->Init(this);

		MenuManager::Instance()->Init();
		SetMenuBar(MenuManager::Instance()->GetMainMenuBar());

		ToolbarManager::Instance()->Init();
		m_UIManager.AddPane(ToolbarManager::Instance()->GetMainToolbar(), wxAuiPaneInfo().Top().LeftDockable(false).RightDockable(false).CaptionVisible(false));

		m_UIManager.Update();

		Maximize();			
		
		super::Show(true);	

		// create the console window
		Console* pConsole = new Console();
		pConsole->Show();
		pConsole->Iconize();
	}

	// event handlers
	//void MainFrame::OnNewTab(wxCommandEvent& event)
	//{
	//	ShootEditor::Instance()->GetMainTabControl()->AddNewTab();
	//}

	void MainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
	{
		// true is to force the frame to close
		Close(true);	
	}

	void MainFrame::OnClose(wxCloseEvent& event)
	{		
		if(Console* pConsole = Console::Instance())
			pConsole->Close(true);

		event.Skip();
	}

	void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
	{
		wxMessageBox("Amine Rehioui 2010", "Shoot Editor", wxOK | wxICON_INFORMATION, this);
	}

	void MainFrame::OnInsert(wxCommandEvent& event)
	{
		Engine* pEngine = Engine::Instance();

		int typeIndex = event.GetId() - ID_Menu_Insert_FirstEntity;
		Entity* pEntity = NULL;
		bool bCancelled = false;

		// hack to insert entity templates
		bool bIsTemplateInstance = (typeIndex == ObjectFactory::Instance()->GetNumCreatableTypes()+1);
		if(bIsTemplateInstance)
		{
			std::string path;			
			if(EditorUtils::OpenFileDialog(this, path, "XML files (*.xml)|*.xml"))
			{
				auto object = ObjectManager::Instance()->Load(path.c_str());
				pEntity = DYNAMIC_CAST(object, Entity);
				if (!pEntity)
				{
					wxMessageBox(Utils::StringFormat("Invalid object type. Expected 'Entity', got '%s'", object->GetClassName()), "Invalid Type", wxOK | wxICON_ERROR, this);
					bCancelled = true;
				}						
			}
			else
			{
				bCancelled = true;
			}
		}
		else
		{
			Object* pObject = ObjectFactory::Instance()->Create(typeIndex);
			pEntity = DYNAMIC_CAST(pObject, Entity);
			SHOOT_ASSERT(pEntity, "Cannot create an Entity from Object");			

			// init entities that need resources
			Image* pImage = DYNAMIC_CAST(pEntity, Image);
			Sprite* pSprite = DYNAMIC_CAST(pEntity, Sprite);
			Text* pText = DYNAMIC_CAST(pEntity, Text);
			MeshEntity* pMeshEntity = DYNAMIC_CAST(pEntity, MeshEntity);
			BillBoardEntity* pBillBoard = DYNAMIC_CAST(pEntity, BillBoardEntity);
			ParticleGenerator* pParticleGenerator = DYNAMIC_CAST(pEntity, ParticleGenerator);
			SkyBoxEntity* pSkyBox = DYNAMIC_CAST(pEntity, SkyBoxEntity);
			Camera* pCamera = DYNAMIC_CAST(pEntity, Camera);

			if(pSprite != NULL)
			{
				std::string path;			
				if(EditorUtils::OpenFileDialog(this, path, "XML files (*.xml)|*.xml"))
				{
					pSprite->SetPath(path);				
				}
				else
				{
					bCancelled = true;
				}
			}
			else if(pImage != NULL)
			{
				std::string path;
				if (EditorUtils::OpenFileDialog(this, path, "Textures (*.png)|*.png")) // TODO: set file filter
				{
					path = Utils::GetFilePathNoExt(path) + ".xml";
					if (!File::Exists(path.c_str()))
					{
						Reference<Texture> textureInfo = snew Texture();
						textureInfo->SaveToXML(path.c_str());
					}

					Material::CreationInfo materialInfo;
					materialInfo.AddTexture(path);
					Material* pMaterial = MaterialProvider::Instance()->GetMaterial(materialInfo);
					GraphicComponent* pGraphic = snew GraphicComponent();
					pGraphic->SetMaterial(pMaterial);
					pGraphic->SetRenderingPass(GraphicComponent::RP_2D);
					pImage->AddComponent(pGraphic, true);
				}
				else
				{
					bCancelled = true;
				}
			}	 
			else if(pText != NULL)
			{
				std::string path;
				if(EditorUtils::OpenFileDialog(this, path, "TTF files (*.ttf)|*.ttf"))
				{
					pText->SetFont(path);
				}
				else
				{
					bCancelled = true;
				}
			}
			else if(pMeshEntity != NULL)
			{
				std::string path;			
				if(EditorUtils::OpenFileDialog(this, path, "XML files (*.xml)|*.xml"))
				{
					pMeshEntity->SetMesh(path);
				}
				else
				{
					bCancelled = true;
				}
			}
			else if(pBillBoard != NULL)
			{
				std::string path;			
				if(EditorUtils::OpenFileDialog(this, path)) // TODO: set file filter
				{
					Material::CreationInfo materialInfo;
					materialInfo.AddTexture(path);
					Material* pMaterial = MaterialProvider::Instance()->GetMaterial(materialInfo);
					GraphicComponent* pGraphic = snew GraphicComponent();
					pGraphic->SetMaterial(pMaterial);
					pGraphic->SetRenderingPass(GraphicComponent::RP_3DTransparent);
					pBillBoard->AddComponent(pGraphic, true);
				}
				else
				{
					bCancelled = true;
				}
			}
			else if(pParticleGenerator != NULL)
			{
				std::string path;
				if (EditorUtils::OpenFileDialog(this, path, "Textures (*.png)|*.png")) // TODO: set file filter
				{
					path = Utils::GetFilePathNoExt(path) + ".xml";
					Material::CreationInfo materialInfo;
					materialInfo.AddTexture(path);
					materialInfo.SetShader("common/shaders/tex_col.xml");
					Material* pMaterial = MaterialProvider::Instance()->GetMaterial(materialInfo);
					GraphicComponent* pGraphic = snew GraphicComponent();
					pGraphic->SetMaterial(pMaterial);
					pGraphic->SetRenderingPass(GraphicComponent::RP_3DParticles);
					pParticleGenerator->AddComponent(pGraphic, true);
				}
				else
				{
					bCancelled = true;
				}
			}
			else if(pSkyBox != NULL)
			{
				std::string path;			
				if(EditorUtils::OpenFileDialog(this, path, "XML files (*.xml)|*.xml"))
				{
					pSkyBox->SetCubeMap(path);
				}
				else
				{
					bCancelled = true;
				}
			}
			else if(pCamera != NULL)
			{
				pCamera->SetName(Camera::GetStaticClassName());
			}
		}

		if(!bCancelled)
		{
			Entity* pParentEntity = NULL;
			bool bIsContext = pEntity->IsA(Context::TypeID);

			// determine parent entity
			if(!bIsContext)
			{
				if(pEngine->GetNumContexts() == 0)
				{
					// if context stack is empty, create one context to contain the entity
					pParentEntity = new Context();
					pParentEntity->SetSavableToXML(true);
					pParentEntity->Init();
					pEngine->GetContextStack()->AddChild(pParentEntity, true);

					// update tree view
					ShootEditor::Instance()->GetTreeView()->InsertEntity(pParentEntity);
				}
				else
				{
					Entity* pSelectedEntity = ShootEditor::Instance()->GetSelectedEntity();
					// if no parent entity is selected then insert into the first root context in the stack
					pParentEntity = pSelectedEntity ? pSelectedEntity : pEngine->GetContext(0);	
				}
			}
			
			// make sure entities are not directly inserted into the context stack but rather in a context
			if(pParentEntity == pEngine->GetContextStack())
			{
				pParentEntity = pEngine->GetContext(0);
			}
			// TODO: more Parent/Child type validation

			if(pParentEntity)
			{
				pParentEntity->AddChild(pEntity, true);
			}
			else
			{
				// inserting a context into the engine stack
				SHOOT_ASSERT(bIsContext, "No context is available to contain the inserted entity");
				pEngine->GetContextStack()->AddChild(pEntity, true);
			}

			// Initialize the entity
			pEntity->SetSavableToXML(true);
			pEntity->Init();

			ShootEditor::Instance()->GetTreeView()->InsertEntity(pEntity);
			ShootEditor::Instance()->UpdateCameraSelectors();

			if(ShootEditor::Instance()->HasPin())
			{
				if (Entity3D* pEntity3D = DYNAMIC_CAST(pEntity, Entity3D))
				{
					pEntity3D->SetAbsolutePosition(ShootEditor::Instance()->GetPinPosition());

					// mark position as overriden
					PropertyStream stream(SM_Write);
					pEntity->Serialize(stream);
					pEntity->GetOverridenProperties().AddProperty(stream.GetProperty("Position")->Copy());
				}
			}

			// make sure it's added to the quadtree
			if(QuadTree* pQuadTree = pEntity->GetRootContext()->GetQuadTree())
			{
				pQuadTree->ExtractEntities(pEntity);
				pQuadTree->Rebuild();
			}
		}
		else
		{
			// cancelled
			sdelete(pEntity);
		}
	}

	void MainFrame::OnRemove(wxCommandEvent& event)
	{
		Entity* pEntity = ShootEditor::Instance()->GetSelectedEntity();
		if(pEntity)
		{
			if(pEntity->GetParent())
			{
				// cannot delete current camera (TODO find a better fix)
				if(pEntity->IsA(Camera::TypeID)
				&&  ((EntityRenderer::Instance()->Get3DCamera() == pEntity) 
				||   (EntityRenderer::Instance()->Get2DCamera() == pEntity)))
				{
					wxMessageBox("Cannot remove a camera currently in use", "Cannot remove", wxOK | wxICON_ERROR, this);					
				}
				else
				{
					if( wxMessageBox("Do you really want to remove the selected entity?",
									 "Comfirm removal",
									 wxYES_NO | wxICON_QUESTION) == wxYES )
					{
						ShootEditor::Instance()->GetTreeView()->RemoveEntity(pEntity);						
						UndoManager::Instance()->ClearHistory(pEntity);						

						pEntity->GetParent()->RemoveChild(pEntity, true);						

						ShootEditor::Instance()->SelectEntity(NULL);
						ShootEditor::Instance()->UpdateCameraSelectors();
					}
				}
			}
			else
			{
				Log << "Cannot remove a root entity\n";
			}
		}
		else
		{
			Log << "No entity selected\n";
		}
	}

	void MainFrame::OnDuplicate(wxCommandEvent& event)
	{
		Entity* pEntity = ShootEditor::Instance()->GetSelectedEntity();
		if(!pEntity)
		{
			Log << "Select an entity to duplicate\n";
		}
		else if(!pEntity->GetParent())
		{
			Log << "Cannot duplicate root item\n";
		}
		else if(pEntity->IsA(SubMesh::TypeID))
		{
			Log << "Cannot duplicate sub-meshes\n";
		}
		else
		{
			Object* pBaseCopy = pEntity->Copy();
			Entity* pCopy = DYNAMIC_CAST(pBaseCopy, Entity);
			SHOOT_ASSERT(pCopy, "Cloned entity is not of same type as source entity.. WTF?");

			size_t indexInParent = 0;
			for (size_t i = 0; i<pEntity->GetParent()->GetChildCount(); ++i)
			{
				if(pEntity->GetParent()->GetChild(i) == pEntity)
				{
					indexInParent = i;
					break;
				}
			}

			if(indexInParent+1 < pEntity->GetParent()->GetChildCount())
			{
				pEntity->GetParent()->InsertChild(pCopy, indexInParent+1, true);			
				ShootEditor::Instance()->GetTreeView()->InsertEntity(pCopy, indexInParent+1);
			}
			else
			{
				pEntity->GetParent()->AddChild(pCopy, true);
				ShootEditor::Instance()->GetTreeView()->InsertEntity(pCopy);
			}

			pCopy->Init();
			ShootEditor::Instance()->UpdateCameraSelectors();
			ShootEditor::Instance()->SelectEntity(pCopy);

			// make sure it's added to the quadtree
			if(QuadTree* pQuadTree = pCopy->GetRootContext()->GetQuadTree())
			{
				pQuadTree->ExtractEntities(pCopy);
				pQuadTree->Rebuild();
			}
		}
	}

	void MainFrame::OnUndo(wxCommandEvent& event)
	{
		UndoManager::Instance()->Undo();
	}

	void MainFrame::OnRedo(wxCommandEvent& event)
	{
		UndoManager::Instance()->Redo();
	}

	void MainFrame::OnStartEngine(wxCommandEvent& event)
	{
		ShootEditor::Instance()->StartEngine();
	}

	void MainFrame::OnRecordEngine(wxCommandEvent& event)
	{
		ShootEditor::Instance()->RecordEngine();
	}

	void MainFrame::OnReplayEngine(wxCommandEvent& event)
	{
		ShootEditor::Instance()->ReplayEngine();
	}

	void MainFrame::OnStopEngine(wxCommandEvent& event)
	{
		ShootEditor::Instance()->StopEngine();
	}

	void MainFrame::OnPauseEngine(wxCommandEvent& event)
	{
		ShootEditor::Instance()->PauseEngine();
	}

	void MainFrame::OnToggleGameCameraControl(wxCommandEvent& event)
	{
		CameraManager::Instance()->SetGameCameraControl(!CameraManager::Instance()->GetGameCameraControl());
	}

	void MainFrame::OnShowSettings(wxCommandEvent& event)
	{
		EditorSettingsDialog settingsDialog(this);
		settingsDialog.ShowModal();
	}

	void MainFrame::OnShowConsole(wxCommandEvent& event)
	{
		Console* console = Console::Instance();
		if(!console)
		{
			console = new Console();
			console->Show();
		}			
		console->Iconize(false);
		console->Raise();		
	}

	void MainFrame::OnChangeEntityControlMode(wxCommandEvent& event)
	{
		ShootEditor::E_ControlMode mode = (ShootEditor::E_ControlMode)(event.GetId() - ID_Edit_Translate);
		ShootEditor::Instance()->SetControlMode(mode);
	}

	void MainFrame::OnPin(wxCommandEvent& event)
	{
		if(ShootEditor::Instance()->HasPin())
		{
			if(Entity3D* pEntity3D = DYNAMIC_CAST(ShootEditor::Instance()->GetSelectedEntity(), Entity3D))
			{
				UndoManager::Instance()->RecordTransformState(pEntity3D);
				pEntity3D->SetAbsolutePosition(ShootEditor::Instance()->GetPinPosition());
				ShootEditor::Instance()->GetObjectInspector()->UpdateProperty("Position", WXVARIANT(pEntity3D->GetPosition())); 
			}
			else
			{
				ShootEditor::Instance()->SetHasPin(false);
			}
		}
	}

	void MainFrame::OnIdle(wxIdleEvent& event)	
	{
		ShootEditor::Instance()->Update();
	}

	void MainFrame::OnToggleEditorPostRendering(wxCommandEvent& event)
	{
		EditorRenderer::Instance()->SetEnabled(event.IsChecked());
	}

	void MainFrame::OnToggleAudio(wxCommandEvent& event)
	{
		ShootEditor::Instance()->SetAudioEnabled(!ShootEditor::Instance()->IsAudioEnabled());
	}

	void MainFrame::OnToggle3DControl(wxCommandEvent& event)
	{
		ShootEditor::Instance()->Set3DControl(!ShootEditor::Instance()->Get3DControl());
	}

	void MainFrame::OnToggleGameMouseControl(wxCommandEvent& event)
	{
		ShootEditor::Instance()->SetGameMouseControl(!ShootEditor::Instance()->HasGameMouseControl());
	}
}


