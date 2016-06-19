/* 

Amine Rehioui
Created: March 8th 2014

*/

#pragma once


#include "Entity3D.h"
#include "RigidBodyComponent.h"

namespace shoot
{
	class ProgressBar;

	//! HQ
	class HQ : public Entity3D
	{
	public:
		
		DECLARE_OBJECT(HQ, Entity3D);

		//! constructor
		HQ();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		void Init();

		//! called during the update of the entity
		void Update();

		//! called when the HQ has taken damage
		bool OnDamage(float fDamage);

		//! returns the collision component
		inline RigidBodyComponent* GetCollisionComponent() const { return m_Collision; }

		//! GetHitPoints
		inline float GetHitPoints() const { return m_fHitPoints; }

	private:

		Handle<Entity3D> m_HealthBG;
		Handle<ProgressBar> m_HealthBar;
		Handle<RigidBodyComponent> m_Collision;
		float m_fHitPoints;
		float m_fExplosionTimer;

		// properties
		float m_fMaxHitPoints;
		float m_fExplosionIntensity;
		float m_fExplosionDuration;
		Reference<Object> m_ExplosionVisitor;
		Reference<Entity3D> m_ExplosionEntity;
	};
}




