/*

Amine Rehioui
Created: January 3rd 2014

*/

#include "Game.h"

#include "LevelSelectContext.h"
#include "Button.h"
#include "MaterialProvider.h"
#include "Text.h"
#include "Basis3.h"

#include "LevelSelectCamera.h"

namespace shoot
{
	DEFINE_OBJECT(LevelSelectContext);

	//! constructor
	LevelSelectContext::LevelSelectContext()
		: m_CurrentLevel(0)
		// properties
		, m_LineThickness(10.0f)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void LevelSelectContext::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("LineThickness", &m_LineThickness);
		stream.Serialize("LevelInfos", &m_Levels);
	}

	//! called during the initialization of the entity
	void LevelSelectContext::Init()
	{
		super::Init();

		m_TextureRenderer = EntityRenderer::Instance()->GetTextureRenderer();
		m_LevelPanel = static_cast<Entity2D*>(GetChildByName("LevelPanel"));
		m_Root = static_cast<Entity2D*>(GetChildByName("Root"));
		m_Camera = static_cast<LevelSelectCamera*>(GetChildByName("LevelSelectCamera"));
		
		if (m_Root.IsValid())
		{
			auto& buttons = m_LevelPanel->GetChildren();
			auto buttonSize = static_cast<Entity2D*>(buttons[0].Get())->GetBoundingBox().Size();

			// UV spots
			const int spotsPerDimension = 3;
			std::vector<Vector2> spots;
			Vector2 spotPos = Vector2::Zero;
			for (int i = 0; i < spotsPerDimension; ++i)
			{
				for (int j = 0; j < spotsPerDimension; ++j)
				{
					spots.push_back(spotPos);
					spotPos.X += 1.0f / (float)spotsPerDimension;
				}
				spotPos.X = 0.0f;
				spotPos.Y += 1.0f / (float)spotsPerDimension;
			}
			int curSpot = 0;

			// buttons
			for (int i = 0; i < buttons.size(); ++i)
			{
				auto button = static_cast<Entity2D*>(buttons[i].Get());
				auto image = static_cast<Entity2D*>(button->GetChildByName("BG"));
				auto pGraphic = image->GetComponent<GraphicComponent>();

				auto pVB = GraphicsDriver::Instance()->CreateVertexBuffer();
				pVB->SetName("LevelQuad");
				Vertex3D* pVertices = snew Vertex3D[4];
				pVertices[0].Pos = Vector3::Create(buttonSize.X, 0.0f, 0.0f); // Top Right
				pVertices[1].Pos = Vector3::Create(buttonSize.X, buttonSize.Y, 0.0f); // Bottom Right
				pVertices[2].Pos = Vector3::Zero; // Top Left
				pVertices[3].Pos = Vector3::Create(0.0f, buttonSize.Y, 0.0f); // Bottom Left
				
				Vector2 uvStart = spots[curSpot];
				Vector2 uvEnd = uvStart + (Vector2::One / (float)spotsPerDimension);
				curSpot = (curSpot + 1) % spots.size();

				pVertices[0].UV = Vector2::Create(uvEnd.X, uvStart.Y);
				pVertices[1].UV = Vector2::Create(uvEnd.X, uvEnd.Y);
				pVertices[2].UV = Vector2::Create(uvStart.X, uvStart.Y);    
				pVertices[3].UV = Vector2::Create(uvStart.X, uvEnd.Y);				
				pVertices[0].Normal = Vector3::Create(1.0f, 0.0f, 0.0f);
				pVertices[1].Normal = Vector3::Create(1.0f, 1.0f, 0.0f);
				pVertices[2].Normal = Vector3::Create(0.0f, 0.0f, 0.0f);
				pVertices[3].Normal = Vector3::Create(0.0f, 1.0f, 0.0f);
				pVB->SetVertices(pVertices, 4, Vertex3D::VF_Pos | Vertex3D::VF_UV | Vertex3D::VF_Normal);
				pVB->SetPrimitiveType(GraphicsDriver::PT_TriangleStrip);
				pVB->SetDynamic(true);
				pGraphic->SetVertexBuffer(pVB);

				//button->SetCommand("PLAY " + Utils::ToString(i + 1));
				auto text = static_cast<Text*>(button->GetChildByName("Level"));
				text->SetText((Utils::ToWString(i + 1)));

				// update black panel
				auto black = button->GetChildByName("Black");
				if (GAMEMGR->GetGameData()->GetCurrentLevel() < 9)
					black->SetVisible(i >= 9);
				else
					black->SetVisible(i < 9);

				// update content
				auto unlocked = i <= GAMEMGR->GetGameData()->GetLastUnlockedLevel();
				auto current = i == GAMEMGR->GetGameData()->GetCurrentLevel();
				button->GetChildByName("Lock")->SetVisible(!unlocked);
				button->GetChildByName("Play")->SetVisible(unlocked && current);
				text->SetVisible(unlocked && !current);
			}

			// lines
			auto gc = m_Root->GetComponent<GraphicComponent>();
			if (gc)
			{
				auto numLines = buttons.size()-1;
				auto numVertices = (uint)numLines * 6;
				auto vertices = snew Vertex3D[numVertices];
				uint vtx = 0;
				for (int i = 0; i+1 < buttons.size(); ++i)
				{
					auto button1 = static_cast<Entity2D*>(buttons[i].Get());
					auto button2 = static_cast<Entity2D*>(buttons[i + 1].Get());
					auto point1 = button1->GetWorldTransform().GetTranslation() + Vector3::Create(button1->GetBoundingBox().Size() / 2.0f, 0.0f);
					auto point2 = button2->GetWorldTransform().GetTranslation() + Vector3::Create(button2->GetBoundingBox().Size() / 2.0f, 0.0f);
					auto dir = (point2 - point1).Normalized();

					Basis3 localBasis(Vector3::Zero, dir);
					auto lateral = -localBasis.Lateral * m_LineThickness; // thickness
					vertices[vtx + 0].Pos = point1 + lateral;
					vertices[vtx + 1].Pos = point1 - lateral;
					vertices[vtx + 2].Pos = point2 + lateral;
					vertices[vtx + 3].Pos = point2 + lateral;
					vertices[vtx + 4].Pos = point1 - lateral;
					vertices[vtx + 5].Pos = point2 - lateral;

					vertices[vtx + 0].UV = Vector2::Create(0.0f, 0.0f);
					vertices[vtx + 1].UV = Vector2::Create(0.0f, 1.0f);
					vertices[vtx + 2].UV = Vector2::Create(1.0f, 0.0f);
					vertices[vtx + 3].UV = Vector2::Create(1.0f, 0.0f);
					vertices[vtx + 4].UV = Vector2::Create(0.0f, 1.0f);
					vertices[vtx + 5].UV = Vector2::Create(1.0f, 1.0f);

					auto unlocked = i < GAMEMGR->GetGameData()->GetLastUnlockedLevel();
					for (int j = 0; j < 6; ++j)
					{
						vertices[vtx + j].color = unlocked ? Color::White : Color::Create(Vector3::One, 0.25f);
					}

					SHOOT_ASSERT(vtx + 5 < numVertices, "VertexBuffer overflow");
					vtx += 6;
				}

				auto vb = GraphicsDriver::Instance()->CreateVertexBuffer();
				vb->SetName("Lines");
				vb->SetDynamic(true);
				vb->SetVertices(vertices, numVertices, Vertex3D::VF_Pos | Vertex3D::VF_UV | Vertex3D::VF_Color);
				gc->SetVertexBuffer(vb);
			}

			auto currentLevel = GAMEMGR->GetGameData()->GetCurrentLevel();
			if (currentLevel > (int)buttons.size() - 1)
			{
				currentLevel = (int)buttons.size() - 1;
				GAMEMGR->GetGameData()->SetCurrentLevel(currentLevel);
				if (GAMEMGR->GetGameData()->GetLastUnlockedLevel() > currentLevel)
					GAMEMGR->GetGameData()->SetLastUnlockedLevel(currentLevel);
				GAMEMGR->GetGameData()->Save();
			}

			m_Camera->SelectLevel(currentLevel);
		}

		for (auto& level : m_Levels)
			if (!level->previewContext->IsInitialized())
				level->previewContext->Init();
	}

	//! called during the update of the entity
	void LevelSelectContext::Update()
	{
		if (m_CurrentLevel < (int)m_Levels.GetSize() && m_Levels[m_CurrentLevel].previewContext.IsValid())
			m_Levels[m_CurrentLevel].previewContext->Update();


		if (GAMEMGR->IsThankYouPending())
		{
			GAMEMGR->SetState(GameManager::S_ThankYou);
			Engine::Instance()->PushContext("menu/menuthankyou.xml");
			AUDIOMGR->Play(AudioManager::M_Arpeggio);
			GAMEMGR->ResetThankYouPending();
			return;
		}

		super::Update();
	}

	//! registers the entity and its children for rendering
	void LevelSelectContext::RegisterForRendering()
	{
		super::RegisterForRendering();
		
		if (m_CurrentLevel < (int)m_Levels.GetSize() && m_Levels[m_CurrentLevel].previewContext.IsValid())
		{
			EntityRenderer::RenderToTexture rt;
			m_Levels[m_CurrentLevel].previewContext->RegisterForRendering();
		}
	}	

	//! PlayLevel
	void LevelSelectContext::PlayLevel(int level)
	{
		GAMEMGR->GetGameData()->SetCurrentLevel(level);
		GAMEMGR->GetGameData()->Save();
		auto sublevel = level % 9; // mega hack, assumes 9 sublevels per world.
		GAMEMGR->SwitchLevel(m_Levels[m_CurrentLevel].folder, sublevel);
	}

	//! UpdateCurrentLevel
	void LevelSelectContext::UpdateCurrentLevel(uint level)
	{
		auto& buttons = m_LevelPanel->GetChildren();
		for (int i = 0; i < buttons.size(); ++i)
		{
			auto black = buttons[i]->GetChildByName("Black");
			black->SetVisible(false);
			if (level == 0)
				black->SetVisible(i >= 9);
			else
				black->SetVisible(i < 9);
		}

		m_CurrentLevel = level;
		OnEnter();
	}

	//! OnEnter
	void LevelSelectContext::OnEnter()
	{
		{
			// 3D camera in preview context
			auto camera = static_cast<Camera*>(m_Levels[m_CurrentLevel].previewContext->GetChildByName("Camera"));
			camera->Activate();
		}		

		EntityRenderer::Instance()->Set2DCamera(m_Camera);		
	}
}

