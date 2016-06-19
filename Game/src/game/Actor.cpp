/* 

Amine Rehioui
Created: October 10th 2011

*/

#include "Game.h"

#include "MeshEntity.h"

#include "GameManager.h"
#include "AIManager.h"

namespace shoot
{
	// Type literals
	const char* const g_strActorTypeLiterals[] =
	{
		"T_Enemy",
		"T_TowerGun",
		"T_TowerFlame",
		"T_TowerElectric",
		"T_Nuclear",
		"T_DefenderPulse",
		"T_DefenderLaser",
		"T_DefenderPellet",
		0
	};

	std::vector<const char*> Actor::TypeLiterals(g_strActorTypeLiterals, g_strActorTypeLiterals+Actor::T_Count+1);

	const char* const g_strActorLevelLiterals[] =
	{
		"L_1",
		"L_2",
		"L_3",
		0
	};

	DEFINE_OBJECT(Actor);
	DEFINE_OBJECT(ActorSettings);

	//! constructor
	ActorSettings::ActorSettings()
		: m_fHitPoints(100.0f)
		, m_Range(10.0f)
	{
	}

	//! serializes the object to/from a PropertyStream
	void ActorSettings::Serialize(PropertyStream& stream)
	{
		stream.Serialize("HitPoints", &m_fHitPoints);
		stream.Serialize("Range", &m_Range);
	}

	//! Constructor
	Actor::Actor()
		: m_pFSM(NULL)
		, m_fHitPoints(100.0f)
		, m_Settings(NULL)
		, m_vLookAt(Vector3::Create(1.0f, 0.0f, 0.0f))
		// properties
		, m_eType(T_Enemy)
		, m_eLevel(L_1)
		, m_bImmune(false)
		, m_bShakeOnExplode(false)
		, m_fExplosionScaleFactor(1.0f)
	{
	}

	//! Destructor
	Actor::~Actor()
	{
		sdelete(m_pFSM);
	}

	//! serializes the entity to/from a PropertyStream
	void Actor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("ActorType", &m_eType, ENUM_PARAMS1(g_strActorTypeLiterals));
		stream.Serialize("ActorLevel", &m_eLevel, ENUM_PARAMS1(g_strActorLevelLiterals));
		stream.Serialize("Immune", &m_bImmune);
		stream.Serialize("ShakeOnExplode", &m_bShakeOnExplode);
		stream.Serialize("ExplosionScaleFactor", &m_fExplosionScaleFactor);
	}

	//! called during the initialization of the entity
	void Actor::Init()		
	{
		super::Init();

		m_Mesh = static_cast<MeshEntity*>(GetChildByName("BaseMesh"));

		if(m_Settings)
			m_fHitPoints = m_Settings->m_fHitPoints;

		m_MaxHitPoints = m_fHitPoints;

		if(m_pFSM)
			m_pFSM->Init();
	}

	//! initializes the actors from an ActorSettings object
	void Actor::InitActor(ActorSettings* pSettings)
	{
		m_Settings = pSettings;

		Init();
	}

	void Actor::Update()
	{
		if(m_pFSM)
			m_pFSM->Update();
	}
	
	//! called when the actor has been shot
	bool Actor::OnShot(const Vector3& vPosition, CollisionComponent* pCollisionComponent, float damage)
	{
		if (m_fHitPoints < 0.0f)
		{
			SHOOT_ASSERT(false, "Destroying an actor twice");
		}

		if(!m_bImmune)
		{
			m_fHitPoints -= damage;
			if(m_fHitPoints < 0.0f)
			{
				SFXMGR->OnActorDestroyed(this);
				AIMGR->OnActorDestroyed(this);
				return true;
			}
		}

		return false;
	}	
}

