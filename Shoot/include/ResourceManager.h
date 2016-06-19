/* 

Amine Rehioui
Created: November 19th 2011

*/

#pragma once


#include "Resource.h"

namespace shoot
{
	//! Resource manager class
	class ResourceManager : public Singleton<ResourceManager>
	{
		DECLARE_SINGLETON(ResourceManager);

	public:

		//! returns a resource from a path
		template <class T>
		T* GetResource(const std::string& path);

		//! request a resource from a path
		void RequestReloadResource(const std::string& path);

		//! updates the resource manager
		void Update();

		//! reload all resources
		void ReloadResources();

		//! called when a resource is created through ObjectManager::LoadObject
		void OnResourceCreated(Resource* pResource);

		//! called when a resource is destroyed
		void OnResourceDestroyed(Resource* pResource);
		
	private:

		//! reloads a resource from a path
		void ReloadResource(const char* strPath);

		//! resource file info
		struct FileInfo
		{
			//! constructor
			FileInfo(const std::string& path);

			float m_fLastWriteTimer;
			std::string m_Path;
		};

		std::list<Resource*> m_lResources;
		std::list<FileInfo> m_lResourcesToReload;
		Mutex* m_pMutex;
	};
}



