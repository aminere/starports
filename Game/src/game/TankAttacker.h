/* 

Amine Rehioui
Created: September 1st 2014

*/

#pragma once


#include "BaseAttacker.h"

namespace shoot
{
	class LaserComponent;

	//! TankAttacker
	class TankAttacker : public BaseAttacker
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(TankAttacker, BaseAttacker);

		//! constructor
		TankAttacker();

		//! destructor
		virtual ~TankAttacker();
				
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
		
		E_State m_eState;
		Handle<Entity3D> m_Cannon;
		Handle<LaserComponent> m_Laser;
		Handle<Entity3D> m_HitEntity;
		Handle<Entity> m_LaserHalo;
	};	
}




