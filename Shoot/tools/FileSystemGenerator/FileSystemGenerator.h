/* 

Amine Rehioui
Created: April 28th 2012

*/

#ifndef _FILE_SYSTEM_GENERATOR_H_INCLUDED_
#define _FILE_SYSTEM_GENERATOR_H_INCLUDED_

namespace shoot
{
	//! FileSystemGenerator
	class FileSystemGenerator
	{
	public:

		//! generates the file system
		void Generate();

	private:

		//! partition info
		struct PartitionInfo
		{
			std::string name;
			std::vector<std::string> aFiles;
		};

		//! reads the partitions from the data folder
		void ReadPartitions(const std::string& strDirectory, std::vector<PartitionInfo>& aPartitions, int currentPartition = -1);

		//! returns the ShootFS file size
		uint GetShootFSFileSize(const char* strPath) const;

		//! get ShootFS file data
		void GetShootFSFileData(const char* strPath, uchar* pDataOut) const;

		//! build the atlases
		void BuildAtlases(std::vector<PartitionInfo>& aPartitions);

		std::vector< std::vector<std::string> > m_aAtlasFiles;
	};
}

#endif // _FILE_SYSTEM_GENERATOR_H_INCLUDED_


