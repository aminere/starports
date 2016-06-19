/* 

Amine Rehioui
Created: April 15th 2011

*/

#pragma once


namespace shoot
{
	//! Object factory class
	class ObjectFactory : public Singleton<ObjectFactory>
	{
		DECLARE_SINGLETON(ObjectFactory);

	public:

		//! entity creation callback
		typedef Object* ObjectCreationCallback(void);

		//! creates an object from a class name
		/** \param strClassName: the object class name */
		Object* Create(const std::string& strClassName);

		//! register the Object type
		template <class ObjectType>
		void Register()
		{
			SHOOT_ASSERT(m_CreationInfo.find(ObjectType::GetStaticClassName()) == m_CreationInfo.end(), "Object already registered");
			m_CreationInfo[ObjectType::GetStaticClassName()] = ObjectType::Create;			

			RegisterClassInfo(ObjectType::GetStaticClassName(), ObjectType::GetStaticParentClassName());
		}

		//! creates an object given an index in the creatable object table
		/** \param typeIndex: type index of the object */
		Object* Create(int typeIndex);

		//! returns the number of registered creatable objects
		size_t GetNumCreatableTypes() const { return m_CreationInfo.size(); }

		//! returns an entity type name given its index
		const std::string& GetCreatableClassName(int typeIndex);

		//! returns the creatable child class names for a specific class name
		void GetCreatableChildClassNames(std::string className, std::vector<std::string>& childClassNames);	

		//! returns true if a type is a derived from another type
		bool IsDerived(std::string className, std::string parentClassName);

	private:

		int m_TypeCount;

		typedef std::map< std::string, ObjectCreationCallback* > CreationMap;
		typedef std::map< std::string, std::vector<std::string> > ClassMap;

		CreationMap m_CreationInfo;
		ClassMap m_ClassInfo;

		//! registers class information
		void RegisterClassInfo(std::string strClassName, std::string strParentClassName);

		//! register default objects
		void RegisterDefaultObjects();
	};
}



