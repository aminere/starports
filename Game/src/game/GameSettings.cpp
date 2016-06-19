/* 

Amine Rehioui
Created: May 27th 2014

*/

#include "Game.h"

#include "GameSettings.h"

namespace shoot
{
	DEFINE_OBJECT(GameSettings);
	DEFINE_OBJECT(DefenderSettings);
	DEFINE_OBJECT(ShootingAttackerSettings);
	DEFINE_OBJECT(KamikazeAttackerSettings);
	DEFINE_OBJECT(TankAttackerSettings);
	DEFINE_OBJECT(TowerSettings);
	DEFINE_OBJECT(ElectricTowerSettings);
	DEFINE_OBJECT(NuclearTowerSettings);
	DEFINE_OBJECT(DefenderManagerSettings);

	//! constructor
	GameSettings::GameSettings()
		// properties
		: m_fSpawnAnimDuration(0.5f)
		, m_fDragTreshold(1.0f)
		, m_fAvoidanceDist(2.0f)
		, m_fAvoidanceLateralRadius(2.0f)
		, m_fAccelZFactor(0.001f)
		, m_fSpawnDuration(2.5f)
		, m_fSpawnProgressBarOffset(1.0f)
		, m_fMinSpeedForLookAtChange(0.5f)
		, m_fSteerRayCastDist(3.0f)
		, m_PathwaySegmentsPerTile(4)
		, m_RepairCost(25)
		, m_NukeCost(100)
		, m_FireDamageDuration(1.0f)
		, m_DefenderImpactEffectMinDist(15.0f)
		, m_TowerImpactEffectMinDist(8.0f)
		, m_RangeCheckTreshold(5.0f)
		, m_MuzzleFlashDuration(.1f)
		, m_GunTowerRecoilDist(8.5f)
		, m_DefaultCrashDamage(10.0f)
		, m_PlayfieldHighlight(Color::Create(.7f, .7f, .7f, 0.5f))
	{
	}

	//! destructor
	GameSettings::~GameSettings()
	{
	}

	//! serializes the entity to/from a PropertyStream
	void GameSettings::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		
		stream.Serialize("SpawnAnimDuration", &m_fSpawnAnimDuration);
		stream.Serialize("DragTreshold", &m_fDragTreshold);
		stream.Serialize("AvoidanceDist", &m_fAvoidanceDist);
		stream.Serialize("AvoidanceLateralRadius", &m_fAvoidanceLateralRadius);
		stream.Serialize("AccelZFactor", &m_fAccelZFactor);		
		stream.Serialize("SpawnDuration", &m_fSpawnDuration);
		stream.Serialize("SpawnProgressBarOffset", &m_fSpawnProgressBarOffset);		
		stream.Serialize("MinSpeedForLookAtChange", &m_fMinSpeedForLookAtChange);
		stream.Serialize("SteerRayCastDist", &m_fSteerRayCastDist);
		stream.Serialize("PathwaySegmentsPerTile", &m_PathwaySegmentsPerTile);
		stream.Serialize("RepairCost", &m_RepairCost);
		stream.Serialize("NukeCost", &m_NukeCost);
		stream.Serialize("FireDamageDuration", &m_FireDamageDuration);
		stream.Serialize("DefenderImpactEffectMinDist", &m_DefenderImpactEffectMinDist);
		stream.Serialize("TowerImpactEffectMinDist", &m_TowerImpactEffectMinDist);
		stream.Serialize("RangeCheckTreshold", &m_RangeCheckTreshold);
		stream.Serialize("MuzzleFlashDuration", &m_MuzzleFlashDuration);
		stream.Serialize("GunTowerRecoilDist", &m_GunTowerRecoilDist);
		stream.Serialize("DefaultCrashDamage", &m_DefaultCrashDamage);		
		stream.Serialize("PlayfieldHighlight", &m_PlayfieldHighlight);
		stream.Serialize("PlayfieldHighlightAlpha", &m_PlayfieldHighlight.A);

		stream.Serialize("DefenderSettings", &m_DefenderSettings);
		stream.Serialize("GunTowerSettings1", &m_GunTower1Settings);
		stream.Serialize("GunTowerSettings2", &m_GunTower2Settings);
		stream.Serialize("GunTowerSettings3", &m_GunTower3Settings);
		stream.Serialize("ElectricTowerSettings1", &m_ElectricTower1Settings);
		stream.Serialize("ElectricTowerSettings2", &m_ElectricTower2Settings);
		stream.Serialize("ElectricTowerSettings3", &m_ElectricTower3Settings);
		stream.Serialize("FlameTowerSettings1", &m_FlameTower1Settings);
		stream.Serialize("FlameTowerSettings2", &m_FlameTower2Settings);
		stream.Serialize("FlameTowerSettings3", &m_FlameTower3Settings);
		stream.Serialize("NuclearTowerSettings", &m_NuclearTowerSettings);
		stream.Serialize("DefenderManagerSettings", &m_DefenderManagerSettings);		
		stream.Serialize("PreloadedActors", &m_aPreloadedActors);
	}	

	DefenderManagerSettings::DefenderManagerSettings()
		: m_Radius(10.0f)
		, m_SpotsPerQuarter(6)
		, m_CountPerWavePerQuarter(2)
		, m_WaveFrequency(1.0f)
		, m_SpawnDuration(2.0f)
		, m_AngleRange(20.0f)
		, m_Cooldown(60.0f)
		, m_PullBackTreshold(30.0f)
		, m_DiseappearTime(4.0f)
	{

	}

	void DefenderManagerSettings::Serialize(PropertyStream& stream)
	{
		stream.Serialize("Radius", &m_Radius);
		stream.Serialize("SpotsPerQuarter", &m_SpotsPerQuarter);
		stream.Serialize("CountPerWavePerQuarter", &m_CountPerWavePerQuarter);
		stream.Serialize("WaveFrequency", &m_WaveFrequency);
		stream.Serialize("SpawnDuration", &m_SpawnDuration);
		stream.Serialize("AngleRange", &m_AngleRange);
		stream.Serialize("Cooldown", &m_Cooldown);
		stream.Serialize("PullBackTreshold", &m_PullBackTreshold);
		stream.Serialize("DiseappearTime", &m_DiseappearTime);		
	}

	DefenderSettings::DefenderSettings()
		: m_fMinAccelFactor(1.0f)
		, m_fPullbackSteerFactor(2.0f)
		, m_fPullbackAccel(35.0f)
		, m_fPullbackSpeed(10.0f)
	{
	}
	
	void DefenderSettings::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("MinAccelFactor", &m_fMinAccelFactor);
		stream.Serialize("PullbackSteerFactor", &m_fPullbackSteerFactor);
		stream.Serialize("PullbackAccel", &m_fPullbackAccel);
		stream.Serialize("PullbackSpeed", &m_fPullbackSpeed);
	}

	//! constructor
	ShootingAttackerSettings::ShootingAttackerSettings()
		: m_fBulletDamage(30.0f)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void ShootingAttackerSettings::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("BulletDamage", &m_fBulletDamage);
	}

	//! constructor
	KamikazeAttackerSettings::KamikazeAttackerSettings()
		: m_fHQKamikazeRange(15.0f)
		, m_fKamikazeMaxSpeed(20.0f)
		, m_fKamikazeMaxAccel(50.0f)
		, m_fCrashDamage(80.0f)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void KamikazeAttackerSettings::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("HQKamikazeRange", &m_fHQKamikazeRange);
		stream.Serialize("KamikazeMaxSpeed", &m_fKamikazeMaxSpeed);
		stream.Serialize("KamikazeMaxAccel", &m_fKamikazeMaxAccel);
		stream.Serialize("CrashDamage", &m_fCrashDamage);		
	}

	//! constructor
	TankAttackerSettings::TankAttackerSettings()
		: m_fLaserDamagePerSec(5.0f)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void TankAttackerSettings::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("LaserDamagePerSec", &m_fLaserDamagePerSec);
	}

	//! constructor
	TowerSettings::TowerSettings()
		: m_fAimDuration(0.3f)
		, m_fDamagePerSecond(40.0f)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void TowerSettings::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("AimDuration", &m_fAimDuration);
		stream.Serialize("DamagePerSecond", &m_fDamagePerSecond);
	}

	//! constructor
	NuclearTowerSettings::NuclearTowerSettings()
		: m_fLaunchDelay(40.0f)
		, m_fAcceleratedLaunchDelay(10.0f)
		, m_ExplosionRadius(30.0f)
		, m_StrikeDuration(2.0f)
		, m_LookAtDampDuration(0.2f)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void NuclearTowerSettings::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("LaunchDelay", &m_fLaunchDelay);
		stream.Serialize("AcceleratedLaunchDelayhDelay", &m_fAcceleratedLaunchDelay);
		stream.Serialize("ExplosionRadius", &m_ExplosionRadius);
		stream.Serialize("StrikeDuration", &m_StrikeDuration);
		stream.Serialize("LookAtDampDuration", &m_LookAtDampDuration);
	}

	//! constructor
	ElectricTowerSettings::ElectricTowerSettings()
	{
	}

	//! serializes the entity to/from a PropertyStream
	void ElectricTowerSettings::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
	}
}

