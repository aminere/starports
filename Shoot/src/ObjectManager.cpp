/* 

Amine Rehioui
Created: April 2nd 2011

*/

#include "Shoot.h"

#include "Mutex.h"

#include "rapidxml.hpp"

namespace shoot
{
	//! Constructor
	ObjectManager::ObjectManager() 
	{
		m_pMutex = Mutex::Create();
	}

	//! Destructor
	ObjectManager::~ObjectManager()
	{
		SHOOT_ASSERT(m_ObjectMap.empty(), "Object Leaks Detected");
		for(auto it = m_ObjectMap.begin(); it != m_ObjectMap.end(); ++it)
		{
			auto type = (*it).first->GetClassName();
			Log << "Object type: " << type << "\n";
		}
		sdelete(m_pMutex);
	}

	//! registers an object within the manager and assigns a random ID to it		
	void ObjectManager::RegisterObject(Object* pObject)
	{
		SHOOT_ASSERT(m_ObjectMap.find(pObject) == m_ObjectMap.end(), "Trying to register an object twice");
		m_ObjectMap[pObject] = ObjectContainer();

		uint ID = Random::NextU32();
		pObject->SetID(ID);
	}

	//! removes an object from the manager
	void ObjectManager::UnregisterObject(Object* pObject)
	{
		ObjectContainer& container = m_ObjectMap[pObject];		
		SHOOT_ASSERT(container.lReferences.empty(), "Trying to delete an object that is still referenced");
		
		for(std::list<IHandle*>::iterator it = container.lHandles.begin(); it != container.lHandles.end(); ++it)		
			(*it)->ClearByObjectManager();
		
		m_ObjectMap.erase(pObject);
	}

	//! load an object from a path
	Object* ObjectManager::Load(const std::string& path)
	{
		SHOOT_ASSERT(Utils::GetFileExtension(path) == "xml", "Invalid Object file");
		rapidxml::xml_document<> document;
		try
		{
			document.parseFile<0>(path);
		}
		catch (rapidxml::parse_error e)
		{
			SHOOT_ASSERT(false, "Couldn't load object '%s'", path.c_str());
			return NULL;
		}

		auto root = document.first_node();
		auto className = root->name();
		auto object = ObjectFactory::Instance()->Create(className);

		if (!object)
			return NULL;

		SetTemplatePath(object, path);
		object->LoadFromXML(root);
		return object;
	}

	//! tries to find an existing object or loads a new one if not found
	Object* ObjectManager::FindOrLoad(const std::string& path)
	{
		if (auto object = Find(path))
			return object;

		return Load(path);
	}

	//! tries to find an existing object
	Object* ObjectManager::Find(const std::string& path)
	{
		auto it = _templateToObjectMap.find(path);
		if (it != _templateToObjectMap.end())
		{
			if (it->second.IsValid())
				return it->second.Get();
			else
				_templateToObjectMap.erase(it);
		}

		return NULL;
	}

	//! registers a reference to an object
	void ObjectManager::RegisterReference(IReference* reference, Object* pObject)
	{
		UpdateRefCount(reference, pObject, 1);
	}

	//! unregisters a reference (to an object)
	void ObjectManager::UnregisterReference(IReference* reference, Object* pObject)
	{
		auto refCount = UpdateRefCount(reference, pObject, -1);
		if(refCount == 0)
			sdelete(pObject);
	}

	//! registers a handle to an object
	void ObjectManager::RegisterHandle(Object* pObject, IHandle* pHandle)
	{
		//Mutex::_Lock lock(m_pMutex);
		ObjectContainer* pContainer = GetContainer(pObject);
		SHOOT_ASSERT(std::find(pContainer->lHandles.begin(), pContainer->lHandles.end(), pHandle) == pContainer->lHandles.end(), "Duplicate Object Handle detected");
		pContainer->lHandles.push_back(pHandle);		
	}

	//! unregisters a handle (to an object)
	void ObjectManager::UnregisterHandle(Object* pObject, IHandle* pHandle)
	{
		//Mutex::_Lock lock(m_pMutex);
		ObjectContainer* pContainer = GetContainer(pObject);
		std::list<IHandle*>::iterator it = std::find(pContainer->lHandles.begin(), pContainer->lHandles.end(), pHandle);
		SHOOT_ASSERT(it != pContainer->lHandles.end(), "Object '%s' Handle not found", pObject->GetClassName());
		pContainer->lHandles.erase(it);		
	}

	//! returns an object container from an object pointer
	ObjectManager::ObjectContainer* ObjectManager::GetContainer(Object* pObject)
	{
		auto it = m_ObjectMap.find(pObject);
		SHOOT_ASSERT(it != m_ObjectMap.end(), "Objet '%s' container not found", pObject->GetClassName());
		return &(it->second);
	}

	//! returns an object container from an object pointer
	ObjectManager::ObjectContainer* ObjectManager::GetContainer(const Object* pObject)
	{
		return GetContainer(const_cast<Object*>(pObject));
	}

	//! sets an object template path
	void ObjectManager::SetTemplatePath(Object* pObject, const std::string& path)
	{
		//Mutex::_Lock lock(m_pMutex);
		auto container = GetContainer(pObject);
		auto oldPath = container->strPath;
		container->strPath = path;

		if (!oldPath.empty())
		{
			auto it = _templateToObjectMap.find(oldPath);
			if (it != _templateToObjectMap.end())
				_templateToObjectMap.erase(it);
		}

		_templateToObjectMap[path] = pObject;
	}

	//! gets an object template path
	const std::string& ObjectManager::GetTemplatePath(Object* pObject) const
	{		
		//Mutex::_Lock lock(m_pMutex);
		const std::string& path = const_cast<ObjectManager*>(this)->GetContainer(pObject)->strPath;
		return path;
	}

	//! updates an object ref count
	size_t ObjectManager::UpdateRefCount(IReference* reference, Object* pObject, int amount)
	{		
		//Mutex::_Lock lock(m_pMutex);
		auto c = GetContainer(pObject);
		auto& r = c->lReferences;
		if (amount > 0)
		{
			SHOOT_ASSERT(std::find(r.begin(), r.end(), reference) == r.end(), "Duplicate reference detected");
			r.push_back(reference);
		}
		else
		{
			auto it = std::find(r.begin(), r.end(), reference);
			SHOOT_ASSERT(it != r.end(), "Object referer not found");
			r.erase(it);
		}

		return r.size();
	}

	//! create a copy of the object
	Object* ObjectManager::CopyObject(Object* pObject)
	{
		std::map<int, int> IDMap_OriginalToCopy;
		return InternalCopyObject(pObject, IDMap_OriginalToCopy);
	}

	//! create a copy of the object
	Object* ObjectManager::InternalCopyObject(Object* pObject, std::map<int, int>& IDMap_OriginalToCopy)
	{
		Object* pCopy = ObjectFactory::Instance()->Create(pObject->GetClassName());

		// copy all the properties from the source object
		int ID = pCopy->GetID();
		PropertyStream stream(SM_Write, pCopy);
		stream.SetUsedForObjectCopy(true);
		pObject->Serialize(stream);
		stream.SetMode(SM_Read);
		stream.SetIDMap_OriginalToCopy(&IDMap_OriginalToCopy);
		pCopy->Serialize(stream);
		IDMap_OriginalToCopy[pCopy->GetID()] = ID;
		pCopy->SetID(ID);

		// if the object is an entity, copy children and components
		if(Entity* pEntity = DYNAMIC_CAST(pObject, Entity))
		{
			Entity* pEntityCopy = DYNAMIC_CAST(pCopy, Entity);

#ifdef SHOOT_EDITOR
			pEntityCopy->SetSavableToXML(pEntity->GetSavableToXML());

			// copy overriden property statuses
			for (size_t i=0; i<stream.GetNumProperties(); ++i)
			{
				if(pEntity->GetOverridenProperties().GetProperty(stream.GetProperty(i)->GetName()))
				{
					pEntityCopy->GetOverridenProperties().AddProperty(stream.GetProperty(i)->Copy());
				}
			}
#endif // SHOOT_EDITOR

			// copy children
			auto numChildren = pEntity->GetChildCount();
			for (size_t i = 0; i<numChildren; ++i)
			{
				Object* pChild = pEntity->GetChild(i);
				Object* pChildCopy = InternalCopyObject(pChild, IDMap_OriginalToCopy);
				pEntityCopy->AddChild(DYNAMIC_CAST(pChildCopy, Entity), true);
			}

			// copy components
			auto numComponents = pEntity->GetComponentCount();
			for (size_t i = 0; i<numComponents; ++i)
			{
				Object* pComponent = pEntity->GetComponent(i);
				Object* pComponentCopy = InternalCopyObject(pComponent, IDMap_OriginalToCopy);				
				pEntityCopy->AddComponent(static_cast<Component*>(pComponentCopy), true);
			}
		}

		// copy container path
		{
			//Mutex::_Lock lock(m_pMutex);
			ObjectContainer* pDestContainer = GetContainer(pCopy);
			ObjectContainer* pSrcContainer = GetContainer(pObject);
			pDestContainer->strPath = pSrcContainer->strPath;
		}

		return pCopy;
	}
}

