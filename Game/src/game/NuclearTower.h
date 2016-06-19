/* 

Amine Rehioui
Created: May the 4th be with you 2014

*/

#pragma once


#include "Tower.h"

namespace shoot
{
	class ProgressIndicator;

	//! NuclearTower
	class NuclearTower : public Tower
	{
	public:

		//! State
		enum E_State
		{
			S_Idle,
			S_PreLaunch,
			S_Ascending,
			S_Striking
		};

		//! Macro to provide type information
		DECLARE_OBJECT(NuclearTower, Tower);

		//! constructor
		NuclearTower();

		//! destructor
		virtual ~NuclearTower();
				
		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		void Init();

		//! called during the update of the entity
		void Update();

		//! Nuke
		void Nuke();

		//! called when a missile has landed
		void OnMissileLanded(const Vector3& vPosition);

		//! GetState
		inline E_State GetState() const { return m_eState; }

		//! CanManuallyLaunch
		inline bool CanManuallyLaunch() { return m_eState == S_Idle || m_CanManuallyLaunch; }

	private:		

		//! HandleTriggerEvent
		void HandleTriggerEvent(Event* pEvent);

		bool m_bActive;
		float m_fTimer;
		E_State m_eState;
		Vector3 m_StrikeSrcPos;
		Vector3 m_StrikeTargetPos;
		Vector3 m_MissileDirection;
		Vector3 m_MissileTargetDirection;
		Handle<Entity3D> m_CurrentMissile;
		float m_Interpolator;
		float m_LaunchDelay;
		Handle<ProgressIndicator> m_ProgressIndicator;
		bool m_CanManuallyLaunch;

		// properties
		Reference<Entity> m_Missile;
		Reference<Entity3D> m_Explosion;
	};	
}




