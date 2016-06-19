/* 

Amine Rehioui
Created: February 18th 2014

*/

#pragma once


#include "Actor.h"

namespace shoot
{
	class Tile;
	class BaseAttacker;
	class ProgressBar;
	class Text;

	//! Defender
	class Defender : public Actor
	{
	public:

		//! state
		enum E_State
		{
			S_Idle,
			S_Hover,
			S_PullBack,
		};

		//! types
		enum E_DefenderType
		{
			DT_Pulse,
			DT_Laser,
			DT_Pellet,
			DT_Bomber
		};

		//! Macro to provide type information
		DECLARE_OBJECT(Defender, Actor);

		//! constructor
		Defender();
				
		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		void Init();

		//! called during the update of the entity
		void Update();

		//! called when the actor has been shot
		bool OnShot(const Vector3& vPosition, CollisionComponent* pCollisionComponent, float damage);

		//! switches the state
		void SwitchState(E_State eState);

		//! returns the state
		inline E_State GetState() const { return m_eState; }

		//! returns the speed
		inline const Vector3& GetSpeed() const { return m_vCurrentSpeed; }

		//! returns collision status
		bool CollidesWith(const Vector3& vRayStart, const Vector3& vRayDir);

		//! LookAt
		void LookAt(const Vector3& vDirNorm);

	private:
		
		bool PickAttacker();
		Vector3 Separate();
		Vector3 SteerTo(const Vector3&);

		Vector3 m_vCurrentSpeed;
		Vector3 m_vSpeed;
		Vector3 m_vAccel;

		Handle<BaseAttacker> m_Target;
		E_State m_eState;		
		
		Handle<Entity3D> m_LeftCannon;
		Handle<Entity3D> m_RightCannon;
		Handle<Entity> m_MuzzleFlashLeft;
		Handle<Entity> m_MuzzleFlashRight;
		Handle<ParticleGenerator> m_RecoilLeft;
		Handle<ParticleGenerator> m_RecoilRight;
		int m_CurrentCannon;
		Vector3 m_PullBackTarget;

		float m_ImpactTimer;

		// properties
		float m_fShootAngle;
		float m_fDamagePerSecond;
		E_DefenderType m_eDefenderType;
	};	
}




