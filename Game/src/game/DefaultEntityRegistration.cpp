/* 

Amine Rehioui
Created: April 4th 2010

*/

#include "Game.h"

// Game
#include "GameManager.h"
#include "AudioManager.h"
#include "GameData.h"
#include "UIContext.h"
#include "VKButton.h"
#include "VKeyboard.h"
#include "AIManager.h"
#include "GameCamera.h"
#include "Playfield.h"
#include "FollowCamera.h"
#include "GameSettings.h"
#include "GameContext.h"
#include "LevelSelectContext.h"
#include "FPSContext.h"
#include "IAPContext.h"
#include "MenuContext.h"

// Entities
#include "Pathway.h"
#include "SpawnPoint.h"
#include "ProgressBar.h"
#include "Defender.h"
#include "BaseAttacker.h"
#include "ShootingAttacker.h"
#include "KamikazeAttacker.h"
#include "TankAttacker.h"
#include "HQ.h"
#include "WaveManager.h"
#include "GunTower.h"
#include "FlameTower.h"
#include "ElectricTower.h"
#include "NuclearTower.h"
#include "BlackHole.h"
#include "DefenderManager.h"

//! Components
#include "PathwayGraphicComponent.h"
#include "HQCollisionComponent.h"
#include "GroundCollisionComponent.h"
#include "ElectricityComponent.h"
#include "LaserComponent.h"

// Weapon
#include "PulseManager.h"
#include "LaserManager.h"
#include "PelletManager.h"
#include "BulletManagerCollisionComponent.h"
#include "LaserManagerCollisionComponent.h"

// UI
#include "WheelMenu.h"
#include "WheelMenuButton.h"
#include "UIMarker.h"
#include "UIProgressBar.h"
#include "LevelSelectButton.h"
#include "LevelSelectCamera.h"

// Effects
#include "StencilEffects.h"
#include "RangeIndicator.h"
#include "ProgressIndicator.h"

using namespace shoot;

//! Object registration function
void RegisterObjects()
{
	ObjectFactory* pFactory = ObjectFactory::Instance();

	// Game
	pFactory->Register<Actor>();
	pFactory->Register<ActorSettings>();
	pFactory->Register<TowerSettings>();
	pFactory->Register<ElectricTowerSettings>();
	pFactory->Register<NuclearTowerSettings>();
	pFactory->Register<GameManager>();	
	pFactory->Register<AudioManager>();
	pFactory->Register<SFXManager>();
	pFactory->Register<GameData>();
	pFactory->Register<UIContext>();
	pFactory->Register<VKButton>();
	pFactory->Register<VKeyboard>();
	pFactory->Register<AIManager>();
	pFactory->Register<GameCamera>();
	pFactory->Register<Playfield>();
	pFactory->Register<FollowCamera>();
	pFactory->Register<GameSettings>();
	pFactory->Register<GameContext>();
	pFactory->Register<LevelSelectContext>();
	pFactory->Register<FPSContext>();	
	pFactory->Register<IAPContext>();
	pFactory->Register<MenuContext>();	

	// Weapon
	pFactory->Register<BulletManager>();
	pFactory->Register<PulseManager>();
	pFactory->Register<LaserManager>();
	pFactory->Register<PelletManager>();	
	pFactory->Register<BulletManagerCollisionComponent>();
	pFactory->Register<LaserManagerCollisionComponent>();

	// Entities
	pFactory->Register<Pathway>();
	pFactory->Register<PathwayElement>();	
	pFactory->Register<SpawnPoint>();
	pFactory->Register<ProgressBar>();
	pFactory->Register<Defender>();	
	pFactory->Register<BaseAttacker>();
	pFactory->Register<AttackerSettings>();
	pFactory->Register<DefenderSettings>();
	pFactory->Register<ShootingAttacker>();
	pFactory->Register<ShootingAttackerSettings>();
	pFactory->Register<KamikazeAttacker>();
	pFactory->Register<KamikazeAttackerSettings>();
	pFactory->Register<TankAttacker>();
	pFactory->Register<TankAttackerSettings>();
	pFactory->Register<HQ>();
	pFactory->Register<WaveManager>();
	pFactory->Register<Tower>();
	pFactory->Register<GunTower>();
	pFactory->Register<FlameTower>();
	pFactory->Register<ElectricTower>();
	pFactory->Register<NuclearTower>();
	pFactory->Register<BlackHole>();
	pFactory->Register<DefenderManager>();
	pFactory->Register<DefenderManagerSettings>();

	// Components
	pFactory->Register<PathwayGraphicComponent>();
	pFactory->Register<HQCollisionComponent>();
	pFactory->Register<GroundCollisionComponent>();
	pFactory->Register<ElectricityComponent>();
	pFactory->Register<LaserComponent>();
	pFactory->Register<FlameCollisionComponent>();

	// UI
	pFactory->Register<WheelMenu>();
	pFactory->Register<WheelMenuButton>();
	pFactory->Register<UIMarker>();
	pFactory->Register<UIProgressBar>();
	pFactory->Register<LevelSelectButton>();
	pFactory->Register<LevelSelectCamera>();

	// Effect
	pFactory->Register<StencilEffects>();
	pFactory->Register<RangeIndicator>();
	pFactory->Register<ProgressIndicator>();	
}


