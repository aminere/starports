/* 

Amine Rehioui
Created: August 18th 2014

*/

#include "Game.h"

#include "BlackHole.h"
#include "Quaternion.h"
#include "Basis3.h"

namespace shoot
{
	DEFINE_OBJECT(BlackHole);

	//! constructor
	BlackHole::BlackHole()
	{
	}

	//! serializes the entity to/from a PropertyStream
	void BlackHole::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("RotationDegsPerSecond", &m_RotationDegsPerSecond);
	}

	//! called during the initialization of the entity
	void BlackHole::Init()
	{
		super::Init();
				
		if(Entity* pHoles = GetChildByName("Holes"))
		{
			for (auto &hole : pHoles->GetChildren())
			{
				HoleInfo info = { Handle<Entity3D>(static_cast<Entity3D*>(hole.Get())), Random::GetFloat(0.0f, 360.0f) };
				m_aHoles.push_back(info);
				info.m_Hole->SetUseRotationMatrix(true);
			}
		}
	}

	//! called during the update of the entity		
	void BlackHole::Update()
	{
		// Make the black hole face the camera and apply rotation
		Camera* pCamera = EntityRenderer::Instance()->Get3DCamera();
		if (!pCamera)
			return;

		Vector3 vToCamera = (pCamera->GetPosition()-GetAbsolutePosition()).Normalize();
		Basis3 localBasis(Vector3::Zero, vToCamera);
		int i=0;
		for (auto &hole : m_aHoles)
		{
			Quaternion q(vToCamera, hole.m_fAngle * Math::DegToRadFactor);
			Vector3 vRotatedUp = q.Rotate(localBasis.Vertical);		
			hole.m_Hole->SetRotationMatrix(Matrix44::MakeLookAtLH(Vector3::Zero, vToCamera, vRotatedUp).GetTranspose());
			hole.m_fAngle += m_RotationDegsPerSecond[i] * g_fDeltaTime;
			++i;
		}
	}
}

