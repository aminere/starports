/* 

Amine Rehioui
Created: April 1st 2011

*/

#include "Shoot.h"

#include <iostream>

#if defined(SHOOT_EDITOR)
extern void EditorOutput(const char* strMessage);
#endif

#if SHOOT_PLATFORM == SHOOT_PLATFORM_ANDROID
#include <android/log.h>
#endif

namespace shoot
{
	BaseLog Log;

	//! Constructor
	BaseLog::BaseLog()		
	{
		#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32
		m_pLog = fopen("DebugLog.txt", "wt");
		setbuf(m_pLog, NULL);
		#endif
	}

	//! Prints to available output streams
	void BaseLog::Print(const char* format, ...)
	{
		char buffer[1024];
		va_list va;
		va_start(va, format);
		vsprintf(buffer, format, va);
		va_end(va);
		
		PrintInternal(buffer);
	}

	BaseLog& BaseLog::operator << (const char* str)
	{
		PrintInternal(str);
		return *this;
	}

	BaseLog& BaseLog::operator << (const wchar_t* str)
	{
		char buf[1024];
		wcstombs(buf, str, 1024);
		PrintInternal(buf);
		return *this;
	}	

	BaseLog& BaseLog::operator << (size_t i)
	{
		std::stringstream ss(std::stringstream::in | std::stringstream::out);
		ss << i;
		std::string str = ss.str();
		PrintInternal(str.c_str());
		return *this;
	}

	BaseLog& BaseLog::operator << (float f)
	{
		std::stringstream ss(std::stringstream::in | std::stringstream::out);
		ss << f;
		std::string str = ss.str();
		PrintInternal(str.c_str());
		return *this;
	}

	BaseLog& BaseLog::operator << (const Vector3& v)
	{
		std::stringstream ss(std::stringstream::in | std::stringstream::out);
		ss << v.X << ", " << v.Y << ", " << v.Z;
		std::string str = ss.str();
		PrintInternal(str.c_str());
		return *this;
	}

	BaseLog& BaseLog::operator << (const Vector2& v)
	{
		std::stringstream ss(std::stringstream::in | std::stringstream::out);
		ss << v.X << ", " << v.Y;
		std::string str = ss.str();
		PrintInternal(str.c_str());
		return *this;
	}

	//! Prints to available output streams
	void BaseLog::PrintInternal(const char* str)
	{
#if !defined(_RETAIL_) || SHOOT_PLATFORM == SHOOT_PLATFORM_ANDROID
		std::cout << str;

		#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32 || SHOOT_PLATFORM == SHOOT_PLATFORM_WP8
		wchar_t buf[1024];
		mbstowcs(buf, str, 1024);
		OutputDebugString(buf);
		#endif

		#if defined(SHOOT_EDITOR)
		EditorOutput(str);
		#endif

		#if SHOOT_PLATFORM == SHOOT_PLATFORM_ANDROID
		__android_log_print(ANDROID_LOG_INFO, "Shoot", "%s", str);
		#endif

		#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32
			if (m_pLog)
				fprintf(m_pLog, str);
		#endif
#endif
	}

	//! Close
	void BaseLog::Close()
	{
#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32
		if (m_pLog)
			fclose(m_pLog);
#endif
	}
}

