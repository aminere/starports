/* 

Amine Rehioui
Created: April 26th 2014

*/

#include "Game.h"

#include "GameEntityProcessor.h"
#include "tinyxml2.h"

namespace shoot
{
	//! processes entities
	void GameEntityProcessor::Process()
	{
		ProcessEntities("data");
	}

	//! recursively process entities
	void GameEntityProcessor::ProcessEntities(const std::string& strDirectory)
	{
		WIN32_FIND_DATA findData;
		std::string filter = strDirectory + "/" + "*.*";
		HANDLE hFind = FindFirstFile(filter.c_str(), &findData);
		static std::map<std::string, u32> fileDuplicateChecker;

		if(hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if(std::string(findData.cFileName) == "."
				|| std::string(findData.cFileName) == ".."
				|| std::string(findData.cFileName) == ".svn"
				|| std::string(findData.cFileName) == "Thumbs.db")
				{
					continue;
				}

				if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					std::string directory = strDirectory + "/" + findData.cFileName;
					ProcessEntities(directory);
				}
				else
				{
					std::string fileName = findData.cFileName;
					s32 extIndex = fileName.rfind(".");
					std::string extension = fileName.substr(extIndex+1);
					if(extension != "xml"
					|| Utils::Equals(findData.cFileName, "audiomanager.xml")
					|| Utils::Equals(findData.cFileName, "gamemanager.xml")
					|| Utils::Equals(findData.cFileName, "inputcontroller.xml"))
					{
						continue;
					}

					std::string filePath = strDirectory + "/" + findData.cFileName;
					std::string dataFolder("data");
					size_t dataKeywordIndex = filePath.find(dataFolder);
					SHOOT_ASSERT(dataKeywordIndex != std::string::npos, "File not in data folder");
					filePath = filePath.substr(dataKeywordIndex+5);
					std::transform(filePath.begin(), filePath.end(), filePath.begin(), tolower);
					std::string className;
					Log.Print("Processing %s\n", filePath.c_str());
					Reference<Object> object = ObjectManager::Instance()->Get(filePath.c_str());
					if(Entity* pEntity = DYNAMIC_CAST(object.Get(), Entity))
					{
						// TODO custom processing / conversion
						pEntity->SaveToXML(filePath.c_str());
					}
				}
			} while(FindNextFile(hFind, &findData) != 0);
			FindClose(hFind);
		}
	}
}
