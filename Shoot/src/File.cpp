/* 

Amine Rehioui
Created: August 5th 2010

*/

#include "Shoot.h"

#include "File.h"

#include "FileLibC.h"
#include "FileFS.h"

#if SHOOT_PLATFORM == SHOOT_PLATFORM_ANDROID
#include "FileAPK.h"
#endif

#include <ctype.h> // ::tolower

#include "FileSystem.h"

namespace shoot
{
	// static variables initization
	std::string File::m_strBasePath;
	std::string File::m_strWritablePath;
	std::string File::m_strExternalWritablePath;
	bool File::ms_bUseWritablePath = false;
	bool File::ms_bUseExternalWritablePath = false;	
	bool File::ms_bUseDataFolder = false;

	//! constructor
	File::File(const char* strPath, E_Mode eMode)
		: m_strPath(strPath)
		, m_eMode(eMode)
	{
		std::transform(m_strPath.begin(), m_strPath.end(), m_strPath.begin(), ::tolower);
	}

	//! creates a file based on the platform
	File* File::Create(const char* strPath, E_Mode eMode)
	{
		if(eMode == M_ReadBinary)
		{
			if(!ms_bUseWritablePath && !ms_bUseExternalWritablePath && !ms_bUseDataFolder)			
				return snew FileFS(strPath);
		}

		return CreateNative(strPath, eMode);
	}

	//! create a native file based on the platform
	File* File::CreateNative(const char* strPath, E_Mode eMode)
	{
#if SHOOT_PLATFORM == SHOOT_PLATFORM_ANDROID
		if(!ms_bUseWritablePath && !ms_bUseExternalWritablePath)
		{
			SHOOT_ASSERT(eMode == M_ReadBinary, "APK is read-binary only");
			return snew FileAPK(strPath);
		}
#endif

		return snew FileLibC(strPath, eMode);
	}

	//! returns true if a file exists
	bool File::Exists(const char* strPath)
	{
		if (!ms_bUseWritablePath && !ms_bUseExternalWritablePath && !ms_bUseDataFolder)
			return FileSystem::Instance()->Exists(strPath);

		File* pFile = CreateNative(strPath, M_Read);
		bool bExists = pFile->Open(false);

		if(bExists)
			pFile->Close();

		delete pFile;
		return bExists;
	}

	//! reads a string
	size_t File::Read(std::string& dest, char delim1 /*= 0*/, char delim2 /*= 0*/)
	{
		char c;
		while(Read(&c, 1))
		{
			if(c != '\r' 
			&& c != '\n'
			&& c != ' '
			&& c != '\t'
			&& ((delim1 == 0) || (c != delim1))
			&& ((delim2 == 0) || (c != delim2)))
			{
				dest += c;
			}
			else
			{
				SetOffset(-1);
				break;
			}
		}
		return dest.length();
	}

	//! reads a line until a delimiter
	size_t File::ReadLine(char* pDest, size_t numChars, char delim /*= '\n'*/)
	{
		size_t i;
		for(i=0; i<numChars; ++i)
		{
			char c;
			if(Read(&c, 1))
			{		
				if(c != delim)
					pDest[i] = c;
				else
					break;
			}
			else
			{
				break;
			}
		}
		pDest[i] = '\0';
		return i;
	}

	//! peeks a character without advancing the file pointer
	char File::Peek()
	{
		char c;
		Read(&c, 1);
		SetOffset(-1);
		return c;
	}

	//! ignores n characters or x characters until the delimiter is found
	void File::Ignore(int numChars, char delimiter /*= ' '*/)
	{
		for(int i=0; i<numChars; ++i)
		{
			char c;
			Read(&c, 1);
			if(c == delimiter)
			{
				break;
			}
		}
	}

	//! returns the file size in bytes
	size_t File::GetSize()
	{
		auto offset = GetOffset();
		SetOffset(0, File::OT_End);
		auto size = GetOffset();
		SetOffset(offset, shoot::File::OT_Start);
		return (size_t)size;
	}
}

