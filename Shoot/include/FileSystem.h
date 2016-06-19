/* 

Amine Rehioui
Created: April 25th 2012

*/

#pragma once


namespace shoot
{
	//! FileSystem class
	class FileSystem : public Singleton<FileSystem>
	{
		DECLARE_SINGLETON(FileSystem);

	public:

		static const int MaxPartitionNameLength = 16;
		static const int MaxFilePathLength = 64;
		static const int MaxFilesPerPartition = 1000;
		static const int BootPartitionIndex = 0;
		static const int CommonPartitionIndex = 1;

		//! file info
		struct FileInfo
        {                       
            int offset; // offset in uncompressed data in memory
            int size; // uncompressed size in memory
            uchar partition;
        };      
  
		//! partition info
        struct PartitionInfo
        {
            int offset; // offset in compressed data on disk
            int size; // compressed size on disk
			uchar* pData; // uncompressed data in memory
            int dataSize; // uncompressed data in memory
			std::string name; // name of the partition
			bool loadedByUser;
        };

		//! file info to write
        struct FileInfoWrite
        {
            int size; // uncompressed size in memory
            char path[MaxFilePathLength];
        };

		//! partition info to write
		struct PartitionInfoWrite
        {           
            int offset; // offset in compressed data on disk
            int size; // compressed size on disk
			int uncompressedSize; // uncompressed size in memory
            int numFiles;
			char name[MaxPartitionNameLength]; // name of the partition
        };
 
		typedef std::map< std::string, FileInfo > FileMap;

		//! initializes the file system
		void Init();

		//! loads a partition
		void LoadPartition(int index);

		//! loads a partition
		void LoadPartition(const std::string& strName);

		//! unloads a partition
		void UnloadPartition(const std::string& strName);

		//! unloads all partitions
		void UnloadAllPartitions(const char* strToExclude = NULL);

		//! returns the file map
		FileMap& GetFileMap() { return m_FileMap; }

		//! returns the partitions
		std::vector<PartitionInfo>& GetPartitions() { return m_aPartitions; }

		//! returns true if a file exists
		bool Exists(const char* strPath) const;

	private:

		//! unloads a partition
		void UnloadPartition(size_t index);

		//! returns the data path
		const char* GetDataPath() const;

		FileMap m_FileMap;
		std::vector<PartitionInfo> m_aPartitions;
	};
}




