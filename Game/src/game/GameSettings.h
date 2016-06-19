/* 

Amine Rehioui
Created: May 27th 2014

*/

#pragma once


#include "AttackerSettings.h"

namespace shoot
{
	//! DefendersSettings
	class DefenderManagerSettings : public Object
	{
		DECLARE_OBJECT(DefenderManagerSettings, Object);

	public:

		DefenderManagerSettings();
		void Serialize(PropertyStream& stream);

		float m_Radius;
		int m_SpotsPerQuarter;
		int m_CountPerWavePerQuarter;
		float m_WaveFrequency;
		float m_SpawnDuration;
		float m_AngleRange;
		float m_Cooldown;
		float m_PullBackTreshold;
		float m_DiseappearTime;
	};

	//! Defender settings
	class DefenderSettings : public AttackerSettings
	{
		DECLARE_OBJECT(DefenderSettings, AttackerSettings);

		DefenderSettings();
		void Serialize(PropertyStream& stream);

		float m_fMinAccelFactor;
		float m_fPullbackSteerFactor;
		float m_fPullbackAccel;
		float m_fPullbackSpeed;
	};

	//! ShootingAttacker settings
	class ShootingAttackerSettings : public AttackerSettings
	{
		DECLARE_OBJECT(ShootingAttackerSettings, AttackerSettings);

	public:

		//! constructor
		ShootingAttackerSettings();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		float m_fBulletDamage;
	};

	//! KamikazeAttackerSettings
	class KamikazeAttackerSettings : public AttackerSettings
	{
		DECLARE_OBJECT(KamikazeAttackerSettings, AttackerSettings);

	public:

		//! constructor
		KamikazeAttackerSettings();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		float m_fHQKamikazeRange;
		float m_fKamikazeMaxSpeed;
		float m_fKamikazeMaxAccel;
		float m_fCrashDamage;
	};

	//! TankAttackerSettings
	class TankAttackerSettings : public AttackerSettings
	{
		DECLARE_OBJECT(TankAttackerSettings, AttackerSettings);

	public:

		//! constructor
		TankAttackerSettings();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		float m_fLaserDamagePerSec;
	};

	//! TowerSettings
	class TowerSettings : public ActorSettings
	{
		DECLARE_OBJECT(TowerSettings, ActorSettings);

	public:

		//! constructor
		TowerSettings();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		float m_fAimDuration;
		float m_fDamagePerSecond;
	};

	//! ElectricTowerSettings
	class ElectricTowerSettings : public TowerSettings
	{
		DECLARE_OBJECT(ElectricTowerSettings, TowerSettings);

	public:

		//! constructor
		ElectricTowerSettings();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);
	};

	//! NuclearTowerSettings
	class NuclearTowerSettings : public ActorSettings
	{
		DECLARE_OBJECT(NuclearTowerSettings, ActorSettings);

	public:

		//! constructor
		NuclearTowerSettings();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		float m_fLaunchDelay;
		float m_fAcceleratedLaunchDelay;
		float m_ExplosionRadius;
		float m_StrikeDuration;
		float m_LookAtDampDuration;
	};

	//! GameSettings
	class GameSettings : public Entity
	{
		DECLARE_OBJECT(GameSettings, Entity);

	public:

		//! constructor
		GameSettings();

		//! destructor
		~GameSettings();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! ActorInfo
		struct ActorInfo : public ISerializableStruct
		{
			struct LevelInfo : public ISerializableStruct
			{
				LevelInfo()
					: Diamonds(0)
				{
				}

				Reference<Actor> m_Actor;
				Reference<Texture> UpgradeIcon;
				int Diamonds;

				void Serialize(PropertyStream& stream)
				{
					stream.Serialize("Actor", &m_Actor);
					stream.Serialize("Diamonds", &Diamonds);
					stream.Serialize("UpgradeIcon", &UpgradeIcon);
				}
			};

			Actor::E_Type Type;
			Reference<Texture> Icon;
			Array<LevelInfo> Levels;

			ActorInfo()
				: Type(Actor::T_TowerGun)
			{
			}

			void Serialize(PropertyStream& stream)
			{
				stream.Serialize("Type", &Type, ENUM_PARAMS1(Actor::TypeLiterals.data()));
				stream.Serialize("Icon", &Icon);
				stream.Serialize("Levels", &Levels);
			}
		};

		// properties
		float m_fDragTreshold;
		float m_fSpawnAnimDuration;
		float m_fAvoidanceDist;
		float m_fAvoidanceLateralRadius;
		float m_fAccelZFactor;
		float m_fSpawnDuration;
		float m_fSpawnProgressBarOffset;
		float m_fMinSpeedForLookAtChange;
		float m_fSteerRayCastDist;
		int m_PathwaySegmentsPerTile;
		int m_RepairCost;
		int m_NukeCost;
		float m_FireDamageDuration;
		float m_DefenderImpactEffectMinDist;
		float m_TowerImpactEffectMinDist;
		float m_RangeCheckTreshold;
		float m_MuzzleFlashDuration;
		float m_GunTowerRecoilDist;
		float m_DefaultCrashDamage;
		Color m_PlayfieldHighlight;
		Reference<DefenderSettings> m_DefenderSettings;
		Reference<TowerSettings> m_GunTower1Settings;
		Reference<TowerSettings> m_GunTower2Settings;
		Reference<TowerSettings> m_GunTower3Settings;
		Reference<ElectricTowerSettings> m_ElectricTower1Settings;
		Reference<ElectricTowerSettings> m_ElectricTower2Settings;
		Reference<ElectricTowerSettings> m_ElectricTower3Settings;
		Reference<TowerSettings> m_FlameTower1Settings;
		Reference<TowerSettings> m_FlameTower2Settings;
		Reference<TowerSettings> m_FlameTower3Settings;
		Reference<NuclearTowerSettings> m_NuclearTowerSettings;
		Reference<DefenderManagerSettings> m_DefenderManagerSettings;
		Array<ActorInfo> m_aPreloadedActors;
	};
}



