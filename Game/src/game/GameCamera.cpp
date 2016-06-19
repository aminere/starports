/* 

Amine Rehioui
Created: December 22nd 2013

*/

#include "Game.h"

#include "GameCamera.h"

#include "Playfield.h"
#include "FollowCamera.h"

namespace shoot
{
	DEFINE_OBJECT(GameCamera);

	//! constructor
	GameCamera::GameCamera()
		: m_bAnimating(false)
		, m_bIsShaking(false)
		// properties
		, m_fTransitionDuration(0.5f)
	{
		// camera shake
		std::vector<int> noiseSigns;
		for (uint i = 0; i < MaxNoiseSamples / 2; ++i)
		{
			noiseSigns.push_back(1);
			noiseSigns.push_back(-1);
		}
		for (uint i = 0; i < MaxNoiseSamples; ++i)
		{
			uint signIndex = Random::GetInt(0, (uint)noiseSigns.size() - 1);
			m_fNoiseSamplesX[i] = Random::GetFloat(0.01f, 0.1f) * noiseSigns[signIndex];
			m_fNoiseSamplesY[i] = Random::GetFloat(0.01f, 0.1f) * noiseSigns[signIndex];
			noiseSigns.erase(noiseSigns.begin() + signIndex);
		}
	}

	//! destructor
	GameCamera::~GameCamera()
	{
	}

	//! serializes the entity to/from a PropertyStream
	void GameCamera::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("TransitionDuration", &m_fTransitionDuration);
	}

	//! called during the initialization of the entity
	void GameCamera::Init()
	{
		if(QuadTree* pQuadTree = GetRootContext()->GetQuadTree())		
			pQuadTree->SetCamera(this);		

		m_TransitionDest = snew Camera();
		super::Init();
	}

	//! called during the update of the entity
	void GameCamera::Update()
	{
		if(m_bAnimating)
		{
			m_fTransitionTimer += g_fDeltaTime;
			if(m_fTransitionTimer < m_fTransitionDuration)
			{
				float fFactor = m_fTransitionTimer/m_fTransitionDuration;
				Vector3 vPosition = (m_TransitionDest->GetPosition() - m_TransitionSrc->GetPosition())*fFactor + m_TransitionSrc->GetPosition();
				Vector3 vLookAt = (m_TransitionDest->GetLookAt() - m_TransitionSrc->GetLookAt())*fFactor + m_TransitionSrc->GetLookAt();
				float fFOV = (m_TransitionDest->GetFOV() - m_TransitionSrc->GetFOV())*fFactor + m_TransitionSrc->GetFOV();
				SetPosition(vPosition);
				SetLookAt(vLookAt);
				SetFOV(fFOV);
			}
			else
			{
				this->CopyFrom(m_TransitionDest);
				FOLLOWCAM->OnGameCameraReady();
				m_bAnimating = false;
			}
		}

		// apply shake
		if (m_bIsShaking)
		{
			if (m_fShakeTimer < m_fShakeDuration)
			{
				m_fNoiseIndex += g_fDeltaTime * m_fInvShakeFrequency;
				uint noiseIndex = ((uint)m_fNoiseIndex) % MaxNoiseSamples;
				float fNoiseSampleX = m_fNoiseSamplesX[noiseIndex];
				float fNoiseSampleY = m_fNoiseSamplesY[noiseIndex];
				SetPosition(m_vPosition + Vector3::Create(-fNoiseSampleX, fNoiseSampleY, fNoiseSampleX)*m_fShakeIntensity);
				m_fShakeTimer += g_fDeltaTime;
			}
			else
			{
				SetPosition(m_vPreShakePosition);
				m_bIsShaking = false;
			}
		}

	}

	//! Transition to this camera from another camera
	void GameCamera::TransitionFrom(Camera* pOther)
	{
		m_TransitionSrc = pOther;
		m_TransitionDest->CopyFrom(this);
		this->CopyFrom(pOther);
		m_fTransitionTimer = 0.0f;
		m_bAnimating = true;
		Activate();
	}

	//! shakes the camera
	void GameCamera::Shake(float intensity, float frequency, float duration)
	{
		if (InputManager::Instance()->IsTouchPressed() || m_bAnimating)
			return;

		if (m_bIsShaking)
			SetPosition(m_vPreShakePosition);

		m_fShakeDuration = duration;
		m_fInvShakeFrequency = 1.0f / frequency;
		m_fShakeIntensity = intensity;
		m_fShakeTimer = 0.0f;
		m_fNoiseIndex = 0.0f;
		m_vPreShakePosition = GetPosition();
		m_bIsShaking = true;
	}

	//! StopShaking
	void GameCamera::StopShaking()
	{
		if (!m_bIsShaking)
			return;

		SetPosition(m_vPreShakePosition);
		m_bIsShaking = false;
	}
}

