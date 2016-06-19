/* 

Amine Rehioui
Created: April 12th 2014

*/

#pragma once


#include "Tower.h"

namespace shoot
{
	class ParticleGenerator;
	class Visitor;

	//! GunTower
	class GunTower : public Tower
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(GunTower, Tower);

		//! constructor
		GunTower();
				
		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		void Init();

		//! called during the update of the entity
		void Update();

	private:

		//! State
		enum E_State
		{
			S_Locking,
			S_Shooting
		};
		
		Handle<Entity3D> m_Core;
		std::vector< Handle<Entity3D> > m_MuzzleFlashes;
		Handle<Entity3D> m_GroundFlash;
		Handle<ParticleGenerator> m_Particles;
		Handle<Visitor> m_RecoilAnim;
		float m_ImpactTimer;
		float m_LockTimer;
		float m_MuzzleTimer;
		E_State m_State;
		int m_CurrentMuzzleFlash;

		//! Looks in a direction
		void LookAt(const Vector3& vDirection);

		// properties
		float m_fShowRecoilThreshold;
	};	
}




