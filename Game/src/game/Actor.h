/* 

Amine Rehioui
Created: October 10th 2011

*/

#pragma once


#include "FSM.h"

#include "MeshEntity.h"

namespace shoot
{
	//! Base ActorSettings class
	class ActorSettings : public Object
	{
	public:
		
		DECLARE_OBJECT(ActorSettings, Object);

		//! constructor
		ActorSettings();

		//! serializes the object to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		float m_fHitPoints;
		float m_Range;
	};

	//! Actor
	class Actor : public Entity3D
	{
	public:
		
		DECLARE_OBJECT(Actor, Entity3D);

		//! Actor type
		enum E_Type
		{
			T_Enemy,
			T_TowerGun,
			T_TowerFlame,
			T_TowerElectric,
			T_Nuclear,
			T_DefenderPulse,
			T_DefenderLaser,
			T_DefenderPellet,
			T_Count
		};

		//! Level
		enum E_Level
		{
			L_1,
			L_2,
			L_3,
			L_Max
		};

		//! Constructor
		Actor();

		//! Destructor
		virtual ~Actor();

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		virtual void Init();

		//! initializes the actors from an ActorSettings object
		virtual void InitActor(ActorSettings* pSettings);

		//! called during the update of the entity		
		virtual void Update();

		//! returns the actor mesh entity
		MeshEntity* GetMeshEntity() { return m_Mesh; }

		//! called when the actor has been shot
		virtual bool OnShot(const Vector3& vPosition, CollisionComponent* pCollisionComponent, float damage);

		//! sets immune status
		virtual void SetImmune(bool bImmune) { m_bImmune = bImmune; }

		//! returns the actor hit points
		inline float GetHitPoints() const { return m_fHitPoints; }

		//! SetHitPoints
		inline void SetHitPoints(float hitPoints) { m_fHitPoints = hitPoints; }

		//! returns immune status
		inline bool IsImmune() const { return m_bImmune; }

		//! returns the shake on explode status
		inline bool GetShakeOnExplode() const { return m_bShakeOnExplode; }

		//! returns the explosion scale factor
		inline float GetExplosionScaleFactor() const { return m_fExplosionScaleFactor; }

		//! returns the type
		inline E_Type GetType() const { return m_eType; }

		//! returns the level
		inline E_Level GetLevel() const { return m_eLevel; }

		//! returns the FSM
		inline FSMBase* GetFSM() const { return m_pFSM; }

		//! returns the range
		inline float GetRange() const { return m_Settings ? m_Settings->m_Range : 10.0f; }
		
		//! GetMaxHitPoints
		inline float GetMaxHitPoints() const { return m_MaxHitPoints; }	

		//! GetSettings
		inline ActorSettings* GetSettings() const { return m_Settings; }

		//! returns the look at
		inline const Vector3& GetLookAt() const { return m_vLookAt; }

		static std::vector<const char*> TypeLiterals;

	protected:

		FSMBase* m_pFSM;
		ActorSettings* m_Settings;
		Handle<MeshEntity> m_Mesh;
		float m_fHitPoints;
		float m_MaxHitPoints;
		Vector3 m_vLookAt;

		// properties
		E_Type m_eType;
		E_Level m_eLevel;
		bool m_bImmune;
		bool m_bShakeOnExplode;
		float m_fExplosionScaleFactor;		
	};
}




