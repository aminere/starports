/* 

Amine Rehioui
Created: August 19th 2011

*/

#pragma once


#include "Camera.h"

#include <wx/stopwatch.h>

namespace shoot
{
	//! Camera manager class
	class CameraManager : public Singleton<CameraManager>
	{
		DECLARE_SINGLETON(CameraManager);

	public:

		//! control mode
		enum E_Mode
		{
			M_Spectate,
			M_Inspect
		};

		//! Strafe the camera
		void StrafeCamera(Camera* pCamera, const Vector2& vStrafeDir);

		//! Zoom the camera
		void ZoomCamera(Camera* pCamera, float fDir);

		//! Rotate the 3D camera
		void RotateCamera(Camera* pCamera, const Vector2& vRotation);

		//! change game camera control status
		inline void SetGameCameraControl(bool bEnabled) { m_bGameCameraControl = bEnabled; }

		//! return game camera control status
		inline bool GetGameCameraControl() const { return m_bGameCameraControl; }

		//! sets SetCameraPathElementInfo status
		void SetCameraPathElementInfo(bool bEnabled);

		//! updates camera control steps
		void UpdateControlSteps();

		//! check property overrides
		void CheckPropertyOverrides(Camera* pCamera);

		//! sets the control mode
		inline void SetMode(E_Mode mode) { m_eMode = mode; }

	private:
		
		bool m_bGameCameraControl;

		bool m_bSetCameraPathElementInfo;

		float m_f3DStrafeStep;
		float m_f3DZoomStep;
		wxStopWatch m_CameraStepTimer;

		E_Mode m_eMode;

		// constants
		static float m_CameraIncrStepTime;
	};
}




