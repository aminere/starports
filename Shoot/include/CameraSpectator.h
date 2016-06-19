/* 

Amine Rehioui
Created: January 25th 2014

*/

#pragma once


#include "Camera.h"

namespace shoot
{
	//! CameraSpectator	
	class CameraSpectator : public Camera
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(CameraSpectator, Camera);

		//! constructor
		CameraSpectator();
		
		//! called during the update of the entity
		void Update();

		//! sets the zoom step
		inline void SetZoomStep(float fStep) { m_fZoomStep = fStep; }

		//! sets the strafe step
		inline void SetStrafeStep(float fStep) { m_fStrafeStep = fStep; }

		//! sets the vertical strafe step
		inline void SetVerticalStrafeStep(float fStep) { m_fVerticalStrafeStep = fStep; }

	protected:

		Vector2 m_vLastTouchPos;

		// properties
		float m_fZoomStep;
		float m_fStrafeStep;
		float m_fVerticalStrafeStep;
	};
}



