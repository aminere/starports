/* 

Amine Rehioui
Created: August 20th 2011

*/

#include "ShootEditorCommon.h"

#include "EditorRenderer.h"

#include "Path.h"

#include "EditorSettings.h"

#include "Entity3DController.h"

#include "Plane.h"

#include "CollisionComponent.h"

#include "MeshEntity.h"

#include "ParticleGenerator.h"

#include "OpenGL/OpenGLExtensionHandler.h"

#include "EnvironmentCollisionComponent.h"
#include "RigidBodyComponent.h"
#include "SkyBoxEntity.h"
#include "Light.h"
#include "MaterialProvider.h"
#include "SkyBoxVertexBufferNoCubemap_NoVAO.h"
#include "LayoutComponent.h"

namespace shoot
{
	//! constructor
	EditorRenderer::EditorRenderer()
		: m_bEnabled(true)
		, m_bPickingInfoValid(false)
		, m_MaxDebugVertices(16)
	{
	}

	//! destructor
	EditorRenderer::~EditorRenderer()
	{
	}

	//! Init
	void EditorRenderer::Init()
	{
		m_UnitSphere = ResourceManager::Instance()->GetResource<MeshResource>("../EditorData/UnitSphere.xml");

		m_GridVertexBuffer = GraphicsDriver::Instance()->CreateVertexBuffer();
		m_GridVertexBuffer->SetPrimitiveType(GraphicsDriver::PT_Line);
		m_GridVertexBuffer->SetDynamic(true);

		m_PinTexture = ResourceManager::Instance()->GetResource<Texture>("../EditorData/DebugIcons/Pin.xml");

		// debug icons
		m_DebugIconEntity = snew BillBoardEntity();
		GraphicComponent* pGraphic = snew GraphicComponent();
		m_DebugIconEntity->AddComponent(pGraphic, true);
		m_DebugIconEntity->Init();
		m_DebugIconEntity->SetScale(Vector3::One*EditorSettings::Instance()->fDebugIconSize);
		m_DebugIconEntity->Update();

		// debug rendering
		Material::CreationInfo info;
		info.SetShader("../EditorData/debug_shader.xml");
		info.AddTexture("../EditorData/WhiteBG.xml");
		info.SetFlag(Material::MF_AlphaBlending, true);
		m_DebugMaterial = MaterialProvider::Instance()->GetMaterial(info);
		m_DebugVB = GraphicsDriver::Instance()->CreateVertexBuffer();
		Vertex3D* pVertices = snew Vertex3D[m_MaxDebugVertices];		
		m_DebugVB->SetVertices(pVertices, m_MaxDebugVertices, Vertex3D::VF_Pos|Vertex3D::VF_UV);
		m_DebugVB->SetDynamic(true);
	}

	//! performs editor rendering
	void EditorRenderer::Render()
	{
		if(Engine::Instance()->IsLoading())
			return;

		GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_FillMode, GraphicsDriver::FM_Line);
		GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_LineWidth, 1.0f);

		if(m_bEnabled)
		{
			Entity* pSelectedEntity = ShootEditor::Instance()->GetSelectedEntity();
			Entity* pRoot = pSelectedEntity ? pSelectedEntity->GetRoot() : Engine::Instance()->GetContextStack();
			ExtractEntities(pRoot);

			GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_DepthTesting, false);

			m_DebugMaterial->Begin();
			m_DebugMaterial->GetShader()->Begin();

			RenderPost3D();
			RenderPost2D();

			m_2DEntities.clear();
			m_aPaths.clear();
			m_3DEntities.clear();
		}

		// render entity controller
		if(Camera* pCamera = EntityRenderer::Instance()->Get3DCamera())
		{
			GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_Projection, pCamera->GetProjectionMatrix());
			GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_View, pCamera->GetViewMatrix());
			GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, Matrix44::Identity);
			GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_DepthTesting, false);

			m_DebugMaterial->Begin();
			m_DebugMaterial->GetShader()->Begin();
			Entity3DController::Instance()->Render();
		}

		GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_FillMode, GraphicsDriver::FM_Solid);
	}

	//! extracts entities for rendering
	void EditorRenderer::ExtractEntities(Entity* pEntity)
	{
		for (size_t i = 0; i<pEntity->GetChildCount(); ++i)
		{
			RegisterEntity(pEntity->GetChild(i));
			ExtractEntities(pEntity->GetChild(i));
		}
	}

	//! registers an entity for post rendering
	void EditorRenderer::RegisterEntity(Entity* pEntity)
	{
		if(auto entity2D = DYNAMIC_CAST(pEntity, Entity2D))
		{
			m_2DEntities.push_back(entity2D);
		}
		else if(auto pPath = DYNAMIC_CAST(pEntity, Path))
		{
			if(pPath->GetChildCount() > 0)
				m_aPaths.push_back(pPath);
		}
		else if(auto entity3D = DYNAMIC_CAST(pEntity, Entity3D))
		{
			m_3DEntities.push_back(entity3D);
		}
	}

	//! custom Post 2D rendering
	void EditorRenderer::RenderPost2D()
	{
		Camera* pCamera = EntityRenderer::Instance()->Get2DCamera();
		GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_Projection, pCamera->GetProjectionMatrix());
		GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_View, pCamera->GetViewMatrix());
		GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, Matrix44::Identity);

		// render 2D origin
		if(EditorSettings::Instance()->HasFlag(EditorSettings::EF_Render2DOrigin))
		{
			EditorRenderer::Instance()->DrawLine(Vector3::Zero, Vector3::Create(EditorSettings::Instance()->f2DOriginSize, 0.0f, 0.0f), Color::Red);
			EditorRenderer::Instance()->DrawLine(Vector3::Zero, Vector3::Create(0.0f, EditorSettings::Instance()->f2DOriginSize, 0.0f), Color::Green);
		}

		// render entity specific elements
		for(auto pEntity2D : m_2DEntities)
		{
			GraphicComponent* pGraphic = pEntity2D->GetComponent<GraphicComponent>();
			bool hasLayout = (pEntity2D->GetComponent<LayoutComponent>() != NULL || (pGraphic && pGraphic->GetRenderingPass() == GraphicComponent::RP_2DLayout));
			GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_View, hasLayout ? Matrix44::Identity : pCamera->GetViewMatrix());

			// render 2d bounding boxes
			if(EditorSettings::Instance()->HasFlag(EditorSettings::EF_RenderBBox))
			{
				GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, pEntity2D->GetWorldTransform());
				EditorRenderer::Instance()->DrawDebugBox(pEntity2D->GetBoundingBox(), EditorSettings::Instance()->BBoxColor);
			}

			// render Entity2D positions			
			if(EditorSettings::Instance()->HasFlag(EditorSettings::EF_RenderEntity2DPos))			
			{
				GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, pEntity2D->GetCenterTransformationMatrix());

				Color color = (pEntity2D == ShootEditor::Instance()->GetSelectedEntity())
							? EditorSettings::Instance()->SelectedEntityColor 
							: EditorSettings::Instance()->EntityPosColor;
				float fSize = EditorSettings::Instance()->fEntity2DPosSize;
				EditorRenderer::Instance()->DrawLine(Vector3::Create(-fSize/2.0f/pEntity2D->GetScale().X, 0.0f, 0.0f), Vector3::Create(fSize/2.0f/pEntity2D->GetScale().X, 0.0f, 0.0f), color);
				EditorRenderer::Instance()->DrawLine(Vector3::Create(0.0f, -fSize/2.0f/pEntity2D->GetScale().Y, 0.0f), Vector3::Create(0.0f, fSize/2.0f/pEntity2D->GetScale().Y, 0.0f), color);
			}
		}

		// render selected entity at the end
		if(auto entity2D = DYNAMIC_CAST(ShootEditor::Instance()->GetSelectedEntity(), Entity2D))
		{
			auto entity2DGC = entity2D->GetComponent<GraphicComponent>();
			auto hasLayout = (entity2D->GetComponent<LayoutComponent>() != NULL || (entity2DGC && entity2DGC->GetRenderingPass() == GraphicComponent::RP_2DLayout));
			GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_View, hasLayout ? Matrix44::Identity : pCamera->GetViewMatrix());
			GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, entity2DGC ? entity2DGC->GetWorldTransform() : entity2D->GetWorldTransform());
			EditorRenderer::Instance()->DrawDebugBox(entity2D->GetBoundingBox(), EditorSettings::Instance()->SelectedEntityColor);
		}

		// restore camera view matrix in case it was changed by layouted elements
		GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_View, pCamera->GetViewMatrix());
	}

	//! custom Post 3D rendering
	void EditorRenderer::RenderPost3D()
	{
		Camera* pCamera = EntityRenderer::Instance()->Get3DCamera();
		GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_Projection, pCamera->GetProjectionMatrix());
		GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_View, pCamera->GetViewMatrix());
		GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, Matrix44::Identity);

		// render grid
		if(EditorSettings::Instance()->HasFlag(EditorSettings::EF_Render3DGrid))		
		{
			const int gridSize = EditorSettings::Instance()->GridSize;
			const int numGridVertices = (gridSize+1)*4;
			if(m_GridVertexBuffer->GetNumVertices() != numGridVertices)
			{
				Vertex3D* pVertices = snew Vertex3D[numGridVertices];
				Vector3 lineStartHoriz = Vector3::Create(-float(gridSize)/2, -float(gridSize)/2, 0.0f);
				Vector3 lineStartVert = lineStartHoriz;
				int currentVertex = 0;
				for(int i=0; i<=gridSize; ++i)
				{
					pVertices[currentVertex].Pos = lineStartHoriz; pVertices[currentVertex++].UV = Vector2::Zero;
					pVertices[currentVertex].Pos = lineStartHoriz+Vector3::Create(float(gridSize), 0.0f, 0.0f); pVertices[currentVertex++].UV = Vector2::Zero;
					pVertices[currentVertex].Pos = lineStartVert; pVertices[currentVertex++].UV = Vector2::Zero;
					pVertices[currentVertex].Pos = lineStartVert+Vector3::Create(0.0f, float(gridSize), 0.0f); pVertices[currentVertex++].UV = Vector2::Zero;
					lineStartHoriz.Y++;
					lineStartVert.X++;
				}
				SHOOT_ASSERT(currentVertex == numGridVertices, "Grid rendering error");
				m_GridVertexBuffer->SetVertices(pVertices, numGridVertices, Vertex3D::VF_Pos|Vertex3D::VF_UV);
			}

			m_DebugMaterial->GetShader()->UploadConstant<Color>("uAmbientColor", &Color::Create(0.2f, 0.2f, 0.2f), 1);
			m_GridVertexBuffer->Begin(m_DebugMaterial->GetShader());
			m_GridVertexBuffer->Draw();
		}

		// render origin
		if(EditorSettings::Instance()->HasFlag(EditorSettings::EF_Render3DOrigin))			
		{
			EditorRenderer::Instance()->DrawLine(Vector3::Zero, Vector3::Create(1.0f, 0.0f, 0.0f), Color::Red);			
			EditorRenderer::Instance()->DrawLine(Vector3::Zero, Vector3::Create(0.0f, 1.0f, 0.0f), Color::Green);			
			EditorRenderer::Instance()->DrawLine(Vector3::Zero, Vector3::Create(0.0f, 0.0f, 1.0f), Color::Blue);
		}

		// render quad trees
		if(EditorSettings::Instance()->HasFlag(EditorSettings::EF_RenderQuadTrees))
		{
			for(size_t i=0; i<Engine::Instance()->GetNumContexts(); ++i)
			{
				if(QuadTree* pQuadTree = Engine::Instance()->GetContext(i)->GetQuadTree())
				{
					if(QuadTreeNode* pRoot = pQuadTree->GetRoot())
					{
						RenderQuadTreeNodes(pRoot);
						RenderVisibleQuadTreeNodes(pRoot);						
					}
				}
			}
		}

		// render entity specific elements
		for(auto pEntity3D : m_3DEntities)
		{
			// render bounding boxes
			if(EditorSettings::Instance()->HasFlag(EditorSettings::EF_RenderBBox)
			&& !pEntity3D->IsA(MeshEntity::TypeID)
			&& !pEntity3D->IsA(SubMesh::TypeID)
			&& pEntity3D != EntityRenderer::Instance()->Get3DCamera())
			{
				AABBox3D box(-Vector3::One/2.0f, Vector3::One/2.0f);
				if(ParticleGenerator *pParticleGenerator = DYNAMIC_CAST(pEntity3D, ParticleGenerator))
				{
					box = pParticleGenerator->GetEmitBox();
					GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, pEntity3D->Entity3D::GetWorldTransform());
				}
				else
				{
					GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, pEntity3D->GetWorldTransform());
				}

				Color color = (pEntity3D == ShootEditor::Instance()->GetSelectedEntity())
							? EditorSettings::Instance()->SelectedEntityColor
							: EditorSettings::Instance()->BBoxColor;

				EditorRenderer::Instance()->DrawDebugBox(box, color);
			}

			// render Entity3D positions			
			if(EditorSettings::Instance()->HasFlag(EditorSettings::EF_RenderEntity3DPos)
				&& pEntity3D != EntityRenderer::Instance()->Get3DCamera())
			{
				GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, pEntity3D->GetCenterTransformationMatrix());

				Color color = (pEntity3D == ShootEditor::Instance()->GetSelectedEntity())
					        ? EditorSettings::Instance()->SelectedEntityColor
							: EditorSettings::Instance()->EntityPosColor;

				EditorRenderer::Instance()->DrawCross(Vector3::Zero, EditorSettings::Instance()->fEntity3DPosSize/2.0f, color);
			}

			// render collision shapes if any
			if(EditorSettings::Instance()->HasFlag(EditorSettings::EF_CollisionShapes))
			{
				std::vector<CollisionComponent*> colComponents;
				pEntity3D->GetComponents<CollisionComponent>(colComponents);
				for (size_t i=0; i<colComponents.size(); ++i)
				{
					if(colComponents[i]->IsEnabled())
					{
						RenderCollisionShapes(colComponents[i]);
					}
				}
			}		

			// render frustums
			if(EditorSettings::Instance()->HasFlag(EditorSettings::EF_RenderFrustums)
				&& pEntity3D == ShootEditor::Instance()->GetSelectedEntity())
			{
				if(Camera* pCamera = DYNAMIC_CAST(pEntity3D, Camera))
				{
					if(pCamera->GetType() != Projection::Ortho2D)					
						RenderFrustum(pCamera->GetFrustum());					
				}
				else if(Light* pLight = DYNAMIC_CAST(pEntity3D, Light))				
					RenderFrustum(pLight->GetFrustum());				
				else if (auto lightLookAt = DYNAMIC_CAST(pEntity3D, LightLookAt))				
					RenderFrustum(static_cast<Light*>(lightLookAt->GetParent())->GetFrustum());				
			}

			// render debug icons
			if(EditorSettings::Instance()->HasFlag(EditorSettings::EF_DebugIcons))
			{
				m_DebugMaterial->GetShader()->UploadConstant<Color>("uAmbientColor", &Color::White, 1);
				for(DebugIconMap::iterator it = m_DebugIconMap.begin(); it != m_DebugIconMap.end(); ++it)
				{
					if(pEntity3D->IsA(it->first))
					{
						Texture* pTexture = it->second.Get();
						pTexture->Begin(0);
						m_DebugIconEntity->SetPosition(pEntity3D->GetWorldTransform().GetTranslation());
						GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, m_DebugIconEntity->GetWorldTransform());
						GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_FillMode, GraphicsDriver::FM_Solid);
						GraphicComponent* pGraphic = m_DebugIconEntity->GetComponent<GraphicComponent>();
						pGraphic->GetVertexBuffer()->Begin(m_DebugMaterial->GetShader());
						pGraphic->GetVertexBuffer()->Draw();
						break;
					}
				}

				// restore settings
				m_DebugMaterial->GetTexture()->Begin(0);
				GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_FillMode, GraphicsDriver::FM_Line);
			}
		}

		// render paths
		for(auto path : m_aPaths)
			RenderPost3DPath(path);

		// render selected entity at the end
		if(Entity3D* pEntity3D = DYNAMIC_CAST(ShootEditor::Instance()->GetSelectedEntity(), Entity3D))
		{
			GraphicComponent* pGraphic = pEntity3D->GetComponent<GraphicComponent>();
			VertexBuffer* pVB = pGraphic ? pGraphic->GetVertexBuffer() : NULL;
			if(pVB && !pVB->IsA(SkyBoxVertexBufferNoCubemap_NoVAO::TypeID))
			{
				bool hasInstancedVertexBuffer = false;
				if (pEntity3D->IsA(SubMesh::TypeID))				
					hasInstancedVertexBuffer = pEntity3D->GetAncestor<MeshEntity>()->GetMesh()->HasInstancedRendering();				

				if (!hasInstancedVertexBuffer)
				{
					GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, pGraphic->GetWorldTransform());
					m_DebugMaterial->GetShader()->UploadConstant<Color>("uAmbientColor", &EditorSettings::Instance()->SelectedEntityColor, 1);
					pVB->Begin(m_DebugMaterial->GetShader());
					pVB->Draw();
				}
			}
		}

		if(m_bPickingInfoValid && EditorSettings::Instance()->HasFlag(EditorSettings::EF_RenderPickingInfo))
		{
			GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, Matrix44::Identity);
			EditorRenderer::Instance()->DrawLine(m_vPickStart, m_vPickEnd, Color::Green);
			EditorRenderer::Instance()->DrawCross(m_vPickIntersection, 1.0f, Color::Green);
		}		

		// render pin
		if(ShootEditor::Instance()->HasPin())
		{
			GraphicComponent* pGraphic = m_DebugIconEntity->GetComponent<GraphicComponent>();
			m_PinTexture->Begin(0);
			m_DebugIconEntity->SetPosition(ShootEditor::Instance()->GetPinPosition());
			float fProportionalSize = (ShootEditor::Instance()->GetPinPosition() - EntityRenderer::Instance()->Get3DCamera()->GetPosition()).GetLength()*EditorSettings::Instance()->fPinSizeFactor;			
			m_DebugIconEntity->SetScale(Vector3::One*fProportionalSize);
			m_DebugIconEntity->Update();
			GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, m_DebugIconEntity->GetWorldTransform());
			GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_FillMode, GraphicsDriver::FM_Solid);			
			m_DebugMaterial->GetShader()->UploadConstant<Color>("uAmbientColor", &Color::White, 1);
			pGraphic->GetVertexBuffer()->Begin(m_DebugMaterial->GetShader());
			pGraphic->GetVertexBuffer()->Draw();

			// restore settings
			m_DebugIconEntity->SetScale(Vector3::One*EditorSettings::Instance()->fDebugIconSize);
			m_DebugIconEntity->Update();
			m_DebugMaterial->GetTexture()->Begin(0);
			GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_FillMode, GraphicsDriver::FM_Line);
		}
	}

	//! Renders collision shapes
	void EditorRenderer::RenderCollisionShapes(CollisionComponent* pCollisionComponent)
	{
		Entity3D* pParent = DYNAMIC_CAST(pCollisionComponent->GetOwner(), Entity3D);
		std::vector<Matrix44> aSphereTransforms;

		if(EnvironmentCollisionComponent* pEnvCol = DYNAMIC_CAST(pCollisionComponent, EnvironmentCollisionComponent))
		{
			if(MeshResource* pMesh = pEnvCol->GetMesh())
			{
				Vertex3D* pVertices = reinterpret_cast<Vertex3D*>(pMesh->GetSubMeshInfos()[0].m_VertexBuffer->GetVertices());
				auto vb = pMesh->GetSubMeshInfos()[0].m_VertexBuffer;
				int numIndices = vb->GetNumIndices();
				const Matrix44& matrix = static_cast<Entity3D*>(pEnvCol->GetOwner())->GetWorldTransform();

				GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, Matrix44::Identity);
				for(int i=0; i<numIndices; i += 3)
				{
					Vector3 v1 = matrix.TransformVect(pVertices[vb->GetIndex(i + 0)].Pos);
					Vector3 v2 = matrix.TransformVect(pVertices[vb->GetIndex(i + 1)].Pos);
					Vector3 v3 = matrix.TransformVect(pVertices[vb->GetIndex(i + 2)].Pos);

					EditorRenderer::Instance()->DrawLine(v1, v2, EditorSettings::Instance()->EntityPosColor);
					EditorRenderer::Instance()->DrawLine(v2, v3, EditorSettings::Instance()->EntityPosColor);
					EditorRenderer::Instance()->DrawLine(v3, v1, EditorSettings::Instance()->EntityPosColor);

					// draw the normal
					const float fNormalSize = 1.0f;
					Plane plane(v1, v2, v3);
					Vector3 vCenter = (v1 + v2 + v3)/3.0f;
					EditorRenderer::Instance()->DrawLine(vCenter, vCenter+plane.Normal*fNormalSize, EditorSettings::Instance()->EntityPosColor);
				}

				// draw collision points
				for(std::list<EnvironmentCollisionComponent::CollisionInfo>::iterator it = pEnvCol->m_aCollisionInfos.begin(); it != pEnvCol->m_aCollisionInfos.end();)
				{
					Matrix44 collisionPoint = Matrix44::Identity;
					collisionPoint.SetTranslation((*it).vPosition);
					GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, collisionPoint);
					EditorRenderer::Instance()->DrawLine(Vector3::Create(-EditorSettings::Instance()->fEntity3DPosSize/2.0f, 0.0f, 0.0f), Vector3::Create(EditorSettings::Instance()->fEntity3DPosSize/2.0f, 0.0f, 0.0f), Color::Red);
					EditorRenderer::Instance()->DrawLine(Vector3::Create(0.0f, -EditorSettings::Instance()->fEntity3DPosSize/2.0f, 0.0f), Vector3::Create(0.0f, EditorSettings::Instance()->fEntity3DPosSize/2.0f, 0.0f), Color::Red);
					EditorRenderer::Instance()->DrawLine(Vector3::Create(0.0f, 0.0f, -EditorSettings::Instance()->fEntity3DPosSize/2.0f), Vector3::Create(0.0f, 0.0f, EditorSettings::Instance()->fEntity3DPosSize/2.0f), Color::Red);
					(*it).fLife -= g_fDeltaTime;

					if((*it).fLife > 0.0f)
					{
						++it;
					}
					else
					{
						it = pEnvCol->m_aCollisionInfos.erase(it);
					}
				}
			}
		}
		else if(RigidBodyComponent* pRigidBody = DYNAMIC_CAST(pCollisionComponent, RigidBodyComponent))
		{
			for(auto &shape : pRigidBody->GetShapes())
			{
				CollisionShape* pShape = shape->Get();
				if(CollisionShapeBox* pBox = DYNAMIC_CAST(pShape, CollisionShapeBox))
				{
					Matrix44 transform = Matrix44::Identity;
					transform.Scale(pParent->GetAbsoluteScale());
					transform.Translate(pParent->GetWorldTransform().GetTranslation());
					GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, transform);
					EditorRenderer::Instance()->DrawDebugBox(pBox->GetBox(), EditorSettings::Instance()->CollisionShapeColor);
				}
				else if(CollisionShapeSphere* pSphere = DYNAMIC_CAST(pShape, CollisionShapeSphere))
				{
					Matrix44 transform = Matrix44::Identity;
					float fScale = (pParent->GetAbsoluteScale()*pSphere->GetRadius()).X;
					transform.Scale(Vector3::Create(fScale, fScale, fScale));
					transform.Translate(pSphere->GetCenter(pParent->GetWorldTransform()));
					aSphereTransforms.push_back(transform);
				}
			}
		}

		if(!aSphereTransforms.empty())
		{
			m_DebugMaterial->GetShader()->UploadConstant("uAmbientColor", &EditorSettings::Instance()->CollisionShapeColor, 1);
			m_UnitSphere->GetSubMeshInfos()[0].m_VertexBuffer->Begin(m_DebugMaterial->GetShader());
			for (size_t i=0; i<aSphereTransforms.size(); ++i)
			{
				GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, aSphereTransforms[i]);
				m_UnitSphere->GetSubMeshInfos()[0].m_VertexBuffer->Draw();
			}
		}
	}

	//! Render path
	void EditorRenderer::RenderPost3DPath(Path* pPath)
	{
		GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, Matrix44::Identity);

		std::vector<Vector3> aPoints;
		float fStep = 1.0f/EditorSettings::Instance()->fPathSmoothness;
		float k = 0.0f;
		while(k < 1.0f)
		{
			aPoints.push_back(pPath->GetPathPosition(k));
			k += fStep;
		}
		aPoints.push_back(pPath->GetPathPosition(1.0f));
		
		for(int i=0; i<int(aPoints.size())-1; ++i)
		{
			EditorRenderer::Instance()->DrawLine(aPoints[i], aPoints[i+1], EditorSettings::Instance()->PathColor);
		}
	}

	//! Render quad tree nodes
	void EditorRenderer::RenderQuadTreeNodes(QuadTreeNode* pNode)
	{
		RenderQuadTreeNode(pNode, Color::Create(.5f, .5f, .5f));
		
		for(std::list< Handle<Entity3D> >::const_iterator it = pNode->GetEntityList().begin(); 
			it != pNode->GetEntityList().end();
			++it)
		{
			if((*it).IsValid())
			{
				EditorRenderer::Instance()->DrawDebugBox((*it)->GetBoundingBox(), Color::Red);
			}
		}

		for(int i=0; i<QuadTreeNode::T_Count; ++i)
		{
			if(QuadTreeNode* pChild = pNode->GetChild((QuadTreeNode::E_NodeType)i))
			{
				RenderQuadTreeNodes(pChild);
			}
		}
	}

	//! Renders visible quad tree nodes
	void EditorRenderer::RenderVisibleQuadTreeNodes(QuadTreeNode* pNode)
	{		
		if(pNode->IsInFrustum())
		{
			for(int i=0; i<QuadTreeNode::T_Count; ++i)
			{
				if(QuadTreeNode* pChild = pNode->GetChild((QuadTreeNode::E_NodeType)i))
				{
					RenderVisibleQuadTreeNodes(pChild);
				}
			}

			if(pNode->IsLeaf())
			{
				RenderQuadTreeNode(pNode, Color::Yellow);
			}
		}
	}

	//! Renders a quad tree node
	void EditorRenderer::RenderQuadTreeNode(QuadTreeNode* pNode, const Color& color)
	{
		EditorRenderer::Instance()->DrawDebugBox(pNode->GetBBox(), color);
	}

	//! Renders a camera frustum
	void EditorRenderer::RenderFrustum(const Frustum& frustum)
	{
		GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, Matrix44::Identity);

		EditorRenderer::Instance()->DrawLine(frustum.m_NTopLeft, frustum.m_FTopLeft, Color::Yellow);					
		EditorRenderer::Instance()->DrawLine(frustum.m_NTopRight, frustum.m_FTopRight, Color::Yellow);
		EditorRenderer::Instance()->DrawLine(frustum.m_NBottomLeft, frustum.m_FBottomLeft, Color::Yellow);					
		EditorRenderer::Instance()->DrawLine(frustum.m_NBottomRight, frustum.m_FBottomRight, Color::Yellow);

		EditorRenderer::Instance()->DrawLine(frustum.m_NTopLeft, frustum.m_NTopRight, Color::Yellow);
		EditorRenderer::Instance()->DrawLine(frustum.m_NBottomLeft, frustum.m_NBottomRight, Color::Yellow);
		EditorRenderer::Instance()->DrawLine(frustum.m_NTopLeft, frustum.m_NBottomLeft, Color::Yellow);
		EditorRenderer::Instance()->DrawLine(frustum.m_NTopRight, frustum.m_NBottomRight, Color::Yellow);

		EditorRenderer::Instance()->DrawLine(frustum.m_FTopLeft, frustum.m_FTopRight, Color::Yellow);
		EditorRenderer::Instance()->DrawLine(frustum.m_FBottomLeft, frustum.m_FBottomRight, Color::Yellow);
		EditorRenderer::Instance()->DrawLine(frustum.m_FTopLeft, frustum.m_FBottomLeft, Color::Yellow);
		EditorRenderer::Instance()->DrawLine(frustum.m_FTopRight, frustum.m_FBottomRight, Color::Yellow);

		// draw normals
		//for(int i=0; i<Frustum::P_Count; ++i)
		//{
		//	const Plane& plane = frustum.GetPlane((Frustum::E_Plane)i);
		//	Vector3 vCenter = plane.Normal*plane.D;
		//	EditorRenderer::Instance()->DrawLine(vCenter, vCenter+plane.Normal, Color::Yellow);
		//}
	}

	//! set picking info
	void EditorRenderer::SetPickingInfo(const Vector3& vRayStart, const Vector3& vRayDir, const Vector3& vIntersection)
	{
		m_vPickStart = vRayStart;
		m_vPickEnd = vRayStart + vRayDir*999.0f;
		m_vPickIntersection = vIntersection;
	}

	//! renders a line
	void EditorRenderer::DrawLine(const Vector3& vStart, const Vector3& vEnd, const Color& color)
	{
		m_DebugMaterial->GetShader()->UploadConstant("uAmbientColor", &color, 1);
		Vertex3D* pVertices = reinterpret_cast<Vertex3D*>(m_DebugVB->GetVertices());
		pVertices[0].Pos = Vector3::Create(vStart.X, vStart.Y, vStart.Z);		
		pVertices[1].Pos = Vector3::Create(vEnd.X, vEnd.Y, vEnd.Z);
		m_DebugVB->SetDirty(true);
		m_DebugVB->SetNumVertices(2);
		m_DebugVB->SetPrimitiveType(GraphicsDriver::PT_Line);
		m_DebugVB->Begin(m_DebugMaterial->GetShader());
		m_DebugVB->Draw();
	}

	//! renders a debug box
	void EditorRenderer::DrawDebugBox(const AABBox3D& box, const Color& color)
	{
		m_DebugMaterial->GetShader()->UploadConstant("uAmbientColor", &color, 1);
		m_DebugVB->SetPrimitiveType(GraphicsDriver::PT_LineLoop);
		m_DebugVB->SetNumVertices(5);
		Vertex3D* pVertices = reinterpret_cast<Vertex3D*>(m_DebugVB->GetVertices());
		Vector3 vMin = box.Min(), vMax = box.Max();
		pVertices[0].Pos = Vector3::Create(vMin.X, vMin.Y, vMin.Z); 
		pVertices[1].Pos = Vector3::Create(vMax.X, vMin.Y, vMin.Z); 
		pVertices[2].Pos = Vector3::Create(vMax.X, vMax.Y, vMin.Z); 
		pVertices[3].Pos = Vector3::Create(vMin.X, vMax.Y, vMin.Z); 
		pVertices[4].Pos = Vector3::Create(vMin.X, vMin.Y, vMin.Z);
		m_DebugVB->SetDirty(true);
		m_DebugVB->Begin(m_DebugMaterial->GetShader()); m_DebugVB->Draw();
		pVertices[0].Pos = Vector3::Create(vMin.X, vMin.Y, vMax.Z); 
		pVertices[1].Pos = Vector3::Create(vMax.X, vMin.Y, vMax.Z); 
		pVertices[2].Pos = Vector3::Create(vMax.X, vMax.Y, vMax.Z); 
		pVertices[3].Pos = Vector3::Create(vMin.X, vMax.Y, vMax.Z); 
		pVertices[4].Pos = Vector3::Create(vMin.X, vMin.Y, vMax.Z);
		m_DebugVB->SetDirty(true);
		m_DebugVB->Begin(m_DebugMaterial->GetShader()); m_DebugVB->Draw();
		DrawLine(Vector3::Create(vMin.X, vMin.Y, vMin.Z), Vector3::Create(vMin.X, vMin.Y, vMax.Z), color);
		DrawLine(Vector3::Create(vMax.X, vMin.Y, vMin.Z), Vector3::Create(vMax.X, vMin.Y, vMax.Z), color);
		DrawLine(Vector3::Create(vMin.X, vMax.Y, vMin.Z), Vector3::Create(vMin.X, vMax.Y, vMax.Z), color);
		DrawLine(Vector3::Create(vMax.X, vMax.Y, vMin.Z), Vector3::Create(vMax.X, vMax.Y, vMax.Z), color);
	}

	//! renders a debug box
	void EditorRenderer::DrawDebugBox(const AABBox2D& box, const Color& color)
	{
		m_DebugMaterial->GetShader()->UploadConstant("uAmbientColor", &color, 1);
		Vertex3D* pVertices = reinterpret_cast<Vertex3D*>(m_DebugVB->GetVertices());
		Vector2 vMin = box.Min();
		Vector2 vMax = box.Max();
		pVertices[0].Pos = Vector3::Create(vMin.X, vMin.Y, 0.0f);
		pVertices[1].Pos = Vector3::Create(vMax.X, vMin.Y, 0.0f);
		pVertices[2].Pos = Vector3::Create(vMax.X, vMax.Y, 0.0f);
		pVertices[3].Pos = Vector3::Create(vMin.X, vMax.Y, 0.0f);
		pVertices[4].Pos = Vector3::Create(vMin.X, vMin.Y, 0.0f);
		m_DebugVB->SetDirty(true);
		m_DebugVB->SetNumVertices(5);
		m_DebugVB->SetPrimitiveType(GraphicsDriver::PT_LineLoop);
		m_DebugVB->Begin(m_DebugMaterial->GetShader());
		m_DebugVB->Draw();
	}

	//! renders a cross
	void EditorRenderer::DrawCross(const Vector3& vOffset, float fSize, const Color& color)
	{
		DrawLine(vOffset+Vector3::Create(-fSize, 0.0f, 0.0f), vOffset+Vector3::Create(fSize, 0.0f, 0.0f), color);
		DrawLine(vOffset+Vector3::Create(0.0f, -fSize, 0.0f), vOffset+Vector3::Create(0.0f, fSize, 0.0f), color);
		DrawLine(vOffset+Vector3::Create(0.0f, 0.0f, -fSize), vOffset+Vector3::Create(0.0f, 0.0f, fSize), color);
	}	
}
