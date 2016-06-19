/* 

Amine Rehioui
Created: May 16th 2012

*/

#pragma once

#include "HighResTimer.h"

#ifdef _PROFILE_
#define SHOOT_PROFILE_FUNC Profiler profiler(__FUNCTION__);
#define SHOOT_PROFILE(x) Profiler profiler(#x);
#define SHOOT_PROFILE_TAG(x, tag) Profiler profiler(#x, tag);
#else
#define SHOOT_PROFILE_FUNC
#define SHOOT_PROFILE(x)
#define SHOOT_PROFILE_TAG(x, tag)

namespace shoot
{
	// forwards
	class File;

	//! Profile manager
	class ProfileManager
	{
	public:

		//! init
		static void Init();

		//! uninit
		static void Uninit();

		//! print profile info
		static void Print(const char* strInfo);

	private:

		static File* m_pFile;
	};

	//! Profiler
	class Profiler
	{
	public:

		//! constructor
		Profiler(const char* strFunc, const char* strTag = NULL);

		//! destructor
		~Profiler();

	private:

		HighResTimer m_Timer;
		const char* m_strFunc;
	};
}

#endif