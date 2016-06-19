/* 

Amine Rehioui
Created: April 28th 2012

*/

#include "Shoot.h"
#include "FileSystemGenerator.h"
#include "MemoryManager.h"
#include "FBXMeshLoader.h"
#include "ObjectFactory.h"
#include "GraphicExtensionHandler.h"

#include "File.h"

int main(int argc, char** argv)
{
	shoot::HighResTimer timer;
	timer.Start();

	shoot::MemoryManager::Init();
	shoot::ObjectManager::CreateInstance();
	shoot::ResourceManager::CreateInstance();
	shoot::GraphicsDriver::CreateInstance();
	shoot::GraphicExtensionHandler::CreateInstance();
	shoot::ObjectFactory::CreateInstance();
	shoot::File::SetUseDataFolder(true);

	shoot::FileSystemGenerator().Generate();

	shoot::GraphicsDriver::DestroyInstance();
	shoot::ResourceManager::DestroyInstance();
	shoot::ObjectFactory::DestroyInstance();
	shoot::ObjectManager::DestroyInstance();
	shoot::FBXMeshLoader::Uninit();
	shoot::MemoryManager::Uninit();

	auto msec = timer.GetDuration() / 1000;
	shoot::Log << "Duration: " << (float)msec / 1000.0f << " secs\n";
	shoot::Log << "Success!\n";
	shoot::Log.Close();
	system("pause");
	return 0;
}

