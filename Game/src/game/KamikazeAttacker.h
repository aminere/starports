/* 

Amine Rehioui
Created: August 28th 2014

*/

#pragma once


#include "BaseAttacker.h"

namespace shoot
{
	//! KamikazeAttacker
	class KamikazeAttacker : public BaseAttacker
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(KamikazeAttacker, BaseAttacker);

		//! constructor
		KamikazeAttacker();
				
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
			S_PreparingAttack,
			S_AttackingHQ,
			S_Idle
		};

		E_State m_eState;
		float m_fDecelTimer;
	};	
}



