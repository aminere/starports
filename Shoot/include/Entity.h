/* 

Amine Rehioui
Created: February 22th 2010

*/

#pragma once


namespace rapidxml
{
	template<class Ch> class xml_node;
}

namespace shoot
{	
	// forwards
	class Context;

	//! Base class for all game entities	
	class Entity : public Object
	{
	public:

		DECLARE_OBJECT(Entity, Object);

		//! Constructor
		Entity();

		//! Destructor
		virtual ~Entity();
	
		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		virtual void Init();

		//! Uninit
		void Uninit();

		//! called during the update of the entity
		virtual void Update() { }
			
		//! registers the entity and its children for rendering
		virtual void RegisterForRendering() { }

		//! returns the children
		inline const std::vector< Reference<Entity> >& GetChildren() const { return m_aChildren; }

		//! returns the number of children 
		inline size_t GetChildCount() const { return m_aChildren.size(); }

		//! returns a pointer to a child
		/** \param index: index of the wanted child */
		inline Entity* GetChild(size_t index) const { return m_aChildren[index]; }

		//! returns a child using its ID
		/** \param ID: ID of the wanted child */
		Entity* GetChildByID(uint ID) const;

		//! returns a child using its name
		/** \param strName: name of the wanted child */
		Entity* GetChildByName(const std::string& strName, bool bRecursive = true) const;

		//! returns the first child by type
		Entity* GetChildByType(const char* strTypeName) const;

		//! returns a child using a template path
		Entity* GetChildByTemplate(const std::string& templatePath) const;

		//! return the first child of a type
		template <class T>
		T* GetChildByType() const
		{
			for(auto& child : m_aChildren)
			{
				if(T* _child = DYNAMIC_CAST(child.Get(), T))
					return _child;
			}

			// check grand children
			for(auto& child : m_aChildren)
			{
				if(T* grandChild = child->GetChildByType<T>())
					return grandChild;
			}

			return NULL;
		}

		//! returns children by type
		template <class T>
		void GetChildrenByType(std::vector< Handle<T> >& aChildren) const
		{
			for(auto& child : m_aChildren)
			{
				if(child->IsA(T::TypeID))
					aChildren.push_back(Handle<T>(static_cast<T*>(child.Get())));
			}

			// check grand children
			for(auto& child : m_aChildren)
				child->GetChildrenByType<T>(aChildren);
		}

		//! adds a child to the entity
		virtual void AddChild(Entity* pChild, bool bImmediate = false);

		//! inserts a child into a specific index
		virtual void InsertChild(Entity* pChild, size_t index, bool bImmediate = false);

		//! removes a child from this entity
		virtual void RemoveChild(Entity* pChild, bool bImmediate = false);
			
		//! returns the parent of this entity
		inline Entity* GetParent() const { return m_pParent ? m_pParent : m_pPendingParent; }

		//! returns the first ancestor with a certain type
		template <class T>
		T* GetAncestor() const
		{
			if(!m_pParent)
				return NULL;
			
			if(T* pParent = DYNAMIC_CAST(m_pParent, T))
				return pParent;
			else
				return m_pParent->GetAncestor<T>();
		}

		//! returns the root context
		Context* GetRootContext() const;

		//! returns the root entity
		Entity* GetRoot() const;

		//! returns the number of components 
		inline size_t GetComponentCount() const { return m_aComponents.size(); }

		//! returns a pointer to a component
		/** \param index: index of the wanted component */
		inline Component* GetComponent(size_t index) const { return m_aComponents[index]; }

		//! returns a pointer to a component		
		Component* GetComponent(const char* strName) const;

		//! gets a component by type
		template <class T> 
		T* GetComponent() const
		{ 
			for(auto& component : m_aComponents)
			{
				if(auto c = DYNAMIC_CAST(component.Get(), T))
					return c;
			}
			return NULL;
		}

		//! adds a component to the entity
		virtual void AddComponent(Component* pComponent, bool bImmediate = false);

		//! removes a component from this entity
		virtual void RemoveComponent(Component* pComponent, bool bImmediate = false);

		inline const std::vector< Reference<Component> >& GetComponents() const { return m_aComponents; }

		//! gets components by type
		template <class T>
		void GetComponents(std::vector<T*>& components, bool recursive = false)
		{
			for(auto& component : m_aComponents)
			{
				if(T* pComponent = DYNAMIC_CAST(component.Get(), T))				
					components.push_back(pComponent);				
			}			

			if(!recursive)
				return;

			for(auto &child : m_aChildren)
				child->GetComponents<T>(components);
		}

		//! returns if this entity is enabled or not.
		/** Disabled entities are not updated */
		inline bool IsEnabled() const { return m_bEnabled; }

		//! enables / disables this entity
		inline void SetEnabled(bool bEnabled) { m_bEnabled = bEnabled; }

		//! returns the visibility status of this entity
		inline bool IsVisible() const { return m_bVisible; }

		//! sets the visibility status of this entity
		inline void SetVisible(bool bVisible) { m_bVisible = bVisible; }

		//! SetVisibleAndEnabled
		inline void SetVisibleAndEnabled(bool set)
		{
			SetVisible(set);
			SetEnabled(set);
		}

		//! Loads the entity and its hierarchy from an XML
		void LoadFromXML(const char* strPath, Entity* pParent = NULL);

		//! Loads the object from XML
		virtual void LoadFromXML(rapidxml::xml_node<char>* pRoot);

		//! returns the initialization status
		inline bool IsInitialized() const { return m_bInitialized; }

		//! Saves the object to XML
		virtual void SaveToXML(const char* strPath);

		//! recursive write to XML
		void WriteEntity(rapidxml::xml_node<char>* pXMLElement, Entity* pEntity);

		//! set savable to XML flag
		inline void SetSavableToXML(bool bSavable) { m_bSavableToXML = bSavable; }

		//! gets savable to XML flag
		inline bool GetSavableToXML() const { return m_bSavableToXML; }

		//! returns the overriden properties stream
		inline PropertyStream& GetOverridenProperties() { return m_OverridenProperties; }

		//! clears children and components
		void Clear();

	private:

		//! sets the parent of this entity
		void SetParent(Entity* pParent);

		//! recursive read from XML
		void ReadEntity(rapidxml::xml_node<char>* pXMLElement, Entity* pParent);
	
		std::vector< Reference<Component> > m_aComponents;

		Entity* m_pParent;
		Entity* m_pPendingParent;
		
	protected:

		std::vector< Reference<Entity> > m_aChildren;

		bool m_bInitialized;
		bool m_bSavableToXML;

		// properties
		bool m_bEnabled;
		bool m_bVisible;

		PropertyStream m_OverridenProperties;
	};
}



