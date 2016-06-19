/* 

Amine Rehioui
Created: December 22nd 2013

*/

#pragma once


#include "Tile.h"
#include "WheelMenu.h"
#include "Micropather/micropather.h"
#include "GameSettings.h"

namespace shoot
{
	class ProgressIndicator;
	class Defender;
	class RangeIndicator;

	//! Playfield
	class Playfield : public Entity3D, public micropather::Graph
	{
		//! Macro to provide type information
		DECLARE_OBJECT(Playfield, Entity3D);

	public:		

		//! constructor
		Playfield();

		//! destructor
		virtual ~Playfield();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		void Init();

		//! called during the update of the entity		
		void Update();

		//! returns the bounds
		inline const AABBox3D& GetBounds() const { return m_Bounds; }

		//! returns the dimension
		inline const Size& GetDimension() const { return m_Dimension; }

		//! returns the shortest path
		void GetShortestPath(const Vector3& vFrom, const Vector3& vTo, std::vector<void*>& path);

		//! returns the towers
		inline std::list< Handle<Actor> >& GetTowers() { return m_lTowers; }

		//! called when the playfield is just pressed
		bool OnJustPressed(const Vector3& vTouchPos);

		//! called on a menu action
		void OnMenuAction(const std::string& action);

		//! return the landing offset
		inline float GetLandingOffset() const { return m_fLandingOffset; }

		//! sets the selected tile
		inline void SetSelectedTile(Tile* pTile) { m_pSelectedTile = pTile; }

		//! returns the slelected tile
		inline Tile* GetSelectedTile() const { return m_pSelectedTile; }

		//! returns the tiles
		inline Tile** GetAllTiles() const { return m_ppTiles; }

		//! returns the tile collision radius squared
		inline float GetTileCollisionRadiusSq() const { return m_fTileCollisionRadiusSq; }

		//! returns actor info
		GameSettings::ActorInfo* GetPreloadedActorInfo(Actor::E_Type type);

		//! ChangeDiamonds
		inline void ChangeDiamonds(int delta) { m_Diamonds = Math::Clamp<int>(m_Diamonds + delta, 0, 999999); }

		//! HasDiamonds
		inline bool HasDiamonds(int value) const { return m_Diamonds >= value; }

		//! GetBuildableTiles
		inline const std::list<Tile*>& GetBuildableTiles() const { return m_aBuildableTiles; }

		//! GetBlockingTiles
		inline const std::list<Tile*>& GetBlockingTiles() const { return m_lBlockingTiles; }

		//! OnWheelMenuClosing
		void OnWheelMenuClosing();

		//! GetCollisionBox
		inline const AABBox3D& GetCollisionBox() const { return m_CollisionBox; }

		//! GetDraggingStarted
		inline bool GetDraggingStarted() const { return m_bDraggingStarted; }

		//! OnFPSModeChanged
		void OnFPSModeChanged(bool fpsMode);

		// micropather::Graph methods
		float LeastCostEstimate(void* stateStart, void* stateEnd);
		void AdjacentCost(void* state, std::vector< micropather::StateCost >* adjacent);
		void PrintStateInfo(void* state) { }

	private:		

		void InitTiles();
		Tile* GetTile(const Vector3& vPosition, bool bClamp = false);
		void InitTouchStartCamera(Camera* pCamera);
		void ShowWheelMenu();
		void UpdateInput();
		void UpdateSpawnAnims();
		void DeleteTiles(const Size& dimension);
		WheelMenu* GetWheelMenu() const;
		bool Get3DTouchPos(Camera* pCamera, Vector3& vPos, Vector3* vRayStart = NULL, Vector3* vRayDir = NULL) const;
		void UpdateTileInfos();
		void AddSpawnProgress(Tile* pTile, Actor* pActor);
		void SpawnActor(Tile* pTile, Actor::E_Type type, Actor::E_Level level);
		void InitTower(Actor*);
		void HandleWheelEvent(Event* pEvent);
		void Zoom(float);
		void InitVB();
		void RemoveTower(Actor* actor);

		Tile** m_ppTiles;
		Tile* m_pSelectedTile;
		std::list<Tile*> m_aBuildableTiles;
		std::list<Tile*> m_lBlockingTiles;
		bool m_bDirty;
		AABBox3D m_CollisionBox;
		float m_ZoomTimer;
		float m_WheelZoomFactor;
		bool m_PrespawnedTilesInitialized;
		Handle<Entity> m_GridEntity;

		Reference<Entity3D> m_Hexagon;
		Reference<Entity3D> m_SpawnProgress;
		Handle<Entity3D> m_Selector;
		Handle<RangeIndicator> m_RangeIndicator;
		micropather::MicroPather m_Pather;
		std::list< Handle<Actor> > m_lTowers;
		Handle<Actor> m_ActorToSpawn;
		Actor::E_Type _actorToSpawnType;
		Actor::E_Level _actorToSpawnLevel;
		bool m_bActorSpawnAllowed;

		int m_PreviousDiamonds;
		Handle<Text> m_DiamondsText;

		//! ActorToSpawnInfo
		struct ActorToSpawnInfo
		{
			ActorToSpawnInfo(SubMesh* pMesh, const Color& color) : Mesh(pMesh), OriginalColor(color) { }
			Handle<SubMesh> Mesh;
			Color OriginalColor;
		};

		std::vector<ActorToSpawnInfo> m_ActorToSpawnInfos;		

		// Input
		Vector3 m_vTouchStartPos;
		Vector3 m_vTouchPos;
		Reference<Camera> m_TouchStartCamera;
		bool m_bDraggingStarted;
		Plane m_GroundPlane;
		float m_fTouchTimer;
		float m_fTileCollisionRadiusSq;

		//! spawn anim
		struct SpawnAnim
		{
			SpawnAnim(Tile* pTile, Entity3D* pEntity, Entity3D* pConstructioEntity)
				: m_pTile(pTile)
				, m_Entity(pEntity)
				, m_fInterpolator(0.0f)
				, m_ConstructionEntity(pConstructioEntity)
			{
			}

			Tile* m_pTile;
			Handle<Entity3D> m_Entity;
			float m_fInterpolator;
			float m_fSrcPos;
			float m_fDestPos;
			Reference<Entity3D> m_ConstructionEntity;
		};

		//! SpawnProgressAnim
		struct SpawnProgressAnim
		{
			SpawnProgressAnim(Tile* pTile, Entity3D* pEntity, Entity3D* pConstructioEntity);

			Tile* m_pTile;
			Handle<Entity3D> m_Entity;
			Reference<Entity3D> m_ConstructionEntity;
			float m_fSpawnTimer;
		};

		//! PreSpawnInfo
		struct PreSpawnInfo : public ISerializableStruct
		{
			PreSpawnInfo()
				: m_ActorType(Actor::T_TowerGun)
			{
			}

			Size m_Tile;
			Actor::E_Type m_ActorType;

			void Serialize(PropertyStream& stream);
		};

		std::list<SpawnAnim> m_lSpawnAnims;
		std::list<SpawnProgressAnim> m_lSpawnProgressAnims;	

		// properties
		AABBox3D m_Bounds;
		Size m_Dimension;
		float m_fSpawnAnimOffset;
		float m_fLandingOffset;
		float m_fTowerOffset;
		Range m_rZoom;
		float m_fZoomChangeFactor;
		Vector3 m_vCloseLookAtDir;
		Vector3 m_vFarLookAtDir;
		float m_fZoomFactor;
		int m_Diamonds;
		Reference<WheelMenu> _WheelMenu;
		Array<PreSpawnInfo> m_PreSpawnInfos;
		Array<Point> m_aNonBuildableTiles;
	};
}



