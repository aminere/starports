/* 

Amine Rehioui
Created: August 5th 2010

*/

#include "Shoot.h"

#include "FileLibC.h"

namespace shoot
{
	//! constructor
	FileLibC::FileLibC(const char* strPath, E_Mode eMode) 
		: File(strPath, eMode)
		, m_pFile(NULL)
	{
	}	

	//! opens the file
	bool FileLibC::Open(bool bAssertOnFailure /*= true*/)
	{
		m_pFile = InternalOpen(bAssertOnFailure);
		return (m_pFile != NULL);
	}

	//! closes the file
	void FileLibC::Close()
	{
		if(m_pFile)
		{
			fclose(m_pFile);
			m_pFile = NULL;
		}
	}

	//! reads data from the file
	size_t FileLibC::Read(void* pDest, size_t bytesToRead)
	{
		return fread(pDest, 1, bytesToRead, m_pFile);
	}

	//! writes data to the file
	size_t FileLibC::Write(const void* pSrc, size_t bytesToWrite)
	{
		SHOOT_ASSERT(m_pFile, "Trying to write in a NULL file");
		return fwrite(pSrc, 1, bytesToWrite, m_pFile);
	}

	//! changes the current read/write location in the filesize_t
	void FileLibC::SetOffset(size_t offset, E_OffsetType eType /*= OT_Current*/)
	{
		int seekModes[] = { SEEK_CUR, SEEK_SET, SEEK_END };
		while (fseek(m_pFile, (int)offset, seekModes[eType]))
		{
			Log << "WARNING: FileLibC::SetOffset: fseek failure\n";
		}
	}

	//! returns the current read/write location in the file
	size_t FileLibC::GetOffset()
	{
		return ftell(m_pFile);
	}

	//! returns true if end of file has been reached
	bool FileLibC::EOFReached()
	{
		return (feof(m_pFile) != 0);
	}	

	//! InternalOpen
	FILE* FileLibC::InternalOpen(bool bAssertOnFailure)
	{
		const char* strModes[] = { "rt", "wt", "rb", "wb", "a" };

		std::string path;

		if(ms_bUseWritablePath || ms_bUseExternalWritablePath)
		{
			std::string writablePath = ms_bUseWritablePath ? m_strWritablePath : m_strExternalWritablePath;
			if(!writablePath.empty())
			{
				path = writablePath + std::string("/");
			}
		}
		else if(!m_strBasePath.empty())
		{
			path = m_strBasePath + std::string("/");
		}
		else if(ms_bUseDataFolder)
		{
			path = "data/";
		}

		path = path + m_strPath;

		FILE* pFile = fopen(path.c_str(), strModes[m_eMode]);
		if(!pFile && bAssertOnFailure)
		{
			SHOOT_WARNING(false, "Could not open file %s\n", path.c_str());
		}
		return pFile;
	}

	// printing operators
	File& FileLibC::operator << (const char* param)
	{
		fprintf(m_pFile, "%s", param);
		return *this;
	}

	File& FileLibC::operator << (int param)
	{
		fprintf(m_pFile, "%d", param);
		return *this;
	}

	File& FileLibC::operator << (float param)
	{
		fprintf(m_pFile, "%f", param);
		return *this;
	}

	File& FileLibC::operator << (char param)
	{
		fprintf(m_pFile, "%c", param);
		return *this;
	}
}

