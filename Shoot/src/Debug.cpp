/* 

Amine Rehioui
Created: March 18th 2010

*/

#include "Shoot.h"

#include <stdio.h> // vsprintf
#include <stdarg.h>

namespace shoot
{
	// static variables initialization
	Debug::WarningInfo Debug::m_WarningInfos[Debug::MaxWarnings];

	//! Assert
	void Debug::Assert(bool bCondition, const char* strFile, int line, const char* strFormat, ...)
	{
		if(!bCondition)
		{
			char buffer[2048];
			va_list va; 
			va_start(va, strFormat);
			vsprintf(buffer, strFormat, va);
			va_end(va);

			std::stringstream ss(std::stringstream::in | std::stringstream::out);
			ss << strFile << " @" << line << ": " << buffer;
			std::string str = ss.str();
			auto message = str.c_str();

			Log << "*** Assert failed ***\n" << message << "\n";

#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32
			MessageBoxA(NULL, message, "Assert failed", MB_OK);
			DebugBreak();
#endif
		}
	}

	//! Warning
	void Debug::Warning(bool bCondition, const char* strFile, int line, const char* strFormat, ...)
	{
		if(!bCondition)
		{
			char buffer[2048];
			va_list va; 
			va_start(va, strFormat);
			vsprintf(buffer, strFormat, va);
			va_end(va);

			std::stringstream ss(std::stringstream::in | std::stringstream::out);
			ss << strFile << " @" << line << ": " << buffer;
			std::string str = ss.str();
			auto message = str.c_str();

			Log << "*** Warning ***\n" << message << "\n";				

#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32
			int index = GetWarningIndex(strFile, line);
			if(index == -1)
			{
				// add new warning
				for(int i=0; i<MaxWarnings; ++i)
				{
					if(m_WarningInfos[i].bFree)
					{
						m_WarningInfos[i].bFree = false;
						m_WarningInfos[i].strFile = strFile;
						m_WarningInfos[i].Line = line;
						index = i;
						break;
					}
				}
			}

			if(index >= 0)
			{
				if(!m_WarningInfos[index].bDisabled)
				{
					auto choice = MessageBoxA(NULL, message, "Warning - Ignore / Debug / Continue", MB_CANCELTRYCONTINUE);
					switch(choice)
					{
					case IDCANCEL:
						m_WarningInfos[index].bDisabled = true;
						break;

					case IDTRYAGAIN:
						DebugBreak();
						break;

					case IDCONTINUE:
						m_WarningInfos[index].bDisabled = false;
						break;
					}
					m_WarningInfos[index].bFree = !m_WarningInfos[index].bDisabled;
				}
			}
			else
			{
				MessageBoxA(NULL, message, "Warning", MB_OK);
			}				
#endif
		}
	}

	//! LogWarning
	void Debug::LogWarning(bool bCondition, const char* strFile, int line, const char* strFormat, ...)
	{
		if(!bCondition)
		{
			char buffer[2048];
			va_list va; 
			va_start(va, strFormat);
			vsprintf(buffer, strFormat, va);
			va_end(va);

			std::stringstream ss(std::stringstream::in | std::stringstream::out);
			ss << strFile << " @" << line << ": " << buffer;
			std::string str = ss.str();
			const char* message = str.c_str();
			Log << message << "\n";
		}
	}

	//! compare warning info
	bool Debug::WarningInfo::operator == (const WarningInfo& other)
	{
		return ((strcmp(strFile, other.strFile) == 0) && Line == other.Line);
	}

	//! returns a WarningInfo index
	int Debug::GetWarningIndex(const char* strFile, int line)
	{
		for(int i=0; i<MaxWarnings; ++i)
		{
			WarningInfo info;
			info.strFile = strFile;
			info.Line = line;
			if(!m_WarningInfos[i].bFree && m_WarningInfos[i] == info)
			{
				return i;
			}
		}
		return -1;
	}
}

