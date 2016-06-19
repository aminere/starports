/* 

Amine Rehioui
Created: July 11th 2013

*/

#pragma once


#include "File.h"

#include <android/asset_manager.h>

namespace shoot
{
	//! File class to read from Android APK
	class FileAPK : public File
	{
	public:

		//! constructor
		FileAPK(const char* strPath);

		//! opens the file
		bool Open(bool bAssertOnFailure = true);

		//! closes the file
		void Close();

		//! reads data from the file
		size_t Read(void* pDest, size_t bytesToRead);

		//! writes data to the file
		size_t Write(const void* pSrc, size_t bytesToWrite) { return 0; }
	
		//! changes the current read/write location in the file
		void SetOffset(size_t offset, E_OffsetType eType = OT_Current);

		//! returns the current read/write location in the file
		size_t GetOffset();

		//! returns true if end of file has been reached
		bool EOFReached();

		// printing operators
		File& operator << (const char* param) { return *this; }
		File& operator << (int param) { return *this; }
		File& operator << (float param) { return *this; }
		File& operator << (char param) { return *this; }

		//! sets the asset manager
		inline static void SetAssetManager(AAssetManager* pMgr) { m_spAssetManager = pMgr; }

	private:

		AAsset* m_pFile;
		static AAssetManager* m_spAssetManager;
	};
}



