/* 

Amine Rehioui
Created: August 16th 2014

*/

#pragma once


#include "BaseAttacker.h"

namespace shoot
{
	//! ShootingAttacker
	class ShootingAttacker : public BaseAttacker
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(ShootingAttacker, BaseAttacker);

		//! constructor
		ShootingAttacker();

		//! destructor
		virtual ~ShootingAttacker();
				
		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		void Init();

		//! called during the update of the entity
		void Update();

	protected:

		//! State
		enum E_State
		{
			S_Moving,
			S_AttackingHQ,
			S_Idle
		};

		float m_fShootTimer;
		Handle<Entity3D> m_ShootTarget;
		E_State m_eState;

		void PickTarget();
		void AimAndShoot();

		// properties
		float m_fBulletSpeed;
		float m_fBulletLife;
		Vector3 m_vBulletOffset;
		float m_fBulletSize;
		float m_fBulletFrequency;
	};	
}




