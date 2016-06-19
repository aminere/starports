/* 

Amine Rehioui
Created: August 26th 2013

*/

#include "Shoot.h"

#include "Sound.h"
#include "SoundBank.h"

#ifndef SHOOT_NO_SOUND
#include "ck/sound.h"

namespace shoot
{
	DEFINE_OBJECT(Sound);

	//! constructor
	Sound::Sound(SoundBank* pBank, const char* strName)		
	{
		m_pSound = CkSound::newBankSound(pBank->GetBank(), strName);
	}

	//! destructor
	Sound::~Sound()
	{
		Stop();

		if(m_pSound)
		{
			m_pSound->destroy();
			m_pSound = NULL;
		}
	}

	//! plays the sound
	void Sound::Play()
	{
		if(m_pSound)
			m_pSound->play();
	}

	//! stops the sound
	void Sound::Stop()
	{
		if(m_pSound)
			m_pSound->stop();
	}

	//! returns the playing status
	bool Sound::IsPlaying() const
	{
		return m_pSound->isPlaying();
	}
}

#else
namespace shoot
{	
	// strictly for win XP build where cricket lib is not available
	DEFINE_OBJECT(Sound);
	Sound::Sound(SoundBank* pBank, const char* strName) { }
	Sound::~Sound() { }
	void Sound::Play() { }
	void Sound::Stop() { }
	bool Sound::IsPlaying() const { return false; }
}
#endif // SHOOT_NO_SOUND

