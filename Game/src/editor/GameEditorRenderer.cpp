/* 

Amine Rehioui
Created: October 15th 2011

*/

#include "GameEditorCommon.h"

#include "GameEditorRenderer.h"
#include "GameEditor.h"

#include "Playfield.h"
#include "BulletManager.h"
#include "Light.h"
#include "SpawnPoint.h"
#include "WaveManager.h"
#include "Defender.h"
#include "HQ.h"
#include "WheelMenu.h"
#include "Tower.h"
#include "FlameTower.h"
#include "GameSettings.h"

#include "EditorSettings.h"
#include "Quaternion.h"
#include "MaterialProvider.h"

namespace shoot
{
	extern Vector3 g_vHexOffsets[6];

	//! constructor
	GameEditorRenderer::GameEditorRenderer()
		: m_PreviousDimension(-1, -1)
		, m_bPlayfieldDirty(true)
	{
	}

	//! destructor
	GameEditorRenderer::~GameEditorRenderer()
	{
	}

	//! Init
	void GameEditorRenderer::Init()
	{
		super::Init();

		Material::CreationInfo info;
		info.SetShader("common/shaders/tex_col.xml");
		info.AddTexture("../EditorData/WhiteBG.xml");
		info.SetFlag(Material::MF_AlphaBlending, true);
		m_ColorDebugMaterial = MaterialProvider::Instance()->GetMaterial(info);

		// Init the unit circle vertex buffer
		m_UnitCircle = GraphicsDriver::Instance()->CreateVertexBuffer();
		m_UnitCircle->SetPrimitiveType(GraphicsDriver::PT_Line);
		{
			const int numVertices = 100;
			const float fAngleStep = Math::PI*2.0f/float(numVertices);
			float fAngle = 0;
			Vertex3D* pVertices = snew Vertex3D[numVertices];
			for(int i=0; i<numVertices; ++i)
			{
				pVertices[i].Pos = Vector3::Create(Math::Cos(fAngle), Math::Sin(fAngle), 0.0f);
				pVertices[i].UV = Vector2::Zero;
				fAngle += fAngleStep;
			}
			m_UnitCircle->SetVertices(pVertices, numVertices, Vertex3D::VF_Pos|Vertex3D::VF_UV);
		}

		// debug icons
		m_DebugIconMap[SpawnPoint::TypeID] = ResourceManager::Instance()->GetResource<Texture>("../EditorData/DebugIcons/spawner.xml");
		m_DebugIconMap[Light::TypeID] = ResourceManager::Instance()->GetResource<Texture>("../EditorData/DebugIcons/Light.xml");
	}

	//! registers an entity for post rendering
	void GameEditorRenderer::RegisterEntity(Entity* pEntity)
	{
		super::RegisterEntity(pEntity);

		if(BulletManager* pBulletManager = DYNAMIC_CAST(pEntity, BulletManager))
		{
			m_aBulletManagers.push_back(pBulletManager);
		}
		else if(Defender* pDefender = DYNAMIC_CAST(pEntity, Defender))
		{
			m_aDefenders.push_back(pDefender);
		}
		else if(WheelMenu* pWheelMenu = DYNAMIC_CAST(pEntity, WheelMenu))
		{
			m_aWheelMenus.push_back(pWheelMenu);
		}
		else if(Tower* pTower = DYNAMIC_CAST(pEntity, Tower))
		{
			m_aTowers.push_back(pTower);
		}
	}

	//! custom Post 3D rendering
	void GameEditorRenderer::RenderPost3D()
	{
		super::RenderPost3D();
				
		auto gameContext = GameEngine::Instance()->GetGameContext();
		auto pPlayfield = gameContext ? gameContext->m_Playfield.Get() : NULL;
		if(pPlayfield)
		{
			if(!m_PlayfieldVB.IsValid() 
			|| pPlayfield->GetDimension() != m_PreviousDimension
			|| m_bPlayfieldDirty)
			{
				m_PlayfieldVB = GraphicsDriver::Instance()->CreateVertexBuffer();
				m_PlayfieldVB->SetPrimitiveType(GraphicsDriver::PT_Line);

				int numTiles = pPlayfield->GetDimension().Width*pPlayfield->GetDimension().Height;
				int maxVertices = numTiles*6*2;
				Vertex3D* pVertices = snew Vertex3D[maxVertices];
				int currentVertex = 0;
				const Size& dimension = pPlayfield->GetDimension();
				for(int i=0; i<dimension.Height; ++i)
				{
					int curRowSize = (i%2 == 0) ? dimension.Width : dimension.Width-1;
					for(int j=0; j<curRowSize; ++j)
					{
						auto buildable = pPlayfield->GetAllTiles()[i][j].m_bBuildable;
						for(int k=0; k<12; ++k)
							pVertices[currentVertex+k].color = buildable ? Color::Yellow : Color::White*.7f;

						Vector3 vPos = pPlayfield->GetAllTiles()[i][j].m_vPosition;
						pVertices[currentVertex++].Pos = vPos+g_vHexOffsets[0]; pVertices[currentVertex++].Pos = vPos+g_vHexOffsets[1];
						pVertices[currentVertex++].Pos = vPos+g_vHexOffsets[0]; pVertices[currentVertex++].Pos = vPos+g_vHexOffsets[2];
						pVertices[currentVertex++].Pos = vPos+g_vHexOffsets[1]; pVertices[currentVertex++].Pos = vPos+g_vHexOffsets[3];
						pVertices[currentVertex++].Pos = vPos+g_vHexOffsets[2]; pVertices[currentVertex++].Pos = vPos+g_vHexOffsets[4];
						pVertices[currentVertex++].Pos = vPos+g_vHexOffsets[5]; pVertices[currentVertex++].Pos = vPos+g_vHexOffsets[3];
						pVertices[currentVertex++].Pos = vPos+g_vHexOffsets[5]; pVertices[currentVertex++].Pos = vPos+g_vHexOffsets[4];
					}
				}

				m_PlayfieldVB->SetVertices(pVertices, maxVertices, Vertex3D::VF_Pos|Vertex3D::VF_UV|Vertex3D::VF_Color);
				m_PlayfieldVB->SetNumVertices(currentVertex);
				m_PreviousDimension = pPlayfield->GetDimension();
				m_bPlayfieldDirty = false;
			}

			GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, pPlayfield->GetWorldTransform());
			m_ColorDebugMaterial->GetShader()->Begin();
			m_ColorDebugMaterial->GetShader()->UploadConstant<Color>("uAmbientColor", &Color::Create(.7f, .7f, .7f, 1.0f), 1);
			m_PlayfieldVB->Begin(m_ColorDebugMaterial->GetShader());
			m_PlayfieldVB->Draw();
			m_DebugMaterial->GetShader()->Begin();

			if(Tile* pTile = pPlayfield->GetSelectedTile())
			{
				for(int i=0; i<Tile::NT_Count; ++i)
				{
					if(!pTile->m_aNeighBours[i])
						continue;

					Vector3 vPos = pTile->m_aNeighBours[i]->m_vPosition;
					DrawCross(vPos, 1.0f, Color::Yellow);
				}
			}
		}

		// render bullets collision shapes
		if(EditorSettings::Instance()->HasFlag(EditorSettings::EF_CollisionShapes))
		{
			m_DebugMaterial->GetShader()->UploadConstant<Color>("uAmbientColor", &EditorSettings::Instance()->CollisionShapeColor, 1);
			m_UnitSphere->GetSubMeshInfos()[0].m_VertexBuffer->Begin(m_DebugMaterial->GetShader());
			for (size_t i=0; i<m_aBulletManagers.size(); ++i)
			{
				MemoryPool<Bullet>& bullets = m_aBulletManagers[i]->GetBullets();
				for (size_t i = 0; i < bullets.GetCapacity(); ++i)				
				{
					Bullet* pBullet = bullets.GetElement(i);
					if (!pBullet)
						continue;

					Matrix44 transform = Matrix44::Identity;
					transform.Scale(Vector3::One*pBullet->fRadius);
					transform.Translate(pBullet->vPosition);
					GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, transform);
					m_UnitSphere->GetSubMeshInfos()[0].m_VertexBuffer->Draw();
				}
			}
		}

		// pathfinding info
		auto waveManager = gameContext ? gameContext->m_WaveManager.Get() : NULL;
		if(waveManager)
		{
			GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, Matrix44::Identity);
			auto& attackers = waveManager->GetAttackers();
			for(auto it = attackers.begin(); it != attackers.end(); ++it)
			{
				if(auto attacker = (*it).Get())
				{
					const auto& path = attacker->GetPath();
					for (size_t i = 0; i + 1<path.size(); ++i)
					{
						Vector3 vPos1 = static_cast<Tile*>(path[i])->m_vPosition;
						Vector3 vPos2 = static_cast<Tile*>(path[i+1])->m_vPosition;
						EditorRenderer::Instance()->DrawLine(vPos1, vPos2, Color::Green);
					}

					EditorRenderer::Instance()->DrawCross(attacker->GetTargetPos(), 0.5f, Color::Yellow);
				}
			}
		}

		// defender info
		auto gameSettings = gameContext ? gameContext->m_GameSettings.Get() : NULL;
		if(m_aDefenders.size() && gameSettings)
		{
			for (size_t i=0; i<m_aDefenders.size(); ++i)
			{
				Defender* pDefender = m_aDefenders[i];

				// range
				Matrix44 transform = Matrix44::Identity;
				transform.Scale(Vector3::Create(pDefender->GetRange(), pDefender->GetRange(), 1.0f));
				transform.Translate(pDefender->GetAbsolutePosition());
				GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, transform);
				m_DebugMaterial->GetShader()->UploadConstant<Color>("uAmbientColor", &Color::Blue, 1);
				m_UnitCircle->Begin(m_DebugMaterial->GetShader());
				m_UnitCircle->Draw();

				// avoidance info
				//GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, Matrix44::Identity);
				//Vector3 vRayDir = pDefender->GetSpeed().Normalize();
				//Vector3 vCollisionPos, vAvoidance;				
				//float fAvoidanceDist =  gameSettings->fAvoidanceDist;
				//bool bHasObtacle = pDefender->GetCollisionInfo(vCollisionPos, vAvoidance);
				//DrawLine(pDefender->GetAbsolutePosition(), pDefender->GetAbsolutePosition()+vRayDir*fAvoidanceDist, bHasObtacle ? Color::Red : Color::Green);
				//if(bHasObtacle)
				//{
				//	DrawCross(vCollisionPos, 0.5f, Color::Red);
				//	DrawLine(pDefender->GetAbsolutePosition(), pDefender->GetAbsolutePosition()+vAvoidance, Color::Red);
				//}
			}			
		}

		// tower info
		for (size_t i=0; i<m_aTowers.size(); ++i)
		{
			Tower* pTower = m_aTowers[i];

			// range
			Matrix44 transform = Matrix44::Identity;
			transform.Scale(Vector3::Create(pTower->GetRange(), pTower->GetRange(), 1.0f));
			transform.Translate(pTower->GetAbsolutePosition());
			GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, transform);
			m_DebugMaterial->GetShader()->UploadConstant<Color>("uAmbientColor", &Color::Blue, 1);
			m_UnitCircle->Begin(m_DebugMaterial->GetShader());
			m_UnitCircle->Draw();
		}		

		m_aBulletManagers.clear();
		m_aDefenders.clear();
		m_aTowers.clear();
	}

	//! custom Post 2D rendering
	void GameEditorRenderer::RenderPost2D()
	{
		super::RenderPost2D();

		for (size_t i=0; i<m_aWheelMenus.size(); ++i)
		{
			GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, m_aWheelMenus[i]->GetWorldTransform());
			EditorRenderer::Instance()->DrawDebugBox(m_aWheelMenus[i]->GetBoundingBox(), Color::Blue);

			GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, Matrix44::Identity);
			EditorRenderer::Instance()->DrawDebugBox(m_aWheelMenus[i]->GetScreenBounds(), Color::Yellow);
		}

		m_aWheelMenus.clear();
	}
}

