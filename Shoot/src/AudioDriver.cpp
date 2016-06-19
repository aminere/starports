/* 

Amine Rehioui
Created: August 26th 2013

*/

#include "Shoot.h"

#include "AudioDriver.h"

#ifndef SHOOT_NO_SOUND
#include "ck/ck.h"
#include "ck/config.h"
#include "ck/mixer.h"
#include "ck/sound.h"

namespace shoot
{
	//! Constructor
	AudioDriver::AudioDriver()
		: m_pMusicMixer(NULL)
		, m_pSoundMixer(NULL)
		, m_MusicEnabled(true)
		, m_SoundEnabled(true)
		, m_MusicVolume(1.0f)
		, m_SoundVolume(1.0f)
	{
	}

	//! Destructor
	AudioDriver::~AudioDriver()
	{
		if(m_pMusicMixer)
		{
			m_pMusicMixer->destroy();
			m_pMusicMixer = NULL;
		}

		if(m_pSoundMixer)
		{
			m_pSoundMixer->destroy();
			m_pSoundMixer = NULL;
		}

		CkUpdate();
		CkShutdown();
	}

	//! driver initialization
	void AudioDriver::Init()
	{
#if SHOOT_PLATFORM != SHOOT_PLATFORM_ANDROID
		CkConfig config;
		CkInit(&config);
#endif

		m_pMusicMixer = CkMixer::newMixer("MusicMixer");
		m_pSoundMixer = CkMixer::newMixer("SoundMixer");
	}

	//! driver update
	void AudioDriver::Update()
	{
		CkUpdate();
	}

	//! create a sound bank
	SoundBank* AudioDriver::CreateSoundBank(const char* strBank)
	{
		return snew SoundBank(strBank);
	}

	//! creates a sound
	Sound* AudioDriver::CreateSound(SoundBank* pBank, const char* strName)
	{
		Sound* pSound = snew Sound(pBank, strName);
		pSound->GetSound()->setMixer(m_pSoundMixer);
		return pSound;
	}

	//! create a music
	Music* AudioDriver::CreateMusic(const char* strOGG)
	{
		Music* pMusic = snew Music(strOGG);
		pMusic->GetSound()->setMixer(m_pMusicMixer);
		return pMusic;
	}

	//! set music status
	void AudioDriver::SetMusicEnabled(bool bEnabled)
	{
		m_MusicEnabled = bEnabled;
		if (m_MusicEnabled)
			m_pMusicMixer->setVolume(m_MusicVolume);
		else
			m_pMusicMixer->setVolume(0.0f);
	}

	//! set sound status
	void AudioDriver::SetSoundEnabled(bool bEnabled)
	{
		m_SoundEnabled = bEnabled;
		if (m_SoundEnabled)
			m_pSoundMixer->setVolume(m_SoundVolume);
		else
			m_pSoundMixer->setVolume(0.0f);
	}

	//! set the music volume
	void AudioDriver::SetMusicVolume(float fVolume)
	{
		m_MusicVolume = fVolume;
		if (m_MusicEnabled)
			m_pMusicMixer->setVolume(fVolume);
		else
			m_pMusicMixer->setVolume(0.0f);
	}

	//! set the sound volume
	void AudioDriver::SetSoundVolume(float fVolume)
	{
		m_SoundVolume = fVolume;
		if (m_SoundEnabled)
			m_pSoundMixer->setVolume(fVolume*0.2f); // MEGA HACK
		else
			m_pSoundMixer->setVolume(0.0f);
	}

	//! pauses all audio
	void AudioDriver::Pause()
	{
		CkSuspend();
	}

	//! resume all audio
	void AudioDriver::Resume()
	{
		CkResume();
	}
}
#else
namespace shoot
{
	// strictly for win XP build where cricket lib is not available
	AudioDriver::AudioDriver() { }
	AudioDriver::~AudioDriver() { }
	void AudioDriver::Init() { }
	void AudioDriver::Update() { }
	SoundBank* AudioDriver::CreateSoundBank(const char* strBank) { return NULL; }
	Sound* AudioDriver::CreateSound(SoundBank* pBank, const char* strName) { return NULL; }
	Music* AudioDriver::CreateMusic(const char* strOGG)	{ return NULL; }
	void AudioDriver::SetMusicEnabled(bool bEnabled) { }
	void AudioDriver::SetSoundEnabled(bool bEnabled) { }
	void AudioDriver::SetMusicVolume(float fVolume) { }
	void AudioDriver::SetSoundVolume(float fVolume) { }
	void AudioDriver::Pause() { }
	void AudioDriver::Resume() { }
}
#endif // SHOOT_NO_SOUND