/* 

Amine Rehioui
Created: January 25th 2014

*/

#include "Shoot.h"

#include "CameraSpectator.h"

namespace shoot
{
	DEFINE_OBJECT(CameraSpectator);

	//! constructor
	CameraSpectator::CameraSpectator()
		// properties
		: m_fZoomStep(0.2f)
		, m_fStrafeStep(0.2f)
		, m_fVerticalStrafeStep(0.05f)
	{
	}

	void CameraSpectator::Update()
	{
		// update look at
		if(InputManager::Instance()->IsTouchJustPressed())
		{
			m_vLastTouchPos = InputManager::Instance()->GetRawTouchState().vPosition;
		}
		else if(InputManager::Instance()->IsTouchPressed())
		{
			const float fLookStep = 0.2f;
			Vector2 vTouchPos = InputManager::Instance()->GetRawTouchState().vPosition;
			Vector2 vDeltaPos = vTouchPos - m_vLastTouchPos;
			Vector3 localLookAt = GetLookAt() - GetPosition();
			Vector3 lateralAxeRotation = localLookAt.CrossProduct(Vector3::Up).Normalize();
			Vector3 verticalAxeRotation = lateralAxeRotation.CrossProduct(localLookAt).Normalize();
			lateralAxeRotation = lateralAxeRotation * vDeltaPos.Y * Math::DegToRadFactor * fLookStep;
			verticalAxeRotation = verticalAxeRotation * vDeltaPos.X * Math::DegToRadFactor * fLookStep;			
			localLookAt.Rotate(lateralAxeRotation + verticalAxeRotation);
			SetLookAt(GetPosition() + localLookAt);

			m_vLastTouchPos = vTouchPos;
		}
		
		// move forward/backward
		if(InputManager::Instance()->IsKeyPressed(InputManager::KT_W) || InputManager::Instance()->IsKeyPressed(InputManager::KT_S))
		{
			Vector3 vDir = (GetLookAt() - GetPosition()).Normalize();
			float fDir = InputManager::Instance()->IsKeyPressed(InputManager::KT_W) ? 1.0f : -1.0f;
			vDir = vDir * m_fZoomStep * fDir;
			Translate(vDir);
			SetLookAt(GetLookAt() + vDir);
		}

		// strafe left/right
		if(InputManager::Instance()->IsKeyPressed(InputManager::KT_A) || InputManager::Instance()->IsKeyPressed(InputManager::KT_D))
		{
			Vector3 localLookAt = GetLookAt() - GetPosition();
			Vector3 lateralStrafeVect = localLookAt.CrossProduct(Vector3::Up).Normalize();
			float fDir = InputManager::Instance()->IsKeyPressed(InputManager::KT_D) ? 1.0f : -1.0f;
			lateralStrafeVect = lateralStrafeVect * fDir * m_fStrafeStep;
			Translate(lateralStrafeVect);
			SetLookAt(GetLookAt() + lateralStrafeVect);
		}

		// strafe up/down
		if(InputManager::Instance()->IsKeyPressed(InputManager::KT_Q) || InputManager::Instance()->IsKeyPressed(InputManager::KT_E))
		{
			Vector3 localLookAt = GetLookAt() - GetPosition();
			Vector3 lateralStrafeVect = localLookAt.CrossProduct(Vector3::Up).Normalize();
			Vector3 verticalStrafeVect = lateralStrafeVect.CrossProduct(localLookAt);
			float fDir = InputManager::Instance()->IsKeyPressed(InputManager::KT_Q) ? 1.0f : -1.0f;
			verticalStrafeVect = verticalStrafeVect * fDir * m_fVerticalStrafeStep;
			Translate(verticalStrafeVect);
			SetLookAt(GetLookAt() + verticalStrafeVect);
		}
	}
}

