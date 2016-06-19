/* 

Amine Rehioui
Created: December 24th 2012

*/

#include "ShootEditorCommon.h"

#include "ExplosionEditor.h"
#include "EditorUtils.h"
#include "ExplosionVisitor.h"

#include "wxCommonProperties.h"
#include "wxArrayProperty.h"
#include "wxStructProperty.h"
#include "PropertyLocator.h"
#include "wxReferenceProperty.h"
#include "MaterialProvider.h"

#include <wx/artprov.h> // wxArtProvider

using namespace shoot;

// define the event table
BEGIN_EVENT_TABLE(ExplosionEditor, wxFrame)
	EVT_MENU(wxID_EXIT, ExplosionEditor::OnQuit)
	EVT_CLOSE(ExplosionEditor::OnClose)
	EVT_MENU(wxID_ABOUT, ExplosionEditor::OnAbout)
	EVT_IDLE(ExplosionEditor::OnIdle)
	EVT_MENU(wxID_NEW, OnNew)
	EVT_MENU(wxID_OPEN, OnOpen)
	EVT_MENU(wxID_SAVE, OnSave)
	EVT_MENU(wxID_UNDO, OnResetView)
END_EVENT_TABLE()

ExplosionEditor::ExplosionEditor(const wxString& title, const wxSize& size /*= wxSize(800, 600)*/)
: wxFrame(NULL, wxID_ANY, title, wxPoint(100, 100), size)
, m_pToolBar(NULL)
, m_pViewPort(NULL)
, m_SelectedChunk(-1)
, m_SelectedSubMesh(-1)
, m_bChunksChanged(false)
, m_MaxDebugVertices(3)
{
	// tell wxAuiManager to manage this frame
	m_UIManager.SetManagedWindow(this);
}

// Destructor
ExplosionEditor::~ExplosionEditor()
{
	m_UIManager.UnInit();
}

//! Shows the frame
void ExplosionEditor::Show()
{
	// create toolbar
	m_pToolBar = new wxAuiToolBar(this, wxID_ANY);
	wxSize iconSize(24, 22);
	m_pToolBar->SetToolBitmapSize(iconSize);
	m_pToolBar->AddTool(wxID_NEW, wxT("New mesh"), wxArtProvider::GetBitmap(wxART_NEW));	
	m_pToolBar->AddTool(wxID_OPEN, wxT("Open mesh"), wxArtProvider::GetBitmap(wxART_FILE_OPEN));
	m_pToolBar->AddTool(wxID_SAVE, wxT("Save explosion info"), wxArtProvider::GetBitmap(wxART_FILE_SAVE));
	m_pToolBar->AddTool(wxID_UNDO, wxT("Reset view"), wxArtProvider::GetBitmap(wxART_UNDO));
	m_pToolBar->Realize();

	m_pViewPort = new EEViewPort(this);
	m_pPropertyList = new PropertyList(this, this);

	m_UIManager.AddPane(m_pToolBar, wxAuiPaneInfo().Name(wxT("Toolbar")).Top().LeftDockable(false).RightDockable(false).CaptionVisible(false));
	m_UIManager.AddPane(m_pViewPort, wxAuiPaneInfo().Center().MaximizeButton().CloseButton(false).CaptionVisible(false).PaneBorder(false));
	m_UIManager.AddPane(m_pPropertyList, wxAuiPaneInfo().Right().CaptionVisible(false).CloseButton(false).MinSize(350, -1));

	// create the status bar
	const uint numFields = 4;
	wxStatusBar* pStatusBar = CreateStatusBar(numFields);
	int statusWidths[numFields] = { 200, 200, 300, -1 };
	pStatusBar->SetStatusWidths(numFields, statusWidths);
	SetStatusText(_T("Ready"));

	m_UIManager.Update();

	// Init engine with default environment
	Engine::Instance()->Init();
	Engine::Instance()->GetContextStack()->AddChild(snew Context(), true);
	m_Camera = snew Camera();
	EntityRenderer::Instance()->Set3DCamera(m_Camera);
	EntityRenderer::Instance()->Get3DCamera()->SetPosition(Vector3::Create(0.0f, -10.0f, 0.0f));	
	Engine::Instance()->GetContextStack()->Init();

	// debug rendering
	Material::CreationInfo info;
	info.SetShader("../EditorData/debug_shader.xml");
	Material::CreationInfo::TextureInfo* pInfo = snew Material::CreationInfo::TextureInfo();
	pInfo->m_Path = "../EditorData/WhiteBG.png";
	info.m_TextureInfos.Add(pInfo);
	info.SetFlag(Material::MF_AlphaBlending, true);
	m_DebugMaterial = MaterialProvider::Instance()->GetMaterial(info);
	m_DebugVB = GraphicsDriver::Instance()->CreateVertexBuffer();
	Vertex3D* pVertices = snew Vertex3D[m_MaxDebugVertices];
	for(uint i=0; i<m_MaxDebugVertices; ++i)
	{
		pVertices[i].color = Color::White;
		pVertices[i].UV = Vector2::Zero;
	}
	m_DebugVB->SetVertices(pVertices, m_MaxDebugVertices, Vertex3D::VF_Pos|Vertex3D::VF_UV);
	m_DebugVB->SetDynamic(true);

	Maximize();

	super::Show(true);

	OnOpen(wxCommandEvent());
}

//! updates the current mesh entity to edit
void ExplosionEditor::UpdateCurrentMesh()
{	
	SetSelection(-1, -1);
	OnResetView(wxCommandEvent());
	Engine::Instance()->GetContext(0)->Uninit();

	if(MeshResource* pMesh = m_ExplosionVisitor->GetMesh())
	{
		MeshEntity* pMeshEntity = snew MeshEntity();
		pMeshEntity->SetMesh(m_ExplosionVisitor->GetMesh()->GetTemplatePath());
		pMeshEntity->Init();

		std::vector<SubMesh*> subMeshes = pMeshEntity->GetSubMeshes();
		for(uint i=0; i<subMeshes.size(); ++i)
		{
			GraphicComponent* pGraphic = subMeshes[i]->GetComponent<GraphicComponent>();
			Material* pMaterial = pGraphic->GetMaterial();
			VertexBuffer* pVB = pGraphic->GetVertexBuffer();

			pMaterial->SetFlag(Material::MF_AlphaBlending, true);
			//pMaterial->SetFlag(Material::MF_DepthWrite, false);
			
			pVB->SetDynamic(true);
			pVB->SetFormat(pVB->GetFormat() | Vertex3D::VF_Color);
			Vertex3D* pVertices = reinterpret_cast<Vertex3D*>(pVB->GetVertices());
			for(uint j=0; j<pVB->GetNumVertices(); ++j)
				pVertices[j].color = Color::Create(1.0f, 1.0f, 1.0f, 0.5f);
		}
		
		// force default shader
		std::vector<GraphicComponent*> gcs;
		pMeshEntity->GetComponents<GraphicComponent>(gcs, true);
		for(auto graphic : gcs)
			graphic->GetMaterial()->SetShader("common/shaders/tex.xml");

		Engine::Instance()->GetContext(0)->AddChild(pMeshEntity, true);
		Engine::Instance()->GetContext(0)->Init();
		pMeshEntity->AddComponent(m_ExplosionVisitor, true);
	}
	
	ValidateChunks();	
}

//! validates the chunks in the explosion visitor with respect to the selected mesh
void ExplosionEditor::ValidateChunks()
{
	Array<ExplosionVisitor::ChunkGroup>& aChunkGroups = m_ExplosionVisitor->GetChunkGroups();
	if(MeshResource* pMesh = m_ExplosionVisitor->GetMesh())
	{
		std::vector<SubMesh*> subMeshes = static_cast<MeshEntity*>(m_ExplosionVisitor->GetOwner())->GetSubMeshes();
		for(uint i=0; i<subMeshes.size(); ++i)
		{
			VertexBuffer* pVB = subMeshes[i]->GetComponent<GraphicComponent>()->GetVertexBuffer();
			uint numTriangles = pVB->GetNumVertices()/3;
			if(i < aChunkGroups.GetSize())
			{
				Array<ExplosionVisitor::Chunk>& aChunks = aChunkGroups[i].m_aChunks;
				for(uint j=0; j<aChunks.GetSize(); ++j)
				{
					Array<uint>& aTriangles = aChunks[j].aTriangleIndices;

					// remove chunks whose triangles are out of bounds with respect to the current mesh
					for(uint k=0; k<aTriangles.GetSize(); )
					{
						if(aTriangles[k] >= numTriangles)
						{
							aTriangles.Delete(k);
						}
						else
						{
							++k;
						}
					}
				}

				// remove empty chunks
				for(uint j=0; j<aChunks.GetSize(); )
				{
					if(aChunks[j].aTriangleIndices.GetSize() == 0)
					{
						aChunks.Delete(j);
					}
					else
					{
						++j;
					}
				}
			}
			else
			{
				aChunkGroups.Add(snew ExplosionVisitor::ChunkGroup());
			}
		}
	}
	else
	{
		aChunkGroups.Clear();
	}

	m_bChunksChanged = true;
}

//! returns the currently edited mesh
MeshEntity* ExplosionEditor::GetCurrentMesh()
{
	if(Engine::Instance()->GetContext(0)->GetChildCount() > 0)
	{
		return static_cast<MeshEntity*>(Engine::Instance()->GetContext(0)->GetChild(0));
	}
	else
	{
		return NULL;
	}
}

//! sets the selected chunk
//! sets the selection
void ExplosionEditor::SetSelection(int chunkIndex, int subMeshIndex)
{
	m_SelectedSubMesh = subMeshIndex;
	m_SelectedChunk = chunkIndex;
	
	if(chunkIndex >= 0 && subMeshIndex >= 0)
	{
		MeshEntity* pMesh = GetCurrentMesh();	
		ExplosionVisitor* pExplosionVisitor = pMesh->GetComponent<ExplosionVisitor>();
		Array<ExplosionVisitor::ChunkGroup>& aChunkGroups = pExplosionVisitor->GetChunkGroups();
		Array<ExplosionVisitor::Chunk>& aChunks = aChunkGroups[subMeshIndex].m_aChunks;

		char text[128];
		std::stringstream ss;
		ss << "ExplosionChunk: " << chunkIndex << ", Triangles: " << aChunks[chunkIndex].aTriangleIndices.GetSize();
		ss.get(text, 128);
		SetStatusText(text, 2);
	}
	else
	{
		SetStatusText("", 2);		
	}	
}

//! renders a triangle
void ExplosionEditor::DrawTriangle(const Vector3& v1, const Vector3& v2, const Vector3& v3)
{
	Vertex3D* pVertices = reinterpret_cast<Vertex3D*>(m_DebugVB->GetVertices());
	pVertices[0].Pos = v1;
	pVertices[1].Pos = v2;
	pVertices[2].Pos = v3;
	m_DebugVB->SetDirty(true);
	m_DebugVB->SetNumVertices(3);
	m_DebugVB->Begin(m_DebugMaterial->GetShader());
	m_DebugVB->Draw();
	m_DebugVB->End();
}

//! called when the chunks changed
void ExplosionEditor::OnChunksChanged()
{
	m_pPropertyList->Fill(m_ExplosionVisitor);
	SetSelection(m_SelectedChunk, m_SelectedSubMesh);
}

// event handlers
void ExplosionEditor::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	// true is to force the frame to close
	Close(true);	
}

void ExplosionEditor::OnClose(wxCloseEvent& event)
{
	// TODO custom closing code
	event.Skip();		
}

void ExplosionEditor::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxMessageBox("Amine Rehioui 2010", "Shoot Editor", wxOK | wxICON_INFORMATION, this);
}

void ExplosionEditor::OnIdle(wxIdleEvent& event)	
{
	if(m_bChunksChanged)
	{
		OnChunksChanged();
		m_bChunksChanged = false;
	}

	m_pViewPort->Refresh(true);
}

void ExplosionEditor::OnNew(wxCommandEvent& event)
{
	m_ExplosionVisitor = snew ExplosionVisitor();
	m_pPropertyList->Fill(m_ExplosionVisitor);
	Engine::Instance()->GetContext(0)->Clear();
	SetSelection(-1, -1);
	OnResetView(wxCommandEvent());
}

void ExplosionEditor::OnOpen(wxCommandEvent& event)
{
	std::string explosionVisitorPath;
	if(EditorUtils::OpenFileDialog(this, explosionVisitorPath, "Explosion visitor file (*.xml)|*.xml", "Locate explosion visitor (if available)"))
	{
		Object* pObject = ObjectManager::Instance()->FindOrLoad(explosionVisitorPath.c_str());
		m_ExplosionVisitor = DYNAMIC_CAST(pObject, ExplosionVisitor);
		if(!m_ExplosionVisitor.IsValid())
		{
			wxMessageBox("Object not of type ExplisionVisitor", "Error", wxOK | wxICON_ERROR, this);
			sdelete(pObject);
			return;
		}
	}
	else
	{
		m_ExplosionVisitor = snew ExplosionVisitor();
	}

	UpdateCurrentMesh();
}

void ExplosionEditor::OnSave(wxCommandEvent& event)
{
	if(MeshEntity* pMesh = GetCurrentMesh())
	{
		ExplosionVisitor* pExplosionVisitor = pMesh->GetComponent<ExplosionVisitor>();
		std::string path = pExplosionVisitor->GetTemplatePath();
		bool bExists = !pExplosionVisitor->GetTemplatePath().empty();
		if(bExists || EditorUtils::OpenFileDialog(this, path, "Object files (*.xml)|*.xml", "Save File", wxFD_SAVE))			
		{
			EditorUtils::FormatObjectPath(pExplosionVisitor, path);
			pExplosionVisitor->SaveToXML(path.c_str());
		}
	}
}

void ExplosionEditor::OnResetView(wxCommandEvent& event)
{
	*EntityRenderer::Instance()->Get3DCamera() = Camera();
	EntityRenderer::Instance()->Get3DCamera()->SetPosition(Vector3::Create(0.0f, -10.0f, 0.0f));	
}

//! call on property change
void ExplosionEditor::OnPropertyChanged(wxPropertyGridEvent &event)
{
	ExplosionVisitor* pObject = m_ExplosionVisitor;
	PropertyStream stream(shoot::SM_Write);
	pObject->Serialize(stream);

	// check if this is a sub property
	wxPGProperty* pWxProperty = event.GetProperty();
	if(!pWxProperty->GetParent()->IsRoot()) 
	{
		PropertyLocator locator;
		IProperty* pParentProperty = locator.GetProperty(pWxProperty->GetParent(), stream);

		// if this was part of an array, mark it
		if(pParentProperty->GetType() == PT_Array)
		{
			wxArrayProperty* pWxArrayParent = static_cast<wxArrayProperty*>(pWxProperty->GetParent());
			pWxArrayParent->SetEventType(wxArrayProperty::E_SubPropertyChanged);
			pWxArrayParent->SetChangedSubPropertyIndex(pWxProperty->GetIndexInParent());
		}			
		else if(pParentProperty->GetType() == PT_Reference)
		{
			wxReferenceProperty* pWxRefParent = static_cast<wxReferenceProperty*>(pWxProperty->GetParent());
			pWxRefParent->SetEventType(wxReferenceProperty::E_SubPropertyChanged);				
		}

		IProperty* pRootProperty = locator.GetRootProperty(pWxProperty, stream);		

		PropertyList::FillProperty(pWxProperty->GetParent(), pParentProperty);
		stream.SetMode(shoot::SM_Read);
		stream.SetPropertyToUpdate(pRootProperty->GetName());
		pObject->Serialize(stream);
	}
	else 
	{
		PropertyLocator locator;
		IProperty* pProperty = locator.GetProperty(pWxProperty, stream);
		SHOOT_ASSERT(pProperty->GetName() == pWxProperty->GetName(), "wxProperty and shoot Property mismatch");		

		PropertyList::FillProperty(pWxProperty, pProperty);
		stream.SetMode(shoot::SM_Read);
		stream.SetPropertyToUpdate(pProperty->GetName());
		pObject->Serialize(stream);
	}

	if(pWxProperty->GetName() == "Mesh")
	{
		UpdateCurrentMesh();
	}
}

//! called while a property is changing
void ExplosionEditor::OnPropertyChanging(wxPropertyGridEvent &event)
{

}

//! called while a property is selected
void ExplosionEditor::OnPropertySelected(wxPropertyGridEvent &event)
{
	if(wxPGProperty* pProperty = event.GetProperty())
	{
		std::string name = pProperty->GetParent()->GetBaseName();
		if(name == "Chunks")
		{
			SetSelection(pProperty->GetIndexInParent(), pProperty->GetParent()->GetParent()->GetIndexInParent());			
		}
	}	
}

