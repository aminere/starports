/* 

Amine Rehioui
Created: March 31st 2014

*/

#pragma once


#include "Camera.h"
#include "Defender.h"

namespace shoot
{
	//! FollowCamera
	class FollowCamera : public Camera
	{
		//! Macro to provide type information
		DECLARE_OBJECT(FollowCamera, Camera);

	public:

		//! constructor
		FollowCamera();

		//! destructor
		virtual ~FollowCamera();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		void Init();

		//! called during the update of the entity
		void Update();

		//! sets the target
		inline void SetTarget(Actor* pTarget) { m_Target = pTarget; }

		//! gets the target
		inline Actor* GetTarget() const { return m_Target; }

		//! OnFPSModeChanged
		void OnFPSModeChanged(bool fpsMode);

		//! OnDefendersSpawned
		void OnDefendersSpawned();

		//! OnGameCameraReady
		void OnGameCameraReady();

		//! OnMissileLaunched
		void OnMissileLaunched(Entity3D* missile);

	private:	

		//! PickTarget
		Actor* PickTarget();

		//! PickMissile
		Entity3D* PickMissile();

		Handle<Actor> m_Target;
		float m_fTargetFOV;
		bool m_FollowingDefenders;
		int m_TargetIndex;
		std::list< Handle<Entity3D> > m_Missiles;
		Handle<Entity3D> m_Missile;
		bool m_MissileView;
		Vector3 m_PreviousMissilePos;

		// properties
		float m_fFollowDuration;
		float m_fPosBackOffset;
		float m_fPosHeightOffset;
		float m_fLookAtOffset;
		float m_fMissileFollowDuration;
		float m_fMissilePosBackOffset;
		float m_fMissilePosHeightOffset;		
		float m_fMissileLookAtOffset;
		float m_fMissilePosBackOffsetDescent;
	};
}



