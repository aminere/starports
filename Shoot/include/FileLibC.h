/* 

Amine Rehioui
Created: August 5th 2010

*/

#pragma once


#include "File.h"

#include <stdio.h>

namespace shoot
{
	//! File class with default LibC implementation
	class FileLibC : public File
	{
	public:

		//! constructor
		FileLibC(const char* strPath, E_Mode eMode);	

		//! opens the file
		bool Open(bool bAssertOnFailure = true);

		//! closes the file
		void Close();

		//! reads data from the file
		size_t Read(void* pDest, size_t bytesToRead);

		//! writes data to the file
		size_t Write(const void* pSrc, size_t bytesToWrite);
	
		//! changes the current read/write location in the file
		void SetOffset(size_t offset, E_OffsetType eType = OT_Current);

		//! returns the current read/write location in the file
		size_t GetOffset();

		//! returns true if end of file has been reached
		bool EOFReached();

		// printing operators
		File& operator << (const char* param);
		File& operator << (int param);
		File& operator << (float param);
		File& operator << (char param);

	private:

		//! InternalOpen
		FILE* InternalOpen(bool bAssertOnFailure);

		FILE* m_pFile;
	};
}



