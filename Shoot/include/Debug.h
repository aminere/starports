/* 

Amine Rehioui
Created: March 18th 2010

*/

#pragma once

#ifndef _RETAIL_
//! Assert with a formatted message macro
/** Takes a mandatory message argument */
#define SHOOT_ASSERT(condition, ...) shoot::Debug::Assert((condition) ? true : false, __FILE__, __LINE__, __VA_ARGS__);

//! Assert with a formatted message macro
/** Takes a mandatory message argument */
#define SHOOT_VERIFY(condition, ...) SHOOT_ASSERT(condition, __VA_ARGS__)

//! Warning with a formatted message macro
/** Takes a mandatory message argument */
#define SHOOT_WARNING(condition, ...) shoot::Debug::Warning((condition) ? true : false, __FILE__, __LINE__, __VA_ARGS__);

//! Log Warning with a formatted message macro
/** Takes a mandatory message argument */
#define SHOOT_LOG_WARNING(condition, ...) shoot::Debug::LogWarning((condition) ? true : false, __FILE__, __LINE__, __VA_ARGS__);
#else
#define SHOOT_ASSERT(condition, ...)
#define SHOOT_VERIFY(condition, ...) condition
#define SHOOT_WARNING(condition, ...)
#define SHOOT_LOG_WARNING(condition, ...)
#endif

//! Debugging utilities
namespace shoot
{
	//! Debug class
	class Debug
	{
	public:

		//! Assert
		static void Assert(bool bCondition, const char* strFile, int line, const char* strFormat, ...);

		//! Warning
		static void Warning(bool bCondition, const char* strFile, int line, const char* strFormat, ...);

		//! LogWarning
		static void LogWarning(bool bCondition, const char* strFile, int line, const char* strFormat, ...);

		static const int MaxWarnings = 32;

	private:

		//! warning info
		struct WarningInfo
		{
			const char* strFile;
			int Line;
			bool bFree;
			bool bDisabled;

			//! constructor
			WarningInfo() : bFree(true), bDisabled(false)
			{
			}

			//! compare warning info
			bool operator == (const WarningInfo& other);
		};

		//! returns a WarningInfo index
		static int GetWarningIndex(const char* strFile, int line);

		static WarningInfo m_WarningInfos[MaxWarnings];		
	};	
}




