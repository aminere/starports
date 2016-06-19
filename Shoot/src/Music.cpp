/* 

Amine Rehioui
Created: August 28th 2013

*/

#include "Shoot.h"

#include "Music.h"

#ifndef SHOOT_NO_SOUND
#if SHOOT_PLATFORM == SHOOT_PLATFORM_WP8
#include "File.h"
#endif

#include "ck/sound.h"

namespace shoot
{
	DEFINE_OBJECT(Music);

	//! constructor
	Music::Music(const char* strOGG)
		: m_bStartedPlaying(false)
	{
		std::string path = std::string("audio/") + strOGG;

#if SHOOT_PLATFORM == SHOOT_PLATFORM_WP8
		std::replace(path.begin(), path.end(), '/', '-');
		path = File::GetBasePath() + std::string("\\Assets\\") + path;
		m_pMusic = CkSound::newStreamSound(path.c_str(), kCkPathType_FileSystem);
#else
		m_pMusic = CkSound::newStreamSound(path.c_str());
#endif
	}

	//! destructor
	Music::~Music()
	{
		Stop();

		if(m_pMusic)
		{
			m_pMusic->destroy();
			m_pMusic = NULL;
		}
	}

	//! plays the music
	void Music::Play()
	{
		if(m_pMusic)
		{
			m_pMusic->play();
		}
	}

	//! stops the music
	void Music::Stop()
	{
		if(m_pMusic)
		{
			m_pMusic->stop();
		}
	}

	//! returns the playing status
	bool Music::IsPlaying()
	{
		if(m_pMusic->isPlaying())
		{
			m_bStartedPlaying = true;
		}
		return m_bStartedPlaying ? m_pMusic->isPlaying() : true;
	}
}

#else
namespace shoot
{	
	// strictly for win XP build where cricket lib is not available
	DEFINE_OBJECT(Music);
	Music::Music(const char* strOGG) { }
	Music::~Music() { }
	void Music::Play() { }
	void Music::Stop() { }
	bool Music::IsPlaying() { return false; }
}
#endif // SHOOT_NO_SOUND

