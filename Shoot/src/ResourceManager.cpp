/* 

Amine Rehioui
Created: July 10th 2010

*/

#include "Shoot.h"

#include "Utils.h"
#include "Mutex.h"
#include "CubeMapResource.h"

#include "rapidxml.hpp"

namespace shoot
{
	template Shader* ResourceManager::GetResource<Shader>(const std::string&);
	template Texture* ResourceManager::GetResource<Texture>(const std::string&);
	template MeshResource* ResourceManager::GetResource<MeshResource>(const std::string&);
	template CubeMapResource* ResourceManager::GetResource<CubeMapResource>(const std::string&);

	//! Constructor
	ResourceManager::ResourceManager()
	{
		m_pMutex = Mutex::Create();
	}

	//! Destructor
	ResourceManager::~ResourceManager()
	{
		SHOOT_ASSERT(m_lResources.empty(), "Resource Leak Detected");
		sdelete(m_pMutex);
	}

	//! returns a resource from a path
	template <class T>
	T* ResourceManager::GetResource(const std::string& path)
	{
		if (auto resource = ObjectManager::Instance()->Find(path))
			return static_cast<T*>(resource);
		else
		{
			resource = ObjectManager::Instance()->Load(path);
			if (!resource)
				return NULL;

			auto newResource = DYNAMIC_CAST(resource, T);
			SHOOT_ASSERT(newResource, "Trying to load a resource of type '%s' into an instance of type '%s'", resource->GetClassName(), T::GetStaticClassName());
			newResource->ResourceInit();
			return newResource;
		}
	}

	//! request a resource from a path
	void ResourceManager::RequestReloadResource(const std::string& path)
	{
		if(path.find("noreload") != std::string::npos)		
			return;

		Log.Print("RequestReloadResource: %s\n", path.c_str());				
		for(std::list<FileInfo>::iterator it = m_lResourcesToReload.begin(); it != m_lResourcesToReload.end(); ++it)
		{
			if((*it).m_Path == path)
			{
				(*it).m_fLastWriteTimer = 0.0f;
				return;
			}
		}
		
		m_lResourcesToReload.push_back(FileInfo(path));
	}

	//! updates the resource manager
	void ResourceManager::Update()
	{
		for(auto it = m_lResourcesToReload.begin(); it != m_lResourcesToReload.end(); )
		{
			if((*it).m_fLastWriteTimer > 3.0f)
			{
				ReloadResource((*it).m_Path.c_str());
				it = m_lResourcesToReload.erase(it);
			}
			else
			{
				(*it).m_fLastWriteTimer += g_fDeltaTime;
				++it;
			}
		}
	}

	//! reload all resources
	void ResourceManager::ReloadResources()
	{
		MeshResource::MeshReload meshReload;
		for(auto resource : m_lResources)
			resource->ResourceInit();
	}

	//! called when a resource is created through ObjectManager::LoadObject
	void ResourceManager::OnResourceCreated(Resource* pResource)
	{
		m_lResources.push_back(pResource);
	}		

	//! called when a resource is destroyed
	void ResourceManager::OnResourceDestroyed(Resource* pResource)
	{
		auto it = std::find(m_lResources.begin(), m_lResources.end(), pResource);
		SHOOT_ASSERT(it != m_lResources.end(), "Trying to destroy an unknown resource");
		m_lResources.erase(it);
	}

	//! reloads a resource from a path
	void ResourceManager::ReloadResource(const char* strPath)
	{
		for(auto resource : m_lResources)
		{
			if(!(resource->GetTemplatePath() == strPath || resource->IsUsingData(strPath)))
				continue;

			Log << "ReloadResource " << strPath << "...\n";

			resource->ResourceUninit();

			// redo serialization
			if(Utils::GetFileExtension(strPath) == "xml")
			{
				rapidxml::xml_document<> document;
				try
				{
					document.parseFile<0>(strPath);
					auto root = document.first_node();
					PropertyStream stream(SM_Read, resource);
					stream.ReadFromXML(root);
					resource->Serialize(stream);
				}
				catch(rapidxml::parse_error e)
				{
					SHOOT_ASSERT(false, "Could not load file '%s'", strPath);
				}				
			}

			if (auto meshResource = DYNAMIC_CAST(resource, MeshResource))
			{
				MeshResource::MeshReload meshReload;
				meshResource->ResourceInit();
			}
			else 
				resource->ResourceInit();
			return;
		}
	}

	//! FileInfo constructor
	ResourceManager::FileInfo::FileInfo(const std::string& path)
		: m_Path(path)
		, m_fLastWriteTimer(0.0f)
	{
	}
}

