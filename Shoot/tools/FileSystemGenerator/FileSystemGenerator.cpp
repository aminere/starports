/* 

Amine Rehioui
Created: April 28th 2012

*/

#include "Shoot.h"

#include "FileSystemGenerator.h"
#include "FileSystem.h"
#include "File.h"
#include "zlib.h"
#include "PNGLoader.h"
#include "FBXMeshLoader.h"

#ifdef DX11
#include "ShaderDX11.h"
#include <d3dcompiler.h>
#endif

namespace shoot
{
	//! generates the file system
	void FileSystemGenerator::Generate()
	{		
		const char* strPath = "../data.fs";

#ifdef DX11
		 strPath = "../data_dx11.fs";
#endif

		File* pFile = File::CreateNative(strPath, File::M_WriteBinary);
		pFile->Open();

		std::vector<PartitionInfo> aPartitions;
		ReadPartitions("data", aPartitions);

		BuildAtlases(aPartitions);

		// FS meta data
		uint numPartitions = (uint)aPartitions.size();
		pFile->Write(&numPartitions, sizeof(uint));

		// determine partition start offset
		uint partitionStartOffset = sizeof(uint) + numPartitions*sizeof(FileSystem::PartitionInfoWrite);
		for (uint i = 0; i<numPartitions; ++i)
		{
			uint numFiles = (uint)aPartitions[i].aFiles.size();
			partitionStartOffset += numFiles*sizeof(FileSystem::FileInfoWrite);
		}

		uint partitionOffset = partitionStartOffset;
		std::vector<uchar*> aCompressedData;
		std::vector<uint> aCompressedDataSize;
		for (uint i = 0; i<numPartitions; ++i)
		{
			Log << "*** Processing partition '" << aPartitions[i].name.c_str() << "'\n";
			uint numFiles = (uint)aPartitions[i].aFiles.size();
			uint compressedDataSize = 0;
			uchar* compressedData = NULL;
			uint uncompressedDataSize = 0;
			FileSystem::FileInfoWrite* fileWriteInfo = NULL;

			if(numFiles > 0)
			{
				// get file info
				fileWriteInfo = snew FileSystem::FileInfoWrite[numFiles];
				Log << "Processing " << (size_t)numFiles << " files\n";
				std::map<std::string, uint> fileDuplicateChecker;
				for (uint j = 0; j<numFiles; ++j)
				{
					std::string filePath = aPartitions[i].aFiles[j].c_str();					
					uint fileSize = GetShootFSFileSize(filePath.c_str());
					uncompressedDataSize += fileSize;

					// copy file info
					fileWriteInfo[j].size = fileSize;
					uint pathLength = (uint)filePath.length();
					if(pathLength+1 > FileSystem::MaxFilePathLength)
					{
						SHOOT_WARNING(false, "Truncating path %s", filePath.c_str());
						pathLength = FileSystem::MaxFilePathLength-1;
					}
					memcpy(fileWriteInfo[j].path, filePath.c_str(), pathLength);
					fileWriteInfo[j].path[pathLength] = '\0';
					
					{
						// check for duplicates
						std::string path = fileWriteInfo[j].path;
						if (fileDuplicateChecker.find(path) != fileDuplicateChecker.end())
						{
							Log << "Duplicate file path detected '" << path.c_str() << "'\n";
							SHOOT_ASSERT(false, "Failed");
						}
						fileDuplicateChecker[path] = 0;
					}
				}

				// prepare uncompressed partition data
				uchar* uncompressedData = snew uchar[uncompressedDataSize];
				uint fileOffset = 0;
				for (uint j = 0; j<numFiles; ++j)
				{
					std::string filePath = aPartitions[i].aFiles[j].c_str();
					GetShootFSFileData(filePath.c_str(), uncompressedData+fileOffset);
					fileOffset += fileWriteInfo[j].size;
				}

				// compress partition data
				Log << "Compressing " << (size_t)uncompressedDataSize/1024 << " KB\n";
				unsigned long compressedDataSizeL = unsigned long(uncompressedDataSize * 1.1) + 12;				
				compressedData = snew uchar[compressedDataSizeL];
				int result = compress(compressedData, &compressedDataSizeL, uncompressedData, uncompressedDataSize);
				SHOOT_ASSERT(result == Z_OK, "zlib compress failed");
				compressedDataSize = uint(compressedDataSizeL);
				sdelete_array(uncompressedData);
			}

			aCompressedData.push_back(compressedData);
			aCompressedDataSize.push_back(compressedDataSize);

			// write partition info
			FileSystem::PartitionInfoWrite partitionInfo;
			partitionInfo.offset = partitionOffset;
			partitionInfo.size = compressedDataSize;
			partitionInfo.uncompressedSize = uncompressedDataSize;
			partitionInfo.numFiles = numFiles;
			uint partitionNameLength = (uint)aPartitions[i].name.length();
			SHOOT_ASSERT(partitionNameLength < FileSystem::MaxPartitionNameLength, "Partition name too long (Max %d)", FileSystem::MaxPartitionNameLength);
			memcpy(partitionInfo.name, aPartitions[i].name.c_str(), partitionNameLength);
			partitionInfo.name[partitionNameLength] = '\0';

			pFile->Write(&partitionInfo, sizeof(FileSystem::PartitionInfoWrite));
			if(numFiles > 0)
			{
				pFile->Write(fileWriteInfo, numFiles*sizeof(FileSystem::FileInfoWrite));
				sdelete_array(fileWriteInfo);
			}

			partitionOffset += compressedDataSize;
			Log << "\n";
		}

		// write actual data		
		for (uint i = 0; i < numPartitions; ++i)
		{
			if(aCompressedDataSize[i] > 0)
				pFile->Write(aCompressedData[i], aCompressedDataSize[i]);			
			sdelete_array(aCompressedData[i]);
		}		

		pFile->Close();
		delete pFile;
	}

	//! reads the partitions
	void FileSystemGenerator::ReadPartitions(const std::string& strDirectory, std::vector<PartitionInfo>& aPartitions, int currentPartition /*= -1*/)
	{
		WIN32_FIND_DATA findData;
		std::string filter = strDirectory + "/" + "*.*";
		HANDLE hFind = FindFirstFile(filter.c_str(), &findData);

		if(hFind != INVALID_HANDLE_VALUE)
		{
			int currentDirectory = 0;
			do
			{
				if(std::string(findData.cFileName) != "."
				&& std::string(findData.cFileName) != ".."
				&& std::string(findData.cFileName) != ".svn"
				&& std::string(findData.cFileName) != "Thumbs.db")
				{
					int partition = currentPartition == -1 ? currentDirectory : currentPartition;
					if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						if(currentPartition == -1)
						{
							PartitionInfo info;
							info.name = findData.cFileName;
							aPartitions.push_back(info);
							m_aAtlasFiles.push_back(std::vector<std::string>());
						}
						std::string directory = strDirectory + "/" + findData.cFileName;
						ReadPartitions(directory, aPartitions, partition);
						currentDirectory++;
					}
					else if(currentPartition != -1)
					{
						std::string filePath = strDirectory + "/" + findData.cFileName;
						auto extIndex = filePath.rfind(".");
						std::string extension = filePath.substr(extIndex+1);
						if(extension == "bak")
							continue;						
						//if(extension == "png")
						//{
						//	m_aAtlasFiles[partition].push_back(filePath);
						//	continue;
						//}

						static auto dataLen = strlen("data/");
						filePath = filePath.substr(dataLen);
						std::transform(filePath.begin(), filePath.end(), filePath.begin(), tolower);
						aPartitions[partition].aFiles.push_back(filePath);
					}
				}
			} while(FindNextFile(hFind, &findData) != 0);
			FindClose(hFind);
		}		
	}	

	//! returns the ShootFS file size
	uint FileSystemGenerator::GetShootFSFileSize(const char* strPath) const
	{
		auto extension = Utils::GetFileExtension(strPath);
		if (Utils::Equals(extension.c_str(), "xml"))
		{
			auto file = File::CreateNative(strPath, File::M_ReadBinary);
			file->Open();
			auto fileSize = file->GetSize();
			file->Close();
			sdelete(file);
			return (uint)fileSize;
		}

		auto binPath = Utils::GetBinPath(strPath);
		auto dirty = true;
		if (File::Exists(binPath.c_str()))
			dirty = Utils::IsFileMoreRecent(strPath, binPath);

		File* binFile = NULL;
		uint fileSize = 0;
		if (dirty)
		{
			Log << "Converting " << strPath << "\n";
			binFile = Utils::CreateBinFile(binPath);
			if (Utils::Equals(extension.c_str(), "png"))
			{
				auto textureResourcePath = Utils::GetFilePathNoExt(strPath) + ".xml";
				bool _32Bits = false;
				if (File::Exists(textureResourcePath.c_str()))
				{
					auto textureResource = static_cast<Texture*>(ObjectManager::Instance()->Load(textureResourcePath));
					_32Bits = textureResource->Is32Bits();
					sdelete(textureResource);
				}				
				int width = 0, height = 0, channels = 0, bpp = 0;
				auto data = (uchar*)PNGLoader::LoadFromRawPng(strPath, width, height, channels, bpp, binFile, _32Bits);
				fileSize = (uint)binFile->GetOffset();
				sdelete_array(data);
				
			}
			else if (Utils::Equals(extension.c_str(), "fbx"))
			{
				std::string meshResourcePath = Utils::GetFilePathNoExt(strPath) + ".xml";
				auto meshResource = static_cast<MeshResource*>(ObjectManager::Instance()->Load(meshResourcePath));
				FBXMeshLoader(meshResource).Load(strPath, binFile, true);
				fileSize = (uint)binFile->GetOffset();
				sdelete(meshResource);
			}
#ifdef DX11
			else if (Utils::Equals(extension.c_str(), "hlsl"))
			{
				bool bVS = path.find("vs") != std::string::npos;
				bool bPS = path.find("ps") != std::string::npos;
				SHOOT_ASSERT(bVS || bPS, "Couldn't determine type of HLSL file: %s", strPath);
				const char* strEntryPoint = bVS ? "VSMain" : "PSMain";
				const char* strLevel = bVS ? "vs_4_0_level_9_3" : "ps_4_0_level_9_3";
				COMReference<ID3DBlob> blob = ShaderDX11::CompileHLSL(strPath, strEntryPoint, strLevel);
				fileSize += binFile->Write(static_cast<u8*>(blob->GetBufferPointer()), blob->GetBufferSize());
			}
#endif
			else
			{				
				auto file = File::CreateNative(strPath, File::M_ReadBinary);
				file->Open();
				fileSize = (uint)file->GetSize();
				auto data = new uchar[fileSize];
				file->Read(data, fileSize);				
				binFile->Write(data, fileSize);
				file->Close();
				sdelete(file);				
			}			
		}
		else
		{
			binFile = File::CreateNative(binPath.c_str(), File::M_ReadBinary);
			binFile->Open();
			fileSize = (uint)binFile->GetSize();
		}

		binFile->Close();
		sdelete(binFile);
		return fileSize;		
	}

	//! get ShootFS file data
	void FileSystemGenerator::GetShootFSFileData(const char* strPath, uchar* pDataOut) const
	{
		auto extension = Utils::GetFileExtension(strPath);
		if (Utils::Equals(extension.c_str(), "xml"))
		{
			auto file = File::CreateNative(strPath, File::M_ReadBinary);
			file->Open();			
			uint fileSize = (uint)file->GetSize();
			file->Read(pDataOut, fileSize);
			file->Close();
			sdelete(file);
			return;
		}
		
		auto binPath = Utils::GetBinPath(strPath);		
		auto binFile = File::CreateNative(binPath.c_str(), File::M_ReadBinary);
		binFile->Open();
		auto fileSize = binFile->GetSize();
		binFile->Read(pDataOut, fileSize);
		binFile->Close();
		sdelete(binFile);		
	}

	//! build the atlases
	void FileSystemGenerator::BuildAtlases(std::vector<PartitionInfo>& aPartitions)
	{
		SHOOT_ASSERT(m_aAtlasFiles.size() == aPartitions.size(), "Atlas build error");
		for (size_t i = 0; i<m_aAtlasFiles.size(); ++i)
		{
			for (size_t j = 0; j<m_aAtlasFiles[i].size(); ++j)
			{
				// TODO
			}
		}
	}
}
