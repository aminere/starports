/* 

Amine Rehioui
Created: May 31st 2014

*/

#include "Game.h"

#include "UIMarker.h"
#include "Basis3.h"

namespace shoot
{
	DEFINE_OBJECT(UIMarker);

	//! constructor
	UIMarker::UIMarker()
		// properties
		: m_vRadiusFactors(Vector2::Create(0.9f, 0.9f))
	{		
	}

	//! serializes the entity to/from a PropertyStream
	void UIMarker::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("RadiusFactors", &m_vRadiusFactors);
	}

	//! called during the initialization of the entity
	void UIMarker::Init()
	{
		super::Init();

		SetVisible(false);
	}

	//! called during the update of the entity		
	void UIMarker::Update()
	{
		Camera* pCamera = EntityRenderer::Instance()->Get3DCamera();
		if (!m_Target.IsValid() || !pCamera)
		{
			SetVisible(false);
			return;
		}
	
		Basis3 localBasis(pCamera->GetPosition(), pCamera->GetLookAtDirection());		
		auto localBasisPos = localBasis.Transform(m_Target->GetAbsolutePosition());		

		Vector3 vScreenPos;
		bool bInFront = pCamera->GetScreenPos(m_Target->GetAbsolutePosition(), vScreenPos);
		SHOOT_WARNING(bInFront == (localBasisPos.Y > 0.0f), "UIMarker error");
		Vector2 vScreenSize = Vector2::Create(float(pCamera->GetTargetResolution().Width), float(pCamera->GetTargetResolution().Height));
		AABBox2D screenRect(Vector2::Zero, vScreenSize);
		if(bInFront && screenRect.Contains(vScreenPos.XY))
		{
			SetVisible(false);
		}
		else
		{
			Vector2 vScreenHalf = vScreenSize/2.0f;
			Vector2 vToTarget = Vector2::Create(localBasisPos.X, -localBasisPos.Z).Normalize();
			float fXDist = Math::FAbs(vToTarget.X)*(vScreenHalf.X*m_vRadiusFactors.X);
			float fYDist = Math::FAbs(vToTarget.Y)*(vScreenHalf.Y*m_vRadiusFactors.Y);
			float fRadius = Math::SquareRoot(fXDist*fXDist + fYDist*fYDist);
			SetPosition(vScreenHalf + vToTarget*fRadius);
			float fAngle = Math::ATan2(-vToTarget.Y, vToTarget.X);
			SetRotation(fAngle*Math::RadToDegFactor);
			SetVisible(true);
		}
	}
}

