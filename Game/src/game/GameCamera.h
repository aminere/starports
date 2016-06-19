/* 

Amine Rehioui
Created: December 22nd 2013

*/

#pragma once

#include "Camera.h"

#include "Event.h"

namespace shoot
{
	//! Game camera
	class GameCamera : public Camera
	{
		//! Macro to provide type information
		DECLARE_OBJECT(GameCamera, Camera);

	public:

		//! constructor
		GameCamera();

		//! destructor
		virtual ~GameCamera();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		void Init();

		//! called during the update of the entity
		void Update();

		//! sets the target lookat
		void SetTargetLookAt(const Vector3& vLookAt);

		//! Transition to this camera from another camera
		void TransitionFrom(Camera* pOther);

		//! returns animation status
		inline bool IsAnimating() const { return m_bAnimating; }

		//! shakes the camera
		void Shake(float intensity, float frequency, float duration);

		//! StopShaking
		void StopShaking();

		//! shoot event listeners
		static void HandleTriggerEvent(void* pListener, Event* pEvent);

	private:	

		bool m_bAnimating;
		float m_fTransitionTimer;
		Handle<Camera> m_TransitionSrc; 
		Reference<Camera> m_TransitionDest;

		// camera shake
		static const uint MaxNoiseSamples = 10;
		bool m_bIsShaking;
		float m_fShakeTimer;
		float m_fShakeDuration;
		float m_fInvShakeFrequency;
		float m_fShakeIntensity;
		float m_fNoiseSamplesX[MaxNoiseSamples];
		float m_fNoiseSamplesY[MaxNoiseSamples];
		float m_fNoiseIndex;
		Vector3 m_vShakeOffset;
		Vector3 m_vPreShakePosition;

		// properties
		float m_fTransitionDuration;
	};
}



