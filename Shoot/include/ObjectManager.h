/* 

Amine Rehioui
Created: April 2nd 2011

*/

#pragma once


namespace shoot
{
	// forwards
	class Mutex;

	//! Object manager class
	class ObjectManager : public Singleton<ObjectManager>
	{
		DECLARE_SINGLETON(ObjectManager);

	public:

		//! registers an object within the manager and assigns a random ID to it		
		void RegisterObject(Object* pObject);		

		//! removes an object from the manager
		void UnregisterObject(Object* pObject);

		//! load an object from a path
		Object* Load(const std::string& path);

		//! tries to find an existing object or loads a new one if not found
		Object* FindOrLoad(const std::string& path);

		//! tries to find an existing object
		Object* Find(const std::string& path);
		
		//! create a copy of the object
		Object* CopyObject(Object* pObject);

		//! registers a reference to an object
		void RegisterReference(IReference* reference, Object* pObject);

		//! unregisters a reference (to an object)
		void UnregisterReference(IReference* reference, Object* pObject);

		//! registers a handle to an object
		void RegisterHandle(Object* pObject, IHandle* pHandle);

		//! unregisters a handle (to an object)
		void UnregisterHandle(Object* pObject, IHandle* pHandle);

		//! Object Handler signature
		typedef void ObjectHandler(void* pListener, Object* pObject);

		//! ObjectListener
		struct ObjectListener
		{
			ObjectListener(void* _pListener, ObjectHandler* _pHandler)
				: pListener(_pListener)
				, pHandler(_pHandler)
			{
			}

			void* pListener;
			ObjectHandler* pHandler;
		};

		//! object container
		struct ObjectContainer
		{
			std::string strPath;
			std::list<IHandle*> lHandles;
			std::list<IReference*> lReferences;

			bool operator == (const ObjectContainer& other) const
			{
				return (lReferences.size() == other.lReferences.size()) && (strPath == other.strPath);
			}

			bool operator != (const ObjectContainer& other) const
			{
				return !operator == (other);
			}
		};

		//! returns an object container from an object pointer
		ObjectContainer* GetContainer(Object* pObject);

		//! returns an object container from an object pointer
		ObjectContainer* GetContainer(const Object* pObject);

		//! sets an object template path
		void SetTemplatePath(Object* pObject, const std::string& path);

		//! gets an object template path
		const std::string& GetTemplatePath(Object* pObject) const;

		typedef std::map<Object*, ObjectContainer> ObjectMap;

	private:

		//! create a copy of the object
		Object* InternalCopyObject(Object* pObject, std::map<int, int>& IDMap_OriginalToCopy);
		
		//! updates an object ref count
		size_t UpdateRefCount(IReference* reference, Object* pObject, int amount);

		ObjectMap m_ObjectMap;
		std::map< std::string, Handle<Object> > _templateToObjectMap;
		Mutex* m_pMutex;

#ifdef SHOOT_EDITOR
	public:
		ObjectMap& GetObjectMap() { return m_ObjectMap; }
#endif // SHOOT_EDITOR
	};
}



