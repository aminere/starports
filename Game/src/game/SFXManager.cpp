/* 

Amine Rehioui
Created: May 18th 2013

*/

#include "Game.h"

#include "ExplosionVisitor.h"
#include "SequenceVisitor.h"
#include "DelayVisitor.h"
#include "RemoveEntityVisitor.h"
#include "AudioManager.h"
#include "ParticleGenerator.h"
#include "EffectEntity.h"
#include "GameCamera.h"

namespace shoot
{
	DEFINE_OBJECT(SFXManager);

	//! constructor
	SFXManager::SFXManager()
		: m_bSkyBoxAnim(false)
		, m_fInterpolator(0.0f)
		, m_fExplosionTracker(0.0f)
		, m_bActorExplosionsEnabled(true)
		// properties
		, m_fExplosionIntensity(8.0f)
		, m_fExplosionDuration(1.3f)
		, m_fSkyBoxAnimDuration(1.5f)
		, m_fCameraShakeIntensity(.5f)
		, m_fCameraShakeFrequency(.02f)
	{
	}

	//! destructor
	SFXManager::~SFXManager()
	{
	}

	//! serializes the entity to/from a PropertyStream
	void SFXManager::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("ExplosionIntensity", &m_fExplosionIntensity);
		stream.Serialize("ExplosionDuration", &m_fExplosionDuration);
		stream.Serialize("SkyBoxAnimDuration", &m_fSkyBoxAnimDuration);
		stream.Serialize("CameraShakeIntensity", &m_fCameraShakeIntensity);
		stream.Serialize("CameraShakeFrequency", &m_fCameraShakeFrequency);
		stream.Serialize("ExplosionEntity", &m_ExplosionEntity);
		stream.Serialize("ImpactEntity", &m_ImpactEntity);
		stream.Serialize("HitEntity", &m_HitEntity);
		stream.Serialize("NoHitEntity", &m_NoHitEntity);
		stream.Serialize("ExplosionInfos", &m_ExplosionInfos);
	}

	//! called during the initialization of the entity
	void SFXManager::Init()
	{
		super::Init();

		m_Root = GetRoot();

		m_PlayerPulseManager = static_cast<PulseManager*>(GetChildByName("PlayerPulseManager"));
		m_PlayerLaserManager = static_cast<LaserManager*>(GetChildByName("PlayerLaserManager"));
		m_PlayerPelletManager = static_cast<PelletManager*>(GetChildByName("PlayerPelletManager"));
		m_EnemyPulseManager = static_cast<PulseManager*>(GetChildByName("EnemyPulseManager"));
		m_EnemyLaserManager = static_cast<LaserManager*>(GetChildByName("EnemyLaserManager"));
		m_EnemyPelletManager = static_cast<PelletManager*>(GetChildByName("EnemyPelletManager"));
		m_FireDamage = static_cast<ParticleGenerator*>(GetChildByName("FireDamage"));

		if(EffectEntity* pStencilEffectsEntity = static_cast<EffectEntity*>(GetChildByName("StencilEffectsEntity")))		
			m_StencilEffects = DYNAMIC_CAST(pStencilEffectsEntity->GetEffect(), StencilEffects);		
	
		if (Entity* pSkyBox = m_Root->GetChildByType("SkyBoxEntity"))
			m_SkyBoxMaterial = pSkyBox->GetComponent<GraphicComponent>()->GetMaterial();

		if (m_FireDamage.IsValid())
			m_FireDamage->SetActive(false);
	}

	//! called during the update of the entity
	void SFXManager::Update()
	{
		UpdateSkyBox();

		m_fExplosionTracker += g_fDeltaTime;
	}

	//! called when a bullet collided
	void SFXManager::OnBulletCollided(Bullet* pBullet)
	{
		Entity3D* pEntity = static_cast<Entity3D*>(m_NoHitEntity->Copy());
		m_Root->AddChild(pEntity);
		pEntity->SetPosition(pBullet->vPosition);
		pEntity->Init();
	}

	//! called when a laser collided
	void SFXManager::OnLaserCollided(const Vector3& vIntersection)
	{
		Entity3D* pEntity = static_cast<Entity3D*>(m_NoHitEntity->Copy());
		m_Root->AddChild(pEntity);
		pEntity->SetPosition(vIntersection);
		pEntity->Init();
	}

	//! called when damage is done
	void SFXManager::OnDamageDone(const Vector3& vPosition)
	{
		Entity3D* pEntity = static_cast<Entity3D*>(m_HitEntity->Copy());
		m_Root->AddChild(pEntity);
		pEntity->SetPosition(vPosition);
		pEntity->Init();
	}

	//! OnBulletImpact
	float SFXManager::OnBulletImpact(const Vector3& position, Object* shooter)
	{
		Entity3D* pEntity = static_cast<Entity3D*>(m_ImpactEntity->Copy());
		m_Root->AddChild(pEntity);
		pEntity->SetPosition(position);
		pEntity->Init();

		auto seq = m_ImpactEntity->GetComponent<SequenceVisitor>();
		auto delay = DYNAMIC_CAST(seq->GetVisitor(0), DelayVisitor);
		return delay->GetDuration();
	}	

	//! called when an actor is destroyed
	void SFXManager::OnActorDestroyed(Actor* pActor)
	{
		float duration = 0.0f;
		if (m_bActorExplosionsEnabled)
			duration = Explode(pActor);
		else
		{
			Object* pBaseExplosionVisitor = GetExplosionVisitor(pActor->GetTemplatePath());
			if (pBaseExplosionVisitor)
			{
				ExplosionVisitor* pExplosionVisitor = static_cast<ExplosionVisitor*>(pBaseExplosionVisitor->Copy());
				duration = (pExplosionVisitor->GetDuration() < 0.0f) ? m_fExplosionDuration : pExplosionVisitor->GetDuration();
				pExplosionVisitor->SetDefaultIntensity(m_fExplosionIntensity);
				pExplosionVisitor->SetDefaultDuration(m_fExplosionDuration);
				pActor->GetMeshEntity()->AddComponent(pExplosionVisitor);
			}
		}

		DelayVisitor* pDelay = snew DelayVisitor();
		pDelay->SetDuration(duration);
		SequenceVisitor* pSequence = snew SequenceVisitor();
		pSequence->AddVisitor(pDelay);
		pSequence->AddVisitor(snew RemoveEntityVisitor());
		pActor->AddComponent(pSequence);
	}

	//! Makes an Actor explode
	float SFXManager::Explode(Actor* pActor)
	{
		Object* pBaseExplosionVisitor = GetExplosionVisitor(pActor->GetTemplatePath());
		if(!pBaseExplosionVisitor)
			return 0.0f;		

		ExplosionVisitor* pExplosionVisitor = static_cast<ExplosionVisitor*>(pBaseExplosionVisitor->Copy());
		Entity3D* pExplosionEntity = static_cast<Entity3D*>(m_ExplosionEntity->Copy());		

		for (size_t i = 0; i<pActor->GetMeshEntity()->GetComponentCount(); ++i)
		{
			if(Visitor* pVisitor = DYNAMIC_CAST(pActor->GetMeshEntity()->GetComponent(i), Visitor))
				pVisitor->Leave();
		}

		for (size_t i = 0; i<pActor->GetMeshEntity()->GetChildCount(); ++i)
		{
			Entity* pChild = pActor->GetMeshEntity()->GetChild(i);
			if(!pChild->IsA(SubMesh::TypeID) 
				&& !pChild->IsA(ParticleGenerator::TypeID)
				&& pChild->GetName() != "Explosion")
			{
				pActor->GetMeshEntity()->GetChild(i)->SetVisible(false);
			}
		}

		// stop particle generation
		std::vector< Handle<ParticleGenerator> > particleEmitters;
		pActor->GetChildrenByType<ParticleGenerator>(particleEmitters);
		for (size_t i=0; i<particleEmitters.size(); ++i)
			particleEmitters[i]->SetActive(false);

		// add explosion visitor
		pExplosionVisitor->SetDefaultIntensity(m_fExplosionIntensity);
		pExplosionVisitor->SetDefaultDuration(m_fExplosionDuration);
		pActor->GetMeshEntity()->AddComponent(pExplosionVisitor);

		// add explosion entity
		pExplosionEntity->SetScale(Vector3::One * pActor->GetExplosionScaleFactor());
		pActor->AddChild(pExplosionEntity);
		pExplosionEntity->SetPosition(pActor->GetMeshEntity()->GetPosition());

		AUDIOMGR->Play(AudioManager::S_Explosion);

		float fExplosionDuration = (pExplosionVisitor->GetDuration() < 0.0f) ? m_fExplosionDuration : pExplosionVisitor->GetDuration();

		// add camera shake
		auto camera = EntityRenderer::Instance()->Get3DCamera();
		if (camera == GAMECAM)
		{
			bool bMultipleExplosionsDetected = (m_fExplosionTracker < m_fExplosionDuration);
			bool bForceShake = true; // bIsPlayer || bIsBoss || pActor->GetShakeOnExplode();
			float fForceShakeFactor = 2.0f;
			if (bMultipleExplosionsDetected || bForceShake)
			{
				float fIntFactor = bForceShake ? fForceShakeFactor : 1.0f;
				float fFreqFactor = bForceShake ? (1.0f / fForceShakeFactor) : 1.0f;
				GAMECAM->Shake(m_fCameraShakeIntensity*fIntFactor, m_fCameraShakeFrequency*fFreqFactor, fExplosionDuration / 2.0f);
			}
		}		

		m_fExplosionTracker = 0.0f;
		pActor->SetRotation(Vector3::Zero);
		pActor->SetRotationMatrix(Matrix44::Identity);

		//WaveSimulator::Instance()->AddShockWave(pActor->GetAbsolutePosition(), m_fExplosionWaveIntensity);
		return fExplosionDuration;
	}

	//! animates the skybox color
	void SFXManager::SetSkyBoxColor(const Color& color)
	{
		m_SrcSkyBoxColor = m_SkyBoxMaterial->GetColor();
		m_DestSkyBoxColor = color;
		m_fInterpolator = 0.0f;
		m_bSkyBoxAnim = true;
	}

	//! returns the ExplosionVisitor corresponding to a template
	Object* SFXManager::GetExplosionVisitor(const std::string& templatePath)
	{
		for (size_t i=0; i<m_ExplosionInfos.GetSize(); ++i)
		{
			for (size_t j = 0; j<m_ExplosionInfos[i].m_TemplatePaths.GetSize(); ++j)
			{
				if(m_ExplosionInfos[i].m_TemplatePaths[j] == templatePath)
				{
					return m_ExplosionInfos[i].m_Explosion;
				}
			}
		}

		return NULL;
	}	

	//! updates the skybox
	void SFXManager::UpdateSkyBox()
	{
		if(m_bSkyBoxAnim)
		{
			if(m_fInterpolator < 1.0f)
			{
				Color color = Math::Lerp<Color>(m_SrcSkyBoxColor, m_DestSkyBoxColor, m_fInterpolator);
				m_SkyBoxMaterial->SetColor(color);
				m_fInterpolator += g_fDeltaTime/m_fSkyBoxAnimDuration;

			}
			else
			{
				m_SkyBoxMaterial->SetColor(m_DestSkyBoxColor);
				m_bSkyBoxAnim = false;
			}
		}
	}

	//! clears all effects
	void SFXManager::Clear()
	{
		m_PlayerPulseManager->Clear();
		m_PlayerLaserManager->Clear();
		m_PlayerPelletManager->Clear();
		m_PlayerSidePulseManager->Clear();
		m_PlayerSideLaserManager->Clear();
		m_PlayerSidePelletManager->Clear();
		m_EnemyPulseManager->Clear();
		m_EnemyLaserManager->Clear();
		m_EnemyPelletManager->Clear();
	}
}
