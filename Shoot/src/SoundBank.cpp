/* 

Amine Rehioui
Created: August 26th 2013

*/

#include "Shoot.h"

#include "SoundBank.h"

#ifndef SHOOT_NO_SOUND

#if SHOOT_PLATFORM == SHOOT_PLATFORM_WP8
#include "File.h"
#endif

#include "ck/bank.h"

namespace shoot
{
	DEFINE_OBJECT(SoundBank);

	//! constructor
	SoundBank::SoundBank(const char* strBank)		
	{
		std::string path = std::string("audio/") + strBank;

#if SHOOT_PLATFORM == SHOOT_PLATFORM_WP8
		std::replace(path.begin(), path.end(), '/', '-');
		path = File::GetBasePath() + std::string("\\Assets\\") + path;
		m_pBank = CkBank::newBank(path.c_str(), kCkPathType_FileSystem);
#else
		m_pBank = CkBank::newBank(path.c_str());
#endif
	}

	//! destructor
	SoundBank::~SoundBank()
	{
		if(m_pBank)
		{
			m_pBank->destroy();
			m_pBank = NULL;
		}
	}
}

#else
namespace shoot
{	
	// strictly for win XP build where cricket lib is not available
	DEFINE_OBJECT(SoundBank);
	SoundBank::SoundBank(const char* strBank) { }
	SoundBank::~SoundBank() { }
}
#endif // SHOOT_NO_SOUND

