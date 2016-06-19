/* 

Amine Rehioui
Created: April 25th 2012

*/

#include "Shoot.h"

#include "FileSystem.h"

#include "File.h"

#include "zlib.h"

namespace shoot
{
	//! constructor
	FileSystem::FileSystem()
	{
#ifndef SHOOT_EDITOR
		Init();
#endif
	}

	//! destructor
	FileSystem::~FileSystem()
	{
		UnloadAllPartitions(NULL);
	}

	//! initializes the file system
	void FileSystem::Init()
	{
		File* pFile = File::CreateNative(GetDataPath(), File::M_ReadBinary);
		pFile->Open();

		uint numPartitions;
		pFile->Read(&numPartitions, sizeof(uint));

		for(uint i=0; i<numPartitions; ++i)
		{
			PartitionInfoWrite partitionInfoWrite;
			pFile->Read(&partitionInfoWrite, sizeof(PartitionInfoWrite));

			PartitionInfo partitionInfo =
			{
				partitionInfoWrite.offset, 
				partitionInfoWrite.size, 
				NULL, 
				partitionInfoWrite.uncompressedSize, 
				std::string(partitionInfoWrite.name),
				false
			};

			m_aPartitions.push_back(partitionInfo);

			SHOOT_ASSERT(partitionInfoWrite.numFiles < MaxFilesPerPartition, "MaxFilesPerPartition reached");
			if(partitionInfoWrite.numFiles > 0)
			{
				FileInfoWrite fileWriteInfos[MaxFilesPerPartition];
				pFile->Read(fileWriteInfos, partitionInfoWrite.numFiles*sizeof(FileInfoWrite));

				int fileOffset = 0;
				for(int j=0; j<partitionInfoWrite.numFiles; ++j)
				{
					std::string filePath = fileWriteInfos[j].path;
					FileInfo info = { fileOffset, fileWriteInfos[j].size, (uchar)i };
					SHOOT_ASSERT(m_FileMap.find(filePath) == m_FileMap.end(), "Can't have multiple files with the same path '%s'", filePath.c_str());
					m_FileMap[filePath] = info;
					fileOffset += info.size;
				}
			}
		}

		pFile->Close();
		delete pFile;
	}

	//! loads a partition
	void FileSystem::LoadPartition(int index)
	{
		if (m_aPartitions[index].pData) // already loaded
			return;

		File* pFile = File::CreateNative(GetDataPath(), File::M_ReadBinary);
		pFile->Open();
		pFile->SetOffset(m_aPartitions[index].offset, File::OT_Start);
		uchar* compressedData = snew uchar[m_aPartitions[index].size];
		pFile->Read(compressedData, m_aPartitions[index].size);
		pFile->Close();
		delete pFile;

		// decompress
		uchar* uncompressedData = snew uchar[m_aPartitions[index].dataSize];
		unsigned long uncompressedDataSizeL = m_aPartitions[index].dataSize;
		int result = uncompress(uncompressedData, &uncompressedDataSizeL, compressedData, m_aPartitions[index].size);
		SHOOT_ASSERT(result == Z_OK, "zlib uncompress failed");
		SHOOT_ASSERT(m_aPartitions[index].dataSize == uncompressedDataSizeL, "unexpected uncompressedDataSize");

		m_aPartitions[index].pData = uncompressedData;
		m_aPartitions[index].loadedByUser = false;
		sdelete_array(compressedData);
	}

	//! loads a partition
	void FileSystem::LoadPartition(const std::string& strName)
	{
		if (m_aPartitions.empty())
			return;

		for (size_t i = 0; i < m_aPartitions.size(); ++i)
		{
			if (m_aPartitions[i].name == strName)
			{
				if (m_aPartitions[i].pData) // already loaded
					return;

				Log.Print("LOADING PARTITION %s..\n", strName.c_str());
				LoadPartition((int)i);
				m_aPartitions[i].loadedByUser = true;
				return;
			}
		}

		SHOOT_WARNING(false, "Unknown partition: %s", strName.c_str());
	}

	//! unloads a partition
	void FileSystem::UnloadPartition(const std::string& strName)
	{
		if (m_aPartitions.empty())
			return;

		for (size_t i=0; i<m_aPartitions.size(); ++i)
		{
			if(m_aPartitions[i].name == strName)
			{
				UnloadPartition(i);
				return;
			}
		}

		SHOOT_WARNING(false, "Unknown partition: %s", strName.c_str());
	}

	//! unloads a partition
	void FileSystem::UnloadPartition(size_t index)
	{
		if(m_aPartitions[index].pData)
		{
			Log.Print("UNLOADING PARTITION %s..\n", m_aPartitions[index].name.c_str());
			sdelete_array(m_aPartitions[index].pData);
		}
	}

	//! unloads all partitions
	void FileSystem::UnloadAllPartitions(const char* strToExclude /*= NULL*/)
	{
		for (size_t i=0; i<m_aPartitions.size(); ++i)
		{
			if(strToExclude && m_aPartitions[i].name == strToExclude)
			{
				 continue;
			}

			UnloadPartition(i);
		}
	}

	//! returns true if a file exists
	bool FileSystem::Exists(const char* strPath) const
	{
		std::string path(strPath);
		std::transform(path.begin(), path.end(), path.begin(), ::tolower);
		bool bExists = (m_FileMap.find(path) != m_FileMap.end());
		return bExists;
	}

	//! returns the data path
	const char* FileSystem::GetDataPath() const
	{
#ifdef DX11
		return "data_dx11.fs";
#else
		return "data.fs";
#endif
	}
}
