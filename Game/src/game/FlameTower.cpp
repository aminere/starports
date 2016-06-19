/* 

Amine Rehioui
Created: April 24th 2014

*/

#include "Game.h"

#include "FlameTower.h"
#include "WaveManager.h"
#include "ParticleGenerator.h"
#include "Visitor.h"
#include "GameSettings.h"

namespace shoot
{
	DEFINE_OBJECT(FlameTower);
	DEFINE_OBJECT(FlameCollisionComponent);

	//! called when a collision with another collision component is detected
	void FlameCollisionComponent::OnCollision(CollisionComponent* pOther, std::vector<CollisionShape::CollisionInfo>& infos)
	{
		if (auto attacker = pOther->GetOwner()->GetAncestor<BaseAttacker>())
		{
			auto tower = GetOwner()->GetAncestor<FlameTower>();
			attacker->OnFireDamage();
			auto settings = (TowerSettings*)tower->GetSettings();
			if (attacker->OnShot(attacker->GetAbsolutePosition(), NULL, g_fDeltaTime*settings->m_fDamagePerSecond))
				attacker->GetMeshEntity()->GetComponent<CollisionComponent>()->SetEnabled(false);
		}
	}

	//! constructor
	FlameTower::FlameTower()
		: m_bActive(false)
		, m_FlameUVOffset(Vector2::Zero)
		// properties
		, m_fAimDuration(0.05f)
	{
		m_vLookAt = Vector3::Create(0.0f, 1.0f, 0.0f);
	}

	//! serializes the entity to/from a PropertyStream
	void FlameTower::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("AimDuration", &m_fAimDuration);
	}

	//! called during the initialization of the entity
	void FlameTower::Init()
	{
		super::Init();

		m_Core = static_cast<Entity3D*>(GetChildByName("Core_orient"));
		if(m_Core.IsValid())
		{
			m_Core->SetUseRotationMatrix(true);
			LookAt(m_vLookAt);
		}

		m_GroundFlash = static_cast<Entity3D*>(GetChildByName("GroundFlash"));
		if(m_GroundFlash.IsValid())
			m_GroundFlash->SetVisibleAndEnabled(false);

		m_Flames = static_cast<ParticleGenerator*>(GetChildByName("Flames"));

		if (m_Flames.IsValid())
			m_FlameGC = m_Flames->GetComponent<GraphicComponent>();

		m_RecoilAnim = m_Core->GetComponent<Visitor>();

		Activate(m_bActive);
	}

	//! called during the update of the entity
	void FlameTower::Update()
	{
		super::Update();
		
		bool bActive = true;
		if(m_fHitPoints < 0.0f || !WAVEMGR)
			bActive = false;

		if (bActive)
		{
			if (!CheckTarget())
				bActive = false;
		}

		if (m_bActive != bActive)
			Activate(bActive);

		if (!bActive)
		{
			m_GroundFlash->SetVisibleAndEnabled(false);
			return;
		}	

		// aim
		Vector3 vPosition = m_Core->GetAbsolutePosition();
		auto targetPos = m_Target->GetAbsolutePosition();
		Vector3 vDirection = (targetPos - vPosition).Normalize();
		m_vLookAt = Math::Damp(m_vLookAt, vDirection, g_fDeltaTime, m_fAimDuration).Normalized();
		LookAt(m_vLookAt);

		Vector3 vFlamePos = m_Flames->Entity3D::GetWorldTransform().GetTranslation();
		m_GroundFlash->SetVisibleAndEnabled(true);
		m_GroundFlash->SetAbsolutePosition(Vector3::Create(vFlamePos.XY, GetPosition().Z));

		// update flame scrolling direction
		auto flameDir = m_vLookAt.XY.Normalize();
		auto flameScreenDir = Vector2::Create(-flameDir.X, flameDir.Y);
		m_FlameUVOffset += flameScreenDir*g_fDeltaTime;
		Matrix44 textureTransform = Matrix44::Identity;
		textureTransform.Scale(Vector3::One);
		textureTransform.Rotate(Vector3::Zero);
		textureTransform.Translate(Vector3::Create(m_FlameUVOffset, 0.0f));
		m_FlameGC->SetTextureTransform(textureTransform);

		// limit flame to target
		auto toTarget = (targetPos - vFlamePos).GetLength();
		auto speed = m_Flames->GetSpeed().Max;
		auto timeToTarget = toTarget / speed; // good old V = D / t
		m_Flames->SetLife(Range(timeToTarget+.1f, timeToTarget+.1f));

		AUDIOMGR->Play(AudioManager::S_Flames, false, true);
	}	

	//! Looks in a direction
	void FlameTower::LookAt(const Vector3& vDirection)
	{
		m_Core->SetRotationMatrix(Matrix44::MakeLookAtLH(Vector3::Zero, vDirection, Vector3::Up).GetTranspose());
	}

	//! Activates the flames
	void FlameTower::Activate(bool bActive)
	{
		std::vector< Handle<ParticleGenerator> > particles;
		m_Core->GetChildrenByType<ParticleGenerator>(particles);
		for (size_t i=0; i<particles.size(); ++i)
			particles[i]->SetActive(bActive);
		m_bActive = bActive;

		if (!m_RecoilAnim.IsValid())
			return;

		if (bActive != m_RecoilAnim->IsActive())
		{
			if (bActive)
				m_RecoilAnim->Visit(m_Core);
			else
				m_RecoilAnim->Leave();
		}
	}	
}

