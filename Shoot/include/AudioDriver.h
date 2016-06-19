/* 

Amine Rehioui
Created: August 26th 2013

*/

#pragma once


#include "Sound.h"
#include "SoundBank.h"
#include "Music.h"

// forwards
class CkMixer;

namespace shoot
{
	//! AudioDriver 
	class AudioDriver : public Singleton<AudioDriver>
	{
		DECLARE_SINGLETON(AudioDriver);

	public:
	
		//! driver initialization
		void Init();

		//! driver update
		void Update();

		//! create a sound bank
		SoundBank* CreateSoundBank(const char* strBank);

		//! creates a sound
		Sound* CreateSound(SoundBank* pBank, const char* strName);

		//! create a music
		Music* CreateMusic(const char* strOGG);

		//! set music status
		void SetMusicEnabled(bool bEnabled);

		//! set sound status
		void SetSoundEnabled(bool bEnabled);

		//! set the music volume
		void SetMusicVolume(float fVolume);

		//! set the sound volume
		void SetSoundVolume(float fVolume);

		//! pauses all audio
		void Pause();

		//! resume all audio
		void Resume();

	private:

		bool m_MusicEnabled;
		bool m_SoundEnabled;
		float m_MusicVolume;
		float m_SoundVolume;
		CkMixer* m_pMusicMixer;
		CkMixer* m_pSoundMixer;
	};
}


