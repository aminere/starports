/* 

Amine Rehioui
Created: December 22nd 2013

*/

#include "Game.h"

#include "Playfield.h"

#include "WaveManager.h"
#include "MaterialProvider.h"
#include "GameManager.h"
#include "GameCamera.h"
#include "FollowCamera.h"
#include "ProgressIndicator.h"
#include "WheelMenu.h"
#include "Tower.h"

#include "GameSettings.h"
#include "Gamepad.h"
#include "DefenderManager.h"
#include "Text.h"
#include "TextVisitor.h"
#include "NuclearTower.h"
#include "RangeIndicator.h"
#include "EventManager.h"

namespace shoot
{
	DEFINE_OBJECT(Playfield);

	Vector3 g_vBaseHexOffsets[6] =
	{
		Vector3::Create(0.0f, 1.08f, 0.0f),
		Vector3::Create(-0.94f, 0.54f, 0.0f),
		Vector3::Create(0.94f, 0.54f, 0.0f),
		Vector3::Create(-0.94f, -0.54f, 0.0f),
		Vector3::Create(0.94f, -0.54f, 0.0f),
		Vector3::Create(0.0f, -1.08f, 0.0f),
	};

	Vector3 g_vHexOffsets[6];

	//! constructor
	Playfield::Playfield()
		: m_ppTiles(NULL)
		, m_Pather(this)
		, m_pSelectedTile(NULL)
		, m_bDraggingStarted(false)
		, m_GroundPlane(Vector3::Up, 0.0f)
		, m_fTileCollisionRadiusSq(1.0f)
		, m_bDirty(true)
		, m_PreviousDiamonds(-1)
		, m_ZoomTimer(-1.0f)
		// properties
		, m_Bounds(Vector3::Create(-50.0f, -50.0f, -999.0f), Vector3::Create(50.0f, 50.0f, 999.0f))
		, m_Dimension(3, 2)
		, m_fSpawnAnimOffset(6.0f)
		, m_fLandingOffset(1.19f)
		, m_fTowerOffset(0.25f)
		, m_rZoom(10.0f, 80.0f)
		, m_fZoomChangeFactor(1.0f)
		, m_vCloseLookAtDir(Vector3::Create(0.0f, -.6f, 1.0f))
		, m_vFarLookAtDir(Vector3::Create(0.0f, -.05f, 1.0f))
		, m_fZoomFactor(1.0f)
		, m_Diamonds(1500)
		, m_PrespawnedTilesInitialized(false)
	{
		m_aChildren.reserve(100);
	}

	//! destructor
	Playfield::~Playfield()
	{
		if (IsInitialized())
			EventManager::Instance()->UnregisterListener<WheelEvent>(this);

		DeleteTiles(m_Dimension);
	}

	//! serializes the entity to/from a PropertyStream
	void Playfield::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		
		stream.Serialize("Bounds", &m_Bounds);

		Size oldDimension = m_Dimension;
		stream.Serialize("Dimension", &m_Dimension);		
		stream.Serialize("SpawnAnimOffset", &m_fSpawnAnimOffset);
		stream.Serialize("TileCollisionRadiusSq", &m_fTileCollisionRadiusSq);
		stream.Serialize("LandingOffset", &m_fLandingOffset);
		stream.Serialize("TowerOffset", &m_fTowerOffset);
		stream.Serialize("ZoomRange", &m_rZoom);
		stream.Serialize("ZoomChangeFactor", &m_fZoomChangeFactor);
		stream.Serialize("CloseLookAtDir", &m_vCloseLookAtDir);
		stream.Serialize("FarLookAtDir", &m_vFarLookAtDir);
		stream.Serialize("ZoomFactor", &m_fZoomFactor);
		stream.Serialize("Diamonds", &m_Diamonds);

		stream.Serialize("WheelMenu", &_WheelMenu);
		stream.Serialize("PreSpawnActors", &m_PreSpawnInfos);

		if(stream.GetMode() == SM_Write)
			UpdateTileInfos();
		else
		{
			m_vCloseLookAtDir.Normalize();
			m_vFarLookAtDir.Normalize();
		}

		stream.Serialize("NonBuildableTiles", &m_aNonBuildableTiles);

		if(m_Dimension != oldDimension)
		{
			if(IsInitialized())
			{
				DeleteTiles(oldDimension);
				InitTiles();
			}
		}
	}

	//! called during the initialization of the entity
	void Playfield::Init()
	{
		super::Init();

		m_Hexagon = static_cast<Entity3D*>(ObjectManager::Instance()->FindOrLoad("ingame/hexagon.xml"));
		m_SpawnProgress = static_cast<Entity3D*>(ObjectManager::Instance()->FindOrLoad("ingame/spawnprogress.xml"));
		m_Selector = static_cast<Entity3D*>(GetChildByName("Selector"));

		// RangeIndicator
		m_RangeIndicator = GetChildByType<RangeIndicator>();
		if (!m_RangeIndicator.IsValid())
		{
			m_RangeIndicator = static_cast<RangeIndicator*>(ObjectManager::Instance()->FindOrLoad("ingame/rangeindicator.xml"));
			AddChild(m_RangeIndicator, true);
			m_RangeIndicator->Init();
		}
		m_RangeIndicator->SetVisible(false);

		auto hexagonSize = 1.0f;
		if(m_Hexagon.IsValid())
		{
			auto hexagonMesh = DYNAMIC_CAST(m_Hexagon->GetChild(0), Entity3D);
			hexagonSize = hexagonMesh->GetScale().X;
		}

		if(m_Selector.IsValid())		
			m_Selector->SetScale(Vector3::Create(hexagonSize, hexagonSize, 1.0f));		
		
		// Initialize the hex offsets
		for(int i=0; i<6; ++i)
			g_vHexOffsets[i] = g_vBaseHexOffsets[i]*hexagonSize;		

		// Init zoom level
		if(Camera* pCamera = GAMECAM)
		{
			Vector3 vInvLookAtDir = Math::Lerp(m_vFarLookAtDir, m_vCloseLookAtDir, m_fZoomFactor).Normalize();
			Vector3 vInvLookAt = vInvLookAtDir*Math::Lerp(m_rZoom.Max, m_rZoom.Min, m_fZoomFactor);
			pCamera->SetPosition(pCamera->GetLookAt() + vInvLookAt);
		}

		InitTiles();

		if(m_Selector.IsValid())
			m_Selector->SetVisible(false);

		// collision box
		if(auto ground = static_cast<Entity3D*>(GetParent()->GetChildByName("Ground")))
		{
			if (auto collision = ground->GetComponent<RigidBodyComponent>())
			{
				auto box = DYNAMIC_CAST(collision->GetShapes()[0].Get(), CollisionShapeBox);
				m_CollisionBox = box->GetBox() * ground->GetAbsoluteScale();
			}
		}

		// pre-spawn actors
		for (auto& info : m_PreSpawnInfos)
		{
			auto type = info->m_ActorType;
			auto pTile = &(m_ppTiles[info->m_Tile.Height][info->m_Tile.Width]);
			auto actorInfo = GetPreloadedActorInfo(type);
			auto newActor = static_cast<Actor*>(actorInfo->Levels[Actor::L_1].m_Actor->Copy());

			auto tower = DYNAMIC_CAST(newActor, Tower);			
			tower->SetPosition(pTile->m_vPosition + Vector3::Create(0.0f, 0.0f, 0.15f));				
			AddChild(newActor);
			InitTower(tower);

			pTile->m_Actor = newActor;
			m_lTowers.push_back(newActor);
		}

		InitVB();

		EventManager::Instance()->RegisterListener<WheelEvent>(this, (EventManager::EventHandler)&Playfield::HandleWheelEvent);

		// hacks
		auto uiContext = Engine::Instance()->GetTopContext();
		auto hackWin = uiContext->GetChildByName("Win");
		auto hackLose = uiContext->GetChildByName("Lose");
		auto hackReset = uiContext->GetChildByName("Reset");
		hackWin->SetVisibleAndEnabled(false);
		hackLose->SetVisibleAndEnabled(false);
		hackReset->SetVisibleAndEnabled(false);
		//#ifndef _RETAIL_
		//	hackWin->SetVisibleAndEnabled(true);
		//	hackLose->SetVisibleAndEnabled(true);
		//	hackReset->SetVisibleAndEnabled(true);
		//#endif

#if SHOOT_PLATFORM == SHOOT_PLATFORM_ANDROID || !defined(_RETAIL_)
		{
			// UI hacks
			auto screenWidth = GraphicsDriver::Instance()->GetViewPort().Size().X;
			auto resources = static_cast<Entity2D*>(Engine::Instance()->GetTopContext()->GetChildByName("Resources"));
			auto wavePanel = static_cast<Entity2D*>(WAVEMGR->GetChildByName("Waves"));
			auto callDown = static_cast<Entity2D*>(GetParent()->GetChildByName("CallDown"));

			auto waveWidth = screenWidth * (26.0f/100.0f);
			auto waveFactor = waveWidth / wavePanel->GetBoundingBox().Size().X;
			auto resourceWidth = screenWidth * (16.0f / 100.0f);
			auto resourceFactor = resourceWidth / resources->GetBoundingBox().Size().X;
			auto callDownWidth = screenWidth * (6.0f / 100.0f);
			auto callDownFactor = callDownWidth / callDown->GetBoundingBox().Size().X;

			wavePanel->SetScale(waveFactor);
			resources->SetScale(resourceFactor);
			callDown->SetScale(callDownFactor);

			Vector2 callDownPos;
			callDownPos.X = 6.0f + resourceWidth;
			callDownPos.Y = 4.0f * resourceFactor;
			callDown->SetPosition(callDownPos);
		}
#endif

		// HACK for release
		m_fZoomChangeFactor = 0.6f;
	}

	//! called during the update of the entity
	void Playfield::Update()
	{
		UpdateSpawnAnims();

		if(EntityRenderer::Instance()->Get3DCamera() == GAMECAM)		
			UpdateInput();

		if (!m_PrespawnedTilesInitialized)
		{
			for (auto& info : m_PreSpawnInfos)
				SFXMGR->GetStencilEffects()->OnActorSpawned(&(m_ppTiles[info->m_Tile.Height][info->m_Tile.Width]));
			m_PrespawnedTilesInitialized = true;
		}

		// update credits
		if (m_Diamonds != m_PreviousDiamonds)
		{
			if (!m_DiamondsText.IsValid())
				m_DiamondsText = static_cast<Text*>(Engine::Instance()->GetTopContext()->GetChildByName("DiamondText"));

			auto textVisitor = m_DiamondsText->GetComponent<TextVisitor>();
			textVisitor->SetText(Utils::ToWString(m_Diamonds));
			textVisitor->Visit(m_DiamondsText);
			m_PreviousDiamonds = m_Diamonds;
		}
	}

		//! returns the shortest path
	void Playfield::GetShortestPath(const Vector3& vFrom, const Vector3& vTo, std::vector<void*>& path)
	{
		path.clear();

		// TODO: optimize - if vFrom == SpawnPoint & vTo == HQ
		Tile* pTile1 = GetTile(vFrom, true);
		Tile* pTile2 = GetTile(vTo, true);
		if(pTile1 == pTile2)
			return;

		float fTotalCost;
		m_Pather.Solve(pTile1, pTile2, &path, &fTotalCost);
	}

	//! returns a tile at a position
	Tile* Playfield::GetTile(const Vector3& vPosition, bool bClamp /*= false*/)
	{
		auto pos = vPosition;
		pos.Z = 0.0f;

		if(bClamp)
		{
			// get closest, buildable tile
			Tile* pClosest = NULL;
			float fMinDistSq = Math::Maxf32;
			for(auto &tile : m_aBuildableTiles)
			{
				auto tilePos = tile->m_vPosition + Vector3::Create(GetPosition().XY, 0.0f);				
				float distSq = (tilePos - pos).GetLengthSquared();
				if(distSq < fMinDistSq)
				{
					pClosest = tile;
					fMinDistSq = distSq;
				}
			}
			return pClosest;
		}
		else
		{
			// get first colliding tile
			for(auto &tile : m_aBuildableTiles)
			{
				auto tilePos = tile->m_vPosition + Vector3::Create(GetPosition().XY, 0.0f);
				if((tilePos - pos).GetLengthSquared() < m_fTileCollisionRadiusSq)
					return tile;
			}			
			return NULL;
		}
	}

	//! called on a menu action
	void Playfield::OnMenuAction(const std::string& action)
	{
		auto actor = static_cast<Actor*>(GetWheelMenu()->GetTile()->m_Actor.Get());
		if(actor && actor->GetHitPoints() < 0.0f)
			return;

		auto tile = GetWheelMenu()->GetTile();
		if(action.find("SPAWN") != std::string::npos)
		{
			std::vector<std::string> tokens;
			Utils::Tokenize(action.c_str(), "_", tokens);
			auto actorType = (Actor::E_Type)Utils::FromString<int>(tokens[1]);
			SHOOT_ASSERT(actorType < Actor::T_Count, "Invalid actor type");
			auto actorInfo = GetPreloadedActorInfo(actorType);
			if (HasDiamonds(actorInfo->Levels[Actor::L_1].Diamonds))
			{
				SpawnActor(tile, actorType, Actor::L_1);
				GetWheelMenu()->Close();
			}
		}
		else if(action == "UPGRADE")
		{
			auto nextLevel = (Actor::E_Level)(int(actor->GetLevel()) + 1);
			SHOOT_ASSERT(nextLevel < Actor::L_Max, "Invalid Actor Level detected");
			auto actorInfo = GetPreloadedActorInfo(actor->GetType());
			if (HasDiamonds(actorInfo->Levels[nextLevel].Diamonds))
			{
				actor->GetParent()->RemoveChild(actor);
				SpawnActor(tile, actor->GetType(), nextLevel);
				GetWheelMenu()->Close();
				RemoveTower(actor);
			}
		}
		else if(action == "SELL")
		{
			actor->GetParent()->RemoveChild(actor);
			auto actorInfo = GetPreloadedActorInfo(actor->GetType());
			ChangeDiamonds(actorInfo->Levels[actor->GetLevel()].Diamonds);
			GetWheelMenu()->Close();
			RemoveTower(actor);
		}
		else if(action == "REPAIR")
		{
			if (HasDiamonds(GSETTINGS->m_RepairCost))
			{
				actor->SetHitPoints(actor->GetMaxHitPoints());
				ChangeDiamonds(-GSETTINGS->m_RepairCost);
				GetWheelMenu()->Close();
			}
		}
		else if (action == "NUKE")
		{
			if (HasDiamonds(GSETTINGS->m_NukeCost))
			{
				auto nuclearTower = DYNAMIC_CAST(actor, NuclearTower);
				nuclearTower->Nuke();
				ChangeDiamonds(-GSETTINGS->m_NukeCost);
				GetWheelMenu()->Close();
			}
		}
		else
			SHOOT_ASSERT(false, "Invalid WheelMenu action: '%s'", action.c_str());		
	}

	GameSettings::ActorInfo* Playfield::GetPreloadedActorInfo(Actor::E_Type type)
	{
		for(auto& actorInfo : GSETTINGS->m_aPreloadedActors)
		{
			if(actorInfo->Type != type)
				continue;

			return actorInfo;
		}
	
		SHOOT_ASSERT(false, "Preloaded Actor '%s' not found", Actor::TypeLiterals[type]);
		return NULL;
	}

	void Playfield::InitTiles()
	{
		m_ppTiles = snew Tile*[m_Dimension.Height];
		for(int i=0; i<m_Dimension.Height; ++i)		
			m_ppTiles[i] = snew Tile[m_Dimension.Width];		
		
		Vector2 vHexStep = Vector2::Create(g_vHexOffsets[2].X-g_vHexOffsets[1].X, -(g_vHexOffsets[0].Y+g_vHexOffsets[1].Y));
		Vector3 vTilePos = Vector3::Create(999.0f, (-vHexStep.Y*0.5f)*(m_Dimension.Height-1), 0.0f);

		for(int i=0; i<m_Dimension.Height; ++i)
		{
			int curRowSize = (i%2 == 0) ? m_Dimension.Width : m_Dimension.Width-1;
			vTilePos.X = (-vHexStep.X*0.5f) * (curRowSize-1);

			for(int j=0; j<curRowSize; ++j)
			{
				Tile& tile = m_ppTiles[i][j];

				// fill neighbors
				if(j > 0) tile.m_aNeighBours[Tile::NT_Left] = &(m_ppTiles[i][j-1]);
				if(j+1 < curRowSize) tile.m_aNeighBours[Tile::NT_Right] = &(m_ppTiles[i][j+1]);

				if(i+1 < m_Dimension.Height)
				{
					if(curRowSize < m_Dimension.Width)
					{
						tile.m_aNeighBours[Tile::NT_BottomLeft] = &(m_ppTiles[i+1][j]);
						tile.m_aNeighBours[Tile::NT_BottomRight] = &(m_ppTiles[i+1][j+1]);
					}
					else
					{
						if(j>0) tile.m_aNeighBours[Tile::NT_BottomLeft] = &(m_ppTiles[i+1][j-1]);
						if(j<curRowSize-1) tile.m_aNeighBours[Tile::NT_BottomRight] = &(m_ppTiles[i+1][j]);
					}
				}

				if(i > 0)
				{
					if(curRowSize < m_Dimension.Width)
					{
						tile.m_aNeighBours[Tile::NT_TopLeft] = &(m_ppTiles[i-1][j]);
						tile.m_aNeighBours[Tile::NT_TopRight] = &(m_ppTiles[i-1][j+1]);
					}
					else
					{
						if(j>0) tile.m_aNeighBours[Tile::NT_TopLeft] = &(m_ppTiles[i-1][j-1]);
						if(j<curRowSize-1) tile.m_aNeighBours[Tile::NT_TopRight] = &(m_ppTiles[i-1][j]);
					}
				}

				tile.m_bBuildable = true;
				m_aBuildableTiles.push_back(&tile);

				// init position
				tile.m_vPosition = vTilePos;
				vTilePos.X += vHexStep.X;
			}

			vTilePos.Y += vHexStep.Y;
		}
				
		// init buildable tiles
		for(auto &coord : m_aNonBuildableTiles)
		{
			SHOOT_ASSERT(coord->X < m_Dimension.Width && coord->Y < m_Dimension.Height, "Invalid TileCoord");
			auto& tile = m_ppTiles[coord->Y][coord->X];
			tile.m_bBuildable = false;
			auto it = std::find(m_aBuildableTiles.begin(), m_aBuildableTiles.end(), &tile);
			SHOOT_ASSERT(it != m_aBuildableTiles.end(), "Tile initialization error");
			m_aBuildableTiles.erase(it);
		}

		m_pSelectedTile = NULL;
	}

	void Playfield::InitTouchStartCamera(Camera* pCamera)
	{
		if(!m_TouchStartCamera.IsValid())
			m_TouchStartCamera = snew Camera();
		
		m_TouchStartCamera->CopyFrom(pCamera);
	}

	void Playfield::ShowWheelMenu()
	{
		auto container = GetParent()->GetChildByName("WheelMenu");
		container->Clear();
		container->AddChild(_WheelMenu, true);
		_WheelMenu->Open(m_pSelectedTile);
	}
	
	void Playfield::UpdateInput()
	{
		GameCamera* pCamera = GAMECAM;
		if(!pCamera
		|| pCamera->IsAnimating()
		|| (EntityRenderer::Instance()->Get3DCamera() != pCamera))
		{
			return;
		}

		if(InputManager::Instance()->IsTouchJustPressed())
		{
			Vector3 vRayStart, vRayDir;
			pCamera->StopShaking();
			Get3DTouchPos(pCamera, m_vTouchStartPos, &vRayStart, &vRayDir);
			InitTouchStartCamera(pCamera);
			m_bDraggingStarted = false;

			m_pSelectedTile = GetTile(m_vTouchStartPos);

			if (m_pSelectedTile && !GetWheelMenu())
			{
				auto indicatorsPos = m_pSelectedTile->m_vPosition + Vector3::Create(0.0f, 0.0f, 0.15f);
				m_Selector->SetVisible(true);
				m_Selector->SetPosition(indicatorsPos);
			}
		}
		else if(InputManager::Instance()->IsTouchPressed())
		{
			if (!m_TouchStartCamera.IsValid())
				return;

			Get3DTouchPos(m_TouchStartCamera, m_vTouchPos);
			Vector3 vDeltaPos = m_vTouchPos-m_vTouchStartPos;
			if(!m_bDraggingStarted)
			{
				if(vDeltaPos.GetLength() > GSETTINGS->m_fDragTreshold)				
					m_bDraggingStarted = true;
			}
			else
			{
				// move camera
				const AABBox3D& bounds = PLAYFIELD->GetBounds();
				Vector3 vNewLookAt = (m_TouchStartCamera->GetLookAt()-vDeltaPos).Clamp(bounds.Min(), bounds.Max());
				pCamera->SetLookAt(vNewLookAt);
				Vector3 vInvLookAtDir = Math::Lerp(m_vFarLookAtDir, m_vCloseLookAtDir, m_fZoomFactor).Normalize();
				Vector3 vInvLookAt = vInvLookAtDir*Math::Lerp(m_rZoom.Max, m_rZoom.Min, m_fZoomFactor);
				pCamera->SetPosition(pCamera->GetLookAt() + vInvLookAt);
			}				
		}
		else if(InputManager::Instance()->IsTouchJustReleased())
		{
			if (!m_TouchStartCamera.IsValid())
				return;

			if(m_bDraggingStarted)
				return;

			auto wheelMenu = GetWheelMenu();
			auto actor = m_pSelectedTile ? static_cast<Actor*>(m_pSelectedTile->m_Actor.Get()) : NULL;

			// update dirtiness & tile blocking status
			if(m_bDirty)
			{
				m_lBlockingTiles.clear();
				for(auto tile : m_aBuildableTiles)
				{
					if(tile->m_Actor.IsValid())
						continue;

					tile->m_Actor = m_Hexagon;
					m_Pather.Reset();
					tile->m_bBlocking = WAVEMGR->IsPathBlocked();
					tile->m_Actor = NULL;
					m_Pather.Reset();

					if (tile->m_bBlocking)
						m_lBlockingTiles.push_back(tile);
				}

				SFXMGR->GetStencilEffects()->OnTilesChanged();
				m_bDirty = false;
			}

			if(!m_pSelectedTile || m_pSelectedTile->m_bBlocking)
			{
				if (wheelMenu)				
					wheelMenu->Close();
				
				SFXMGR->GetStencilEffects()->ShowPlayfieldEffect();
				return;
			}
			else if((wheelMenu && wheelMenu->GetTile() == m_pSelectedTile)
				 || (actor && !actor->IsEnabled()))
			{
				if(wheelMenu)
					wheelMenu->Close();
				return;
			}
						
			if (wheelMenu)
				wheelMenu->Close();
			else
			{
				auto indicatorsPos = m_pSelectedTile->m_vPosition + Vector3::Create(0.0f, 0.0f, 0.15f);
				m_Selector->SetVisible(true);
				m_Selector->SetPosition(indicatorsPos);
				auto showRange = (actor != NULL);
				if (actor && actor->GetType() == Actor::T_Nuclear)
					showRange = false;
				m_RangeIndicator->SetVisible(showRange);
				if (actor)
				{
					m_RangeIndicator->SetPosition(indicatorsPos);
					m_RangeIndicator->SetScale(Vector3::One*actor->GetRange());
				}
				ShowWheelMenu();
			}
		}

		if(InputManager::Instance()->IsKeyPressed(InputManager::KT_Left))
		{
			Zoom(-m_fZoomChangeFactor);
		}
		else if(InputManager::Instance()->IsKeyPressed(InputManager::KT_Right))
		{
			Zoom(m_fZoomChangeFactor);
		}

		if (m_ZoomTimer > 0.0f)
		{
			Zoom(m_WheelZoomFactor);
			m_ZoomTimer -= g_fDeltaTime;
		}
	}

	void Playfield::UpdateSpawnAnims()
	{
		for(std::list<SpawnProgressAnim>::iterator it = m_lSpawnProgressAnims.begin(); it != m_lSpawnProgressAnims.end(); )
		{
			SHOOT_ASSERT((*it).m_Entity.IsValid(), "Entity destroyed before SpawnProgressAnim finished");			

			(*it).m_fSpawnTimer += g_fDeltaTime;
			if((*it).m_fSpawnTimer > GSETTINGS->m_fSpawnDuration)
			{
				// start spawn anim
				(*it).m_Entity->SetVisible(true);
				SpawnAnim anim2((*it).m_pTile, (*it).m_Entity, (*it).m_ConstructionEntity);
				anim2.m_fSrcPos = m_fSpawnAnimOffset;
				anim2.m_fDestPos = (*it).m_Entity->GetPosition().Z;
				(*it).m_Entity->SetPosition((*it).m_Entity->GetPosition() + Vector3::Create(0.0f, 0.0f, m_fSpawnAnimOffset));
				m_lSpawnAnims.push_back(anim2);

				it = m_lSpawnProgressAnims.erase(it);
			}
			else
			{
				++it;
			}
		}

		for(std::list<SpawnAnim>::iterator it = m_lSpawnAnims.begin(); it != m_lSpawnAnims.end(); )
		{
			SHOOT_ASSERT((*it).m_Entity.IsValid(), "Entity destroyed before SpawnAnim finished");
			(*it).m_fInterpolator += g_fDeltaTime/GSETTINGS->m_fSpawnAnimDuration;
			if((*it).m_fInterpolator < 1.0f)
			{
				float fFactor = (*it).m_fInterpolator;
				float posZ = Math::Lerp((*it).m_fSrcPos, (*it).m_fDestPos, fFactor*fFactor*fFactor);
				Vector3 vPos = (*it).m_Entity->GetPosition();
				vPos.Z = posZ;
				(*it).m_Entity->SetPosition(vPos);
				++it;
			}
			else
			{
				Vector3 vPos = (*it).m_Entity->GetPosition();
				vPos.Z = (*it).m_fDestPos;
				(*it).m_Entity->SetPosition(vPos);
				(*it).m_Entity->SetEnabled(true);
				(*it).m_pTile->m_bAnimating = false;
				it = m_lSpawnAnims.erase(it);
			}
		}
	}

	WheelMenu* Playfield::GetWheelMenu() const
	{
		auto container = GetParent()->GetChildByName("WheelMenu");
		if(container->GetChildren().empty())
			return NULL;
		
		return static_cast<WheelMenu*>(container->GetChild(0));
	}
	
	void Playfield::DeleteTiles(const Size& dimension)
	{
		if(!m_ppTiles)		
			return;
		
		for(int i=0; i<dimension.Height; ++i)
			sdelete_array(m_ppTiles[i]);

		if(auto hexagons = GetChildByName("Hexagons"))
			RemoveChild(hexagons, true);
		
		sdelete_array(m_ppTiles);
		m_aBuildableTiles.clear();
		m_aNonBuildableTiles.Clear();
	}

	bool Playfield::Get3DTouchPos(Camera* pCamera, Vector3& vPos, Vector3* vRayStart /*= NULL*/, Vector3* vRayDir /*= NULL*/) const
	{
		Vector3 rayStart = Vector3::Zero, rayDir = Vector3::One;
		Vector2 vTouchPos = InputManager::Instance()->GetRawTouchState().vPosition;
		pCamera->Get3DRayFromScreenPos(rayStart, rayDir, vTouchPos);
		if(vRayStart) *vRayStart = rayStart;
		if(vRayDir) *vRayDir = rayDir;
		return m_GroundPlane.IntersectWithRay(rayStart, rayDir, &vPos);
	}	

	void Playfield::UpdateTileInfos()
	{
		if(!IsInitialized())
			return;

		m_aNonBuildableTiles.Clear();
		for(int i=0; i<m_Dimension.Height; ++i)
		{
			int curRowSize = (i%2 == 0) ? m_Dimension.Width : m_Dimension.Width-1;
			for(int j=0; j<curRowSize; ++j)
			{
				if(m_ppTiles[i][j].m_bBuildable)
					continue;

				m_aNonBuildableTiles.Add(snew Point(j, i));
			}
		}
	}

	void Playfield::AddSpawnProgress(Tile* pTile, Actor* pActor)
	{
		Entity3D* pSpawnProgress = static_cast<Entity3D*>(m_SpawnProgress->Copy());
		GetRoot()->AddChild(pSpawnProgress);
		pSpawnProgress->SetPosition(pTile->m_vPosition + Vector3::Create(0.0f, 0.0f, GSETTINGS->m_fSpawnProgressBarOffset));
		pSpawnProgress->Init();
		auto indicator = DYNAMIC_CAST(pSpawnProgress, ProgressIndicator);
		indicator->SetDuration(GSETTINGS->m_fSpawnDuration);

		MeshEntity* pConstructionEntity = static_cast<MeshEntity*>(pActor->GetMeshEntity()->Copy());		
		m_lSpawnProgressAnims.push_back(SpawnProgressAnim(pTile, pActor, pConstructionEntity));

		// construction effect
		pConstructionEntity->SetPosition(pActor->GetMeshEntity()->GetAbsolutePosition());
		pConstructionEntity->Init();
		SFXMGR->GetStencilEffects()->AddConstructionEffect(pConstructionEntity);
	}

	Playfield::SpawnProgressAnim::SpawnProgressAnim(Tile* pTile, Entity3D* pEntity, Entity3D* pConstructionEntity)
		: m_pTile(pTile)
		, m_Entity(pEntity)
		, m_fSpawnTimer(0.0f)
		, m_ConstructionEntity(pConstructionEntity)
	{
	}

	void Playfield::OnWheelMenuClosing()
	{
		m_Selector->SetVisible(false);
		m_RangeIndicator->SetVisible(false);		
	}

	void Playfield::SpawnActor(Tile* pTile, Actor::E_Type type, Actor::E_Level level)
	{
		auto actorInfo = GetPreloadedActorInfo(type);
		auto newActor = static_cast<Actor*>(actorInfo->Levels[level].m_Actor->Copy());

		auto tower = DYNAMIC_CAST(newActor, Tower);
		if(tower)
		{
			tower->SetPosition(pTile->m_vPosition + Vector3::Create(0.0f, 0.0f, 0.15f));
			ChangeDiamonds(-actorInfo->Levels[level].Diamonds);
		}
		else
			SHOOT_ASSERT(false, "Can't spawn actor '%s/%d'", Actor::TypeLiterals[type], level);
		
		newActor->SetEnabled(false);
		newActor->SetVisible(false);
		AddChild(newActor);
		
		InitTower(tower);		

		pTile->m_Actor = newActor;
		m_lTowers.push_back(newActor);
		AddSpawnProgress(pTile, newActor);
		m_bDirty = true;
		m_Pather.Reset();
		pTile->m_bAnimating = true;
		WAVEMGR->OnTerrainChanged();

		SFXMGR->GetStencilEffects()->OnActorSpawned(pTile);
	}

	void Playfield::InitTower(Actor* tower)
	{
		switch (tower->GetType())
		{
		case Actor::T_TowerGun:
			if (tower->GetLevel() == Actor::L_1)
				tower->InitActor(GSETTINGS->m_GunTower1Settings);
			else if (tower->GetLevel() == Actor::L_2)
				tower->InitActor(GSETTINGS->m_GunTower2Settings);
			else 
				tower->InitActor(GSETTINGS->m_GunTower3Settings);
			break;

		case Actor::T_TowerElectric:
			if (tower->GetLevel() == Actor::L_1)
				tower->InitActor(GSETTINGS->m_ElectricTower1Settings);
			else if (tower->GetLevel() == Actor::L_2)
				tower->InitActor(GSETTINGS->m_ElectricTower2Settings);
			else
				tower->InitActor(GSETTINGS->m_ElectricTower3Settings);
			break;

		case Actor::T_TowerFlame:
			if (tower->GetLevel() == Actor::L_1)
				tower->InitActor(GSETTINGS->m_FlameTower1Settings);
			else if (tower->GetLevel() == Actor::L_2)
				tower->InitActor(GSETTINGS->m_FlameTower2Settings);
			else
				tower->InitActor(GSETTINGS->m_FlameTower3Settings);
			break;

		case Actor::T_Nuclear:
			tower->InitActor(GSETTINGS->m_NuclearTowerSettings);
			break;
		}		
	}

	//! OnFPSModeChanged
	void Playfield::OnFPSModeChanged(bool fpsMode)
	{
		m_GridEntity->SetVisible(!fpsMode);
	}

	//! Serialize
	void Playfield::PreSpawnInfo::Serialize(PropertyStream& stream)
	{
		stream.Serialize("Tile", &m_Tile);
		stream.Serialize("ActorType", &m_ActorType, ENUM_PARAMS1(Actor::TypeLiterals.data()));
	}

	void Playfield::HandleWheelEvent(Event* pEvent)
	{
		auto wheelEvent = (WheelEvent*)pEvent;
		if (wheelEvent->m_Up)
			m_WheelZoomFactor = 1.0f;
		else
			m_WheelZoomFactor = -1.0f;
		m_ZoomTimer = 0.2f;
	}

	void Playfield::Zoom(float changeFactor)
	{
		m_fZoomFactor += g_fDeltaTime*changeFactor;
		m_fZoomFactor = Math::Clamp(m_fZoomFactor, 0.0f, 1.0f);
		Vector3 vInvLookAtDir = Math::Lerp(m_vFarLookAtDir, m_vCloseLookAtDir, m_fZoomFactor).Normalize();
		Vector3 vInvLookAt = vInvLookAtDir*Math::Lerp(m_rZoom.Max, m_rZoom.Min, m_fZoomFactor);
		GAMECAM->StopShaking();
		GAMECAM->SetPosition(GAMECAM->GetLookAt() + vInvLookAt);
	}

	void Playfield::InitVB()
	{
		if (auto gc = GetComponent<GraphicComponent>())
			RemoveComponent(gc, true);		

		auto vb = GraphicsDriver::Instance()->CreateVertexBuffer();
		vb->SetPrimitiveType(GraphicsDriver::PT_Line);

		int numTiles = GetDimension().Width*GetDimension().Height;
		int maxVertices = numTiles * 6 * 2;
		Vertex3D* pVertices = snew Vertex3D[maxVertices];
		int currentVertex = 0;
		const Size& dimension = GetDimension();
		for (int i = 0; i < dimension.Height; ++i)
		{
			int curRowSize = (i % 2 == 0) ? dimension.Width : dimension.Width - 1;
			for (int j = 0; j < curRowSize; ++j)
			{
				auto buildable = GetAllTiles()[i][j].m_bBuildable;
				if (!buildable)
					continue;

				Vector3 vPos = GetAllTiles()[i][j].m_vPosition;
				pVertices[currentVertex++].Pos = vPos + g_vHexOffsets[0]; pVertices[currentVertex++].Pos = vPos + g_vHexOffsets[1];
				pVertices[currentVertex++].Pos = vPos + g_vHexOffsets[0]; pVertices[currentVertex++].Pos = vPos + g_vHexOffsets[2];
				pVertices[currentVertex++].Pos = vPos + g_vHexOffsets[1]; pVertices[currentVertex++].Pos = vPos + g_vHexOffsets[3];
				pVertices[currentVertex++].Pos = vPos + g_vHexOffsets[2]; pVertices[currentVertex++].Pos = vPos + g_vHexOffsets[4];
				pVertices[currentVertex++].Pos = vPos + g_vHexOffsets[5]; pVertices[currentVertex++].Pos = vPos + g_vHexOffsets[3];
				pVertices[currentVertex++].Pos = vPos + g_vHexOffsets[5]; pVertices[currentVertex++].Pos = vPos + g_vHexOffsets[4];
			}
		}

		vb->SetVertices(pVertices, maxVertices, Vertex3D::VF_Pos | Vertex3D::VF_UV);
		vb->SetDynamic(true);
		vb->SetNumVertices(currentVertex);

		auto gc = snew GraphicComponent();
		gc->SetRenderingPass(GraphicComponent::RP_3D);
		gc->SetRenderingPriority(4);
		Material::CreationInfo info;
		info.AddTexture("common/tex/WhiteBG.xml");
		info.SetFlag(Material::MF_AlphaBlending, true);
		info.m_Color = GSETTINGS->m_PlayfieldHighlight;
		auto material = MaterialProvider::Instance()->GetMaterial(info);
		gc->SetMaterial(material);
		gc->SetVertexBuffer(vb);

		{
			m_GridEntity = snew Entity3D();
			m_GridEntity->AddComponent(gc, true);
			AddChild(m_GridEntity, true);
			m_GridEntity->Init();
		}
	}

	void Playfield::RemoveTower(Actor* actor)
	{
		if (auto tower = DYNAMIC_CAST(actor, Tower))
		{
			auto htower = Handle<Actor>(tower);
			auto it = std::find(m_lTowers.begin(), m_lTowers.end(), htower);
			SHOOT_ASSERT(it != m_lTowers.end(), "Tower not found in list");
			m_lTowers.erase(it);
		}
	}

	// micropather::Graph methods
	float Playfield::LeastCostEstimate(void* stateStart, void* stateEnd)
	{
		Tile* pTile1 = static_cast<Tile*>(stateStart);
		Tile* pTile2 = static_cast<Tile*>(stateEnd);
		return (pTile2->m_vPosition - pTile1->m_vPosition).GetLengthSquared();
	}

	void Playfield::AdjacentCost(void* state, std::vector< micropather::StateCost >* adjacent)
	{
		Tile* pTile = static_cast<Tile*>(state);
		for(int i=0; i<Tile::NT_Count; ++i)
		{
			Tile* pNeighbor = pTile->m_aNeighBours[i];
			if(pNeighbor && !pNeighbor->m_Actor.IsValid() && pNeighbor->m_bBuildable)
			{
				micropather::StateCost neighbor;
				neighbor.state = pNeighbor;
				neighbor.cost = (pNeighbor->m_vPosition - pTile->m_vPosition).GetLengthSquared();
				adjacent->push_back(neighbor);
			}
		}
	}
}

