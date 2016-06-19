/* 

Amine Rehioui
Created: April 24th 2014

*/

#pragma once


#include "Tower.h"
#include "RigidBodyComponent.h"

namespace shoot
{
	class BaseAttacker;
	class Visitor;

	//! FlameCollisionComponent
	class FlameCollisionComponent : public RigidBodyComponent
	{
	public:

		DECLARE_OBJECT(FlameCollisionComponent, RigidBodyComponent);

		//! called when a collision with another collision component is detected
		void OnCollision(CollisionComponent* pOther, std::vector<CollisionShape::CollisionInfo>& infos);
	};

	//! FlameTower
	class FlameTower : public Tower
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(FlameTower, Tower);

		//! constructor
		FlameTower();
				
		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		void Init();

		//! called during the update of the entity
		void Update();

	private:
		
		Handle<Entity3D> m_Core;
		bool m_bActive;
		Handle<Entity3D> m_GroundFlash;
		Handle<GraphicComponent> m_FlameGC;
		Vector2 m_FlameUVOffset;
		Handle<Visitor> m_RecoilAnim;
		Handle<ParticleGenerator> m_Flames;

		//! Looks in a direction
		void LookAt(const Vector3& vDirection);

		//! Activates the flames
		void Activate(bool bActive);

		// properties
		float m_fAimDuration;
	};	
}




