/* 

Amine Rehioui
Created: August 5th 2010

*/

#pragma once


namespace shoot
{
	//! base file class
	class File
	{
	public:

		//! File handling mode
		enum E_Mode
		{
			M_Read,
			M_Write,
			M_ReadBinary,
			M_WriteBinary,
			M_Append
		};

		//! creates a file based on the platform
		static File* Create(const char* strPath, E_Mode eMode);

		//! create a native file based on the platform
		static File* CreateNative(const char* strPath, E_Mode eMode);

		//! returns true if a file exists
		static bool Exists(const char* strPath);

		//! destructor
		virtual ~File()
		{
		}

		//! File offset types
		enum E_OffsetType
		{
			OT_Current = 0,
			OT_Start,
			OT_End
		};

		//! opens the file
		virtual bool Open(bool bAssertOnFailure = true) = 0;

		//! closes the file
		virtual void Close() = 0;

		//! reads data from the file
		virtual size_t Read(void* pDest, size_t bytesToRead) = 0;

		//! reads a string
		virtual size_t Read(std::string& dest, char delim1 = 0, char delim2 = 0);

		//! reads a line until a delimiter
		virtual size_t ReadLine(char* pDest, size_t numChars, char delim = '\n');

		//! writes data to the file
		virtual size_t Write(const void* pSrc, size_t bytesToWrite) = 0;

		//! writes data to the file
		template <class T>
		size_t WriteData(const T* pDataIn, size_t count)
		{
			return Write(pDataIn, count*sizeof(T));
		}

		//! peeks a character without advancing the file pointer
		virtual char Peek();

		//! ignores n characters or x characters until the delimiter is found
		virtual void Ignore(int numChars, char delimiter = ' ');
	
		//! changes the current read/write location in the file
		virtual void SetOffset(size_t offset, E_OffsetType eType = OT_Current) = 0;

		//! returns the current read/write location in the file
		virtual size_t GetOffset() = 0;

		//! returns true if end of file has been reached
		virtual bool EOFReached() = 0;

		//! returns the file size in bytes
		size_t GetSize();

		//! returns a pointer to the file data
		virtual uchar* GetData(size_t size) { return NULL; }

		// printing operators
		virtual File& operator << (const char* param) = 0;
		virtual File& operator << (int param) = 0;
		virtual File& operator << (float param) = 0;
		virtual File& operator << (char param) = 0;

		//! returns the file path
		const char* GetPath() const { return m_strPath.c_str(); }

		//! sets base path for all files
		inline static void SetBasePath(const char* strPath) { m_strBasePath = strPath; }

		//! gets the base path
		inline static std::string& GetBasePath() { return m_strBasePath; }

		//! sets the writable path
		inline static void SetWritablePath(const char* strPath) { m_strWritablePath = strPath; }

		//! sets the external writable path
		inline static void SetExternalWritablePath(const char* strPath) { m_strExternalWritablePath = strPath; }

		//! UseDataFolder
		inline static void SetUseDataFolder(bool use) { ms_bUseDataFolder = use; }
		inline static bool GetUseDataFolder() { return ms_bUseDataFolder; }

		//! UseWritablePath
		struct UseWritablePath
		{
			UseWritablePath() {	File::ms_bUseWritablePath = true; }			
			~UseWritablePath() { File::ms_bUseWritablePath = false;	}
		};

		//! UseExternalWritablePath
		struct UseExternalWritablePath
		{
			UseExternalWritablePath() {	File::ms_bUseExternalWritablePath = true; }			
			~UseExternalWritablePath() { File::ms_bUseExternalWritablePath = false;	}
		};

		//! reads data
		template <class T>
		void ReadData(T* pDataOut, int count)
		{
			Read(pDataOut, count*sizeof(T));
		}

	protected:

		//! constructor
		File(const char* strPath, E_Mode eMode);

		E_Mode m_eMode;
		std::string m_strPath;
		static std::string m_strBasePath;
		static std::string m_strWritablePath;
		static std::string m_strExternalWritablePath;
		static bool ms_bUseWritablePath;
		static bool ms_bUseExternalWritablePath;
		static bool ms_bUseDataFolder;
	};
}



