/*

Amine Rehioui
Created: May 8th 2015

*/

#include "Game.h"

#include "FPSContext.h"
#include "FollowCamera.h"

namespace shoot
{
	DEFINE_OBJECT(FPSContext);

	//! serializes the entity to/from a PropertyStream
	void FPSContext::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
	}

	//! called during the initialization of the entity
	void FPSContext::Init()
	{
		super::Init();

		m_Target = static_cast<Defender*>(GetChildByName("Target"));
		auto followCam = GetChildByType<FollowCamera>();
		if (m_Target.IsValid() && followCam)
		{
			followCam->SetTarget(m_Target);
			followCam->Activate();
			followCam->SetEnabled(true);

			m_Target->LookAt(Vector3::Create(1.0f, 0.0f, 0.0f));
			m_Target->SetUseRotationMatrix(true);
			m_PreviousTargetPos = m_Target->GetPosition();
		}
	}

	//! called during the update of the entity
	void FPSContext::Update()
	{
		super::Update();

		Vector3 dir = (m_Target->GetPosition() - m_PreviousTargetPos).Normalized();
		if (dir.GetLength() > 0.0f)
		{
			m_Target->LookAt(dir);
		}
		m_PreviousTargetPos = m_Target->GetPosition();
	}
}

