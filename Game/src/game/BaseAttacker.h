/* 

Amine Rehioui
Created: August 16th 2014

*/

#pragma once


#include "Actor.h"
#include "HQ.h"
#include "Tile.h"

namespace shoot
{
	struct AttackerInfo;
	class ProgressBar;
	class AttackerSettings;

	//! AttackerInfo
	struct AttackerInfo
	{
		Handle<HQ> m_Target;
		float m_fZOffset;
	};

	//! BaseAttacker
	class BaseAttacker : public Actor
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(BaseAttacker, Actor);

		//constructor
		BaseAttacker();

		//! destructor
		virtual ~BaseAttacker();

		//! called during the initialization of the entity
		virtual void Init();

		//! called during the update of the entity
		virtual void Update();

		//! sets the attack info
		inline void SetInfo(AttackerInfo* pInfo) { m_pInfo = pInfo; }

		//! called when the actor has been shot
		virtual bool OnShot(const Vector3& vPosition, CollisionComponent* pCollisionComponent, float damage);

		//! returns the target pos
		inline const Vector3& GetTargetPos() const { return m_vTarget; }

		//! returns target reached status
		inline bool IsTargetReached() const { return m_bTargetReached; }

		//! called when the terran has changed
		virtual void OnTerrainChanged();

		//! returns the path
		inline const std::vector<void*>& GetPath() const { return m_Path; }

		//! OnFireDamage
		void OnFireDamage();

	protected:

		void LookAt(const Vector3& vDirNorm);
		void MoveToTarget();
		void LookAtTarget();
		bool Separate(Vector3& vSeparationOut);
		void SteerToTarget();
		Handle<HQ> m_TargetHQ;
		AttackerSettings* m_AttackerSettings;

		AttackerInfo* m_pInfo;
		size_t m_CurrentTarget;
		Vector3 m_vTarget;
		Vector3 m_vToTarget;
		Vector3 m_vSpeed;
		Vector3 m_vAccel;
		bool m_bTargetReached;
		float m_fZOffset;
		Tile m_DummySteeringTileStart;
		Tile m_DummySteeringTileEnd;
		float m_FireDamageTimer;
		Handle<ParticleGenerator> m_FireDamage;

		std::vector<void*> m_Path;
		Handle<ProgressBar> m_HealthBar;
		Handle<Entity3D> m_HealthBG;
	};	
}




