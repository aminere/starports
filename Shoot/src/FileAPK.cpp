/* 

Amine Rehioui
Created: July 11th 2013

*/

#include "Shoot.h"

#include "FileAPK.h"

namespace shoot
{
	// statics
	AAssetManager* FileAPK::m_spAssetManager = NULL;

	//! constructor
	FileAPK::FileAPK(const char* strPath) 
		: File(strPath, M_Read)
		, m_pFile(NULL)
	{
	}

	//! opens the file
	bool FileAPK::Open(bool bAssertOnFailure /*= true*/)
	{
		const char* strModes[] = { "rt", "wt", "rb", "wb" };		

		m_pFile = AAssetManager_open(m_spAssetManager, m_strPath.c_str(), AASSET_MODE_BUFFER);

		if(!m_pFile && bAssertOnFailure)
		{
			SHOOT_ASSERT(false, "Could not open file %s\n", m_strPath.c_str());
		}
		
		return (m_pFile != NULL);
	}

	//! closes the file
	void FileAPK::Close()
	{
		AAsset_close(m_pFile);
		m_pFile = NULL;
	}

	//! reads data from the file
	size_t FileAPK::Read(void* pDest, size_t bytesToRead)
	{
		return AAsset_read(m_pFile, pDest, bytesToRead);
	}

	//! changes the current read/write location in the file
	void FileAPK::SetOffset(size_t offset, E_OffsetType eType /*= OT_Current*/)
	{
		int seekModes[] = { SEEK_CUR, SEEK_SET, SEEK_END };		
		AAsset_seek(m_pFile, offset, seekModes[eType]);
	}

	//! returns the current read/write location in the file
	size_t FileAPK::GetOffset()
	{
		return AAsset_getLength(m_pFile) - AAsset_getRemainingLength(m_pFile);
	}

	//! returns true if end of file has been reached
	bool FileAPK::EOFReached()
	{
		return (AAsset_getRemainingLength(m_pFile) <= 0);
	}	
}

