/* 

Amine Rehioui
Created: April 25th 2012

*/

#pragma once


#include "File.h"

namespace shoot
{
	//! File that accesses the shoot file system
	class FileFS : public File
	{
	public:

		//! constructor
		FileFS(const char* strPath);	

		//! opens the file
		bool Open(bool bAssertOnFailure = true);

		//! closes the file
		void Close() { }

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

		//! returns a pointer to the file data
		uchar* GetData(size_t size) override;

		// printing operators
		File& operator << (const char* param) { return *this; }
		File& operator << (int param) { return *this; }
		File& operator << (float param) { return *this; }
		File& operator << (char param) { return *this; }

	private:

		size_t m_CurrentOffset;
		uchar* m_pData;
		size_t m_Size;
	};
}



