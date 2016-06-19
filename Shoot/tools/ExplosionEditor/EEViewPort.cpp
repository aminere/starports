/* 

Amine Rehioui
Created: December 24th 2012

*/

#include "ShootEditorCommon.h"

#include "EEViewPort.h"

#include "CameraManager.h"
#include "EditorSettings.h"

#include "ExplosionEditor.h"
#include "Triangle.h"
#include "Plane.h"

// Define event table
BEGIN_EVENT_TABLE(EEViewPort, wxGLCanvas)
	EVT_MOTION(EEViewPort::OnMouseMove)
	EVT_LEFT_DOWN(EEViewPort::OnMouseLeftDown)
	EVT_LEFT_UP(EEViewPort::OnMouseLeftUp)
	EVT_MIDDLE_DOWN(EEViewPort::OnMouseMiddleDown)
	EVT_MIDDLE_UP(EEViewPort::OnMouseMiddleUp)
	EVT_RIGHT_DOWN(EEViewPort::OnMouseRightDown)
	EVT_RIGHT_UP(EEViewPort::OnMouseRightUp)
	EVT_MOUSEWHEEL(EEViewPort::OnMouseWheelRolled)
	EVT_KEY_DOWN(EEViewPort::OnKeyDown)
	EVT_KEY_UP(EEViewPort::OnKeyUp)
	EVT_LEAVE_WINDOW(EEViewPort::OnMouseLeave)
	EVT_SIZE(EEViewPort::OnResized)
	EVT_SET_FOCUS(EEViewPort::OnFocusGained)
	EVT_KILL_FOCUS(EEViewPort::OnFocusLost)
	EVT_PAINT(EEViewPort::OnPaint)
	EVT_ERASE_BACKGROUND(EEViewPort::OnEraseBackground)
END_EVENT_TABLE()

//! Constructor
EEViewPort::EEViewPort(wxWindow *parent)
	: super(parent, wxID_ANY, NULL, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
	, m_bZoomingStarted(false)
	, m_bLookingStarted(false)
	, m_bStrafingStarted(false)
	, m_SelectedTriangleIndex(-1)
	, m_SelectedSubMeshIndex(-1)
{
	EditorSettings::CreateInstance();
	CameraManager::CreateInstance();
	CameraManager::Instance()->SetMode(CameraManager::M_Inspect);

	_glContext = new wxGLContext(this);
	_glContext->SetCurrent(*this);
}

//! Destructor
EEViewPort::~EEViewPort()
{
	sdelete(_glContext);

	EditorSettings::DestroyInstance();
	CameraManager::DestroyInstance();
}

//! returns the index of a triangle in a chunk, -1 if not found
int EEViewPort::GetTriangleIndex(ExplosionVisitor::Chunk& chunk, uint triangle)
{
	int index = -1;
	for(int i=0; i<(int)chunk.aTriangleIndices.GetSize(); ++i)
	{
		if(chunk.aTriangleIndices[i] == triangle)
		{
			index = i;
			break;
		}
	}
	return index;
}

//! returns the index of a chunk containing a triangle, -1 if not found
int EEViewPort::GetChunkIndex(Array<ExplosionVisitor::Chunk>& aChunks, uint triangle)
{
	int index = -1;
	for (int i = 0; i<(int)aChunks.GetSize(); ++i)
	{
		for (int j = 0; (int)j<aChunks[i].aTriangleIndices.GetSize(); ++j)
		{
			if(aChunks[i].aTriangleIndices[j] == triangle)
			{
				index = i;
				break;
			}
		}
	}	
	return index;
}

// event handlers
void EEViewPort::OnMouseMove(wxMouseEvent& event)
{
	// Update the status bar
	char strPosition[128];
	std::stringstream ss;
	ss << event.GetX() << ", " << event.GetY();
	ss.get(strPosition, 128);
	static_cast<wxFrame*>(wxTheApp->GetTopWindow())->SetStatusText(strPosition, 1);

	Vector2 mousePos = Vector2::Create(event.GetX(), event.GetY());
	Vector2 deltaPos = mousePos - m_LastMousePosition;
	Camera* pCamera = EntityRenderer::Instance()->Get3DCamera();

	if(m_bZoomingStarted)
	{
		CameraManager::Instance()->ZoomCamera(pCamera, deltaPos.Y);
	}
	else if(m_bLookingStarted)
	{
		CameraManager::Instance()->RotateCamera(pCamera, deltaPos);
	}
	else if (m_bStrafingStarted)
	{
		CameraManager::Instance()->StrafeCamera(pCamera, deltaPos);
	}
	else
	{
		// determine the selected triangle
		m_SelectedTriangleIndex = -1;
		m_SelectedSubMeshIndex = -1;
		static_cast<wxFrame*>(wxTheApp->GetTopWindow())->SetStatusText("", 3);

		ExplosionEditor* pEditor = static_cast<ExplosionEditor*>(wxTheApp->GetTopWindow());		
		if(MeshEntity* pMesh = pEditor->GetCurrentMesh())
		{
			Vector3 rayStart, rayDir;
			pCamera->Get3DRayFromScreenPos(rayStart, rayDir, mousePos);
			float fMinDist = Math::Maxf32;

			std::vector<SubMesh*> subMeshes = pMesh->GetSubMeshes();
			for(uint i=0; i<subMeshes.size(); ++i)
			{
				VertexBuffer* pVertexBuffer = subMeshes[i]->GetComponent<GraphicComponent>()->GetVertexBuffer();
				//ushort* pIndices = (ushort*)pVertexBuffer->GetIndices();
				//uint numTriangles = pVertexBuffer->GetNumIndices() / 3;
				Vertex3D* pVertices = reinterpret_cast<Vertex3D*>(pVertexBuffer->GetVertices());
				uint numTriangles = pVertexBuffer->GetNumVertices() / 3;
				auto transform = subMeshes[i]->GetWorldTransform();

				for (uint j = 0; j<numTriangles; ++j)
				{
					uint index1 = j * 3 + 0;
					uint index2 = j * 3 + 1;
					uint index3 = j * 3 + 2;

					Vector3 v1 = transform.TransformVect(pVertices[index1].Pos);
					Vector3 v2 = transform.TransformVect(pVertices[index2].Pos);
					Vector3 v3 = transform.TransformVect(pVertices[index3].Pos);

					Plane plane(v1, v2, v3);
					Vector3 vIntersection;
					Plane::E_Classification eClass;
					if(plane.IntersectWithRay(rayStart, rayDir, &vIntersection, &eClass))
					{
						if(eClass == Plane::C_Front)
						{
							Triangle triangle(v1, v2, v3);
							if(triangle.IsPointInside(vIntersection))
							{
								float fDist = (vIntersection - rayStart).GetLengthSquared();
								if(fDist < fMinDist)
								{
									m_SelectedTriangleIndex = j;
									m_SelectedSubMeshIndex = i;
									fMinDist = fDist;
								}
							}
						}
					}
				}
			}

			if(m_SelectedTriangleIndex >= 0)
			{
				char buf[128];
				std::stringstream ss;
				ss << "SubMesh: " << m_SelectedSubMeshIndex;
				ss.get(buf, 128);
				static_cast<wxFrame*>(wxTheApp->GetTopWindow())->SetStatusText(buf, 3);
			}
		}
	}

	m_LastMousePosition = mousePos;
}

void EEViewPort::OnMouseLeftDown(wxMouseEvent& event)
{
	SetFocus();

	Vector2 clickPos = Vector2::Create(event.GetX(), event.GetY());
	if(event.AltDown())
	{
		m_bLookingStarted = true;
	}
	else
	{
		ExplosionEditor* pEditor = static_cast<ExplosionEditor*>(wxTheApp->GetTopWindow());
		if(m_SelectedTriangleIndex < 0)
		{
			pEditor->SetSelection(-1, -1);			
		}
		else
		{
			MeshEntity* pMesh = pEditor->GetCurrentMesh();
			ExplosionVisitor* pExplosionVisitor = pMesh->GetComponent<ExplosionVisitor>();
			Array<ExplosionVisitor::ChunkGroup>& aChunkGroups = pExplosionVisitor->GetChunkGroups();
			int selectedChunk = pEditor->GetSelectedChunk();
			int selectedSubMesh = pEditor->GetSelectedSubMesh();

			if(selectedChunk >= 0
			&& selectedSubMesh == m_SelectedSubMeshIndex
			&& event.ControlDown())
			{
				// find out if the selected triangle is part of the selected chunk				
				int triangleIndexInChunk = GetTriangleIndex(aChunkGroups[selectedSubMesh].m_aChunks[selectedChunk], m_SelectedTriangleIndex);

				// flip the selected triangle status
				if(triangleIndexInChunk >= 0)
				{
					aChunkGroups[selectedSubMesh].m_aChunks[selectedChunk].aTriangleIndices.Delete(triangleIndexInChunk);
				}
				else
				{
					aChunkGroups[selectedSubMesh].m_aChunks[selectedChunk].aTriangleIndices.Add(snew uint(m_SelectedTriangleIndex));

					// make sure the triangle is removed from every other chunk
					for(uint i=0; i<aChunkGroups[selectedSubMesh].m_aChunks.GetSize(); ++i)
					{
						if(i != selectedChunk)
						{
							int triangleIndexInChunk = GetTriangleIndex(aChunkGroups[selectedSubMesh].m_aChunks[i], m_SelectedTriangleIndex);
							if(triangleIndexInChunk >= 0)
							{
								aChunkGroups[selectedSubMesh].m_aChunks[i].aTriangleIndices.Delete(triangleIndexInChunk);
							}
						}
					}
				}
				
				pEditor->OnChunksChanged();
			}
			else
			{
				int chunkIndex = GetChunkIndex(aChunkGroups[m_SelectedSubMeshIndex].m_aChunks, m_SelectedTriangleIndex);
				if(chunkIndex >= 0)
				{
					pEditor->SetSelection(chunkIndex, m_SelectedSubMeshIndex);					
				}
			}
		}
	}
}

void EEViewPort::OnMouseLeftUp(wxMouseEvent& event)
{
	m_bLookingStarted = false;
}

void EEViewPort::OnMouseMiddleDown(wxMouseEvent& event)
{
}

void EEViewPort::OnMouseMiddleUp(wxMouseEvent& event)
{
}

void EEViewPort::OnMouseRightDown(wxMouseEvent& event)
{
	if (event.AltDown())
		m_bZoomingStarted = true;
	else
		m_bStrafingStarted = true;	
}

void EEViewPort::OnMouseRightUp(wxMouseEvent& event)
{
	m_bZoomingStarted = false;
	m_bStrafingStarted = false;
}

void EEViewPort::OnMouseWheelRolled(wxMouseEvent& event)
{			
}

void EEViewPort::OnKeyDown(wxKeyEvent& event)
{
}

void EEViewPort::OnKeyUp(wxKeyEvent& event)
{				
}

void EEViewPort::OnMouseLeave(wxMouseEvent& event)
{	
	m_bZoomingStarted = false;
	m_bStrafingStarted = false;
	m_bLookingStarted = false;
}

void EEViewPort::OnResized(wxSizeEvent& event)
{
	event.Skip();
	
	
	_glContext->SetCurrent(*this);
	Engine::Instance()->ResizeScreen(Vector2::Create(float(event.GetSize().x), float(event.GetSize().y)));	
}

void EEViewPort::OnFocusGained(wxFocusEvent& event)
{
	Refresh(true); // make sure background is erased and refreshed		
}

void EEViewPort::OnFocusLost(wxFocusEvent& event)
{
}

void EEViewPort::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	_glContext->SetCurrent(*this);

	Engine::Instance()->Render();

	// custom rendering
	GraphicsDriver::Instance()->SetRenderTarget(NULL);
	GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_DepthFunc, GraphicsDriver::CF_LessOrEqual);
	GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_Projection, EntityRenderer::Instance()->Get3DCamera()->GetProjectionMatrix());
	GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_View, EntityRenderer::Instance()->Get3DCamera()->GetViewMatrix());		
	GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_LineWidth, 1.0f);
	ExplosionEditor* pEditor = static_cast<ExplosionEditor*>(wxTheApp->GetTopWindow());
	MeshEntity* pMesh = pEditor->GetCurrentMesh();
	ExplosionVisitor* pExplosionVisitor = pMesh ? pMesh->GetComponent<ExplosionVisitor>() : NULL;
	if(pMesh && pExplosionVisitor)
	{
		Color wireColor = Color::Create(0.5f, 0.5f, 0.5f);
		Color selectedColor = Color::White;
		Color chunkColor = Color::Create(0.5f, 0.0f, 0.5f, 0.5f);
		Color selectedChunkColor = Color::Create(1.0f, 1.0f, 0.0f, 0.5f);

		std::vector<SubMesh*> subMeshes = pMesh->GetSubMeshes();
		Array<ExplosionVisitor::ChunkGroup>& aChunkGroups = pExplosionVisitor->GetChunkGroups();
		Material* pMaterial = pEditor->GetMaterial();
		pMaterial->Begin();
		pMaterial->GetShader()->Begin();

		for(uint i=0; i<(uint)subMeshes.size(); ++i)
		{
			VertexBuffer* pVB = subMeshes[i]->GetComponent<GraphicComponent>()->GetVertexBuffer();
			Vertex3D* pVertices = reinterpret_cast<Vertex3D*>(pVB->GetVertices());
			//ushort* pIndices = (ushort*)pVB->GetIndices();
			GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, subMeshes[i]->GetWorldTransform());
			GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_FillMode, GraphicsDriver::FM_Line);
			pMaterial->GetShader()->UploadConstant<Color>("uAmbientColor", &wireColor, 1);
			pVB->Begin(pMaterial->GetShader());
			pVB->Draw();
			pVB->End();

			// render explosion chunks	
			int selectedChunk = pEditor->GetSelectedChunk();
			int selectedSubMesh = pEditor->GetSelectedSubMesh();

			GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_AlphaBlending, true);
			GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_FillMode, GraphicsDriver::FM_Solid);
			Array<ExplosionVisitor::Chunk>& aChunks = aChunkGroups[i].m_aChunks;
			for(uint j=0; j<aChunks.GetSize(); ++j)
			{
				if(i == selectedSubMesh
					&& j == selectedChunk)
				{
					GraphicsDriver::Instance()->SetCullMode(GraphicsDriver::CM_None);
					pMaterial->GetShader()->UploadConstant<Color>("uAmbientColor", &selectedChunkColor, 1);
				}
				else
				{
					GraphicsDriver::Instance()->SetCullMode(GraphicsDriver::CM_CounterClockWise);
					pMaterial->GetShader()->UploadConstant<Color>("uAmbientColor", &chunkColor, 1);
				}

				for (uint k = 0; k<aChunks[j].aTriangleIndices.GetSize(); ++k)
				{
					uint triangleIndex = aChunks[j].aTriangleIndices[k];
					pEditor->DrawTriangle(pVertices[(triangleIndex*3)+0].Pos, pVertices[(triangleIndex*3)+1].Pos, pVertices[(triangleIndex*3)+2].Pos);
				}
			}

			GraphicsDriver::Instance()->SetCullMode(GraphicsDriver::CM_CounterClockWise);
			if(m_SelectedTriangleIndex >= 0 && m_SelectedSubMeshIndex == i)
			{
				GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_FillMode, GraphicsDriver::FM_Line);
				pMaterial->GetShader()->UploadConstant<Color>("uAmbientColor", &selectedColor, 1);
				pEditor->DrawTriangle(pVertices[(m_SelectedTriangleIndex*3)+0].Pos, pVertices[(m_SelectedTriangleIndex*3)+1].Pos, pVertices[(m_SelectedTriangleIndex*3)+2].Pos);
			}
		}

		pEditor->GetMaterial()->GetShader()->End();
		pEditor->GetMaterial()->End();
	}

	GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_FillMode, GraphicsDriver::FM_Solid);

	glFlush();
	SwapBuffers();
}

void EEViewPort::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
	// Do nothing, to avoid flashing.
}

