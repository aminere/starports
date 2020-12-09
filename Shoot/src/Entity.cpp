/* 

Amine Rehioui
Created: February 22th 2010

*/

#include "Shoot.h"

#include "EntityOperation.h"

#include "File.h"

#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

namespace shoot
{
	DEFINE_OBJECT(Entity);

	//! Constructor
	Entity::Entity() 
		: m_pParent(NULL)
		, m_pPendingParent(NULL)
		, m_bInitialized(false)
		, m_bSavableToXML(false)
		, m_OverridenProperties(SM_Read, this)
		// properties
		, m_bEnabled(true)
		, m_bVisible(true)
	{			
	}		

	//! Destructor
	Entity::~Entity()
	{
		Uninit();
	}

	//! serializes the entity to/from a PropertyStream
	void Entity::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);	

		stream.Serialize("Visible", &m_bVisible);		
		stream.Serialize("Enabled", &m_bEnabled);
	}

	//! called during the initialization of the entity
	void Entity::Init()
	{
		SHOOT_ASSERT(!m_bInitialized, "Trying to initialize an Entity twice");

		for(auto &child : m_aChildren)
			child->Init();

		for(auto &component : m_aComponents)
			component->Init();

		m_bInitialized = true;
	}

	//! Uninit
	void Entity::Uninit()
	{
		Clear();
		m_bInitialized = false;
	}

	//! adds a child to the entity
	void Entity::AddChild(Entity* pChild, bool bImmediate /*= false*/)
	{
		if(bImmediate)
		{
			Reference<Entity> child(pChild, this);			

			if(pChild->m_pParent)
				pChild->m_pParent->RemoveChild(pChild, true);			

			m_aChildren.push_back(child);
			pChild->SetParent(this);
		}
		else
		{
			pChild->m_pPendingParent = this;
			EntityOperationInfo info(EO_AddChild, this, pChild);
			Engine::Instance()->AddEntityOperation(info);
		}
	}

	//! inserts a child into a specific index
	void Entity::InsertChild(Entity* pChild, size_t index, bool bImmediate /*= false*/)
	{	
		if(bImmediate)
		{
			Reference<Entity> child(pChild, this);

			if(pChild->m_pParent)
				pChild->m_pParent->RemoveChild(pChild, true);

			if (index < m_aChildren.size())
				m_aChildren.insert(m_aChildren.begin() + index, child);
			else
				m_aChildren.push_back(child);

			pChild->SetParent(this);
		}
		else
		{
			pChild->m_pPendingParent = this;
			EntityOperationInfo info(EO_InsertChild, this, pChild, (int)index);
			Engine::Instance()->AddEntityOperation(info);
		}
	}

	//! removes a child from this entity
	void Entity::RemoveChild(Entity* pChild, bool bImmediate /*= true*/)
	{				
		if(bImmediate)
		{
			Reference<Entity> child(pChild);
			std::vector< Reference<Entity> >::iterator it = std::find(m_aChildren.begin(), m_aChildren.end(), child);
			SHOOT_ASSERT(it != m_aChildren.end(), "Trying to remove unexisting child");

			pChild->SetParent(NULL);
			m_aChildren.erase(it);
		}
		else
		{
			EntityOperationInfo info(EO_RemoveChild, this, pChild);
			Engine::Instance()->AddEntityOperation(info);
		}
	}	

	//! returns a child using its ID
	/** \param ID: ID of the wanted child */
	Entity* Entity::GetChildByID(uint ID) const
	{
		for(auto& child : m_aChildren)
		{
			if(child->GetID() == ID)
				return child.Get();
		}

		for(auto& child : m_aChildren)
		{
			if(auto grandChild = child->GetChildByID(ID))
				return grandChild;
		}

		return NULL;
	}

	//! returns a child using a template path
	Entity* Entity::GetChildByTemplate(const std::string& templatePath) const
	{
		for(auto &child : m_aChildren)
		{
			if(child->GetTemplatePath() == templatePath)
				return child.Get();
		}

		for(auto &child : m_aChildren)
		{
			auto grandChild = child->GetChildByTemplate(templatePath);
			if(grandChild)
				return grandChild;
		}

		return NULL;
	}

	//! returns a child using its name
	/** \param strName: name of the wanted child */
	Entity* Entity::GetChildByName(const std::string& strName, bool bRecursive /*= true*/) const
	{
		for(auto& child : m_aChildren)
		{
			if(child->GetName() == strName)
				return child.Get();
		}

		if(bRecursive)
		{
			for(auto& child : m_aChildren)
			{
				if(auto grandChild = child->GetChildByName(strName, true))
					return grandChild;
			}
		}

		return NULL;
	}

	//! returns the first child by type
	Entity* Entity::GetChildByType(const char* strTypeName) const
	{
		std::string typeName(strTypeName);
		for(auto& child : m_aChildren)
		{			
			if(std::string(child->GetClassName()) == strTypeName)
				return child.Get();
		}

		for(auto& child : m_aChildren)
		{
			if(auto grandChild = child->GetChildByType(strTypeName))
				return grandChild;
		}

		return NULL;
	}

	//! returns the root context
	Context* Entity::GetRootContext() const
	{
		return DYNAMIC_CAST(GetRoot(), Context);
	}

	//! returns the root entity
	Entity* Entity::GetRoot() const
	{
		if(GetParent() == NULL || IsA(Context::TypeID))
			return const_cast<Entity*>(this);
		else
			return GetParent()->GetRoot();
	}

	//! returns a pointer to a component		
	Component* Entity::GetComponent(const char* strName) const
	{
		for(auto& component : m_aComponents)
		{
			if(component->GetName() == strName)
				return component.Get();
		}
		return NULL;
	}

	//! adds a component to the entity
	void Entity::AddComponent(Component* pComponent, bool bImmediate /*= true*/)
	{
		if(bImmediate)
		{
			Reference<Component> component(pComponent, this);			
			std::vector< Reference<Component> >::iterator it = std::find(m_aComponents.begin(), m_aComponents.end(), component);
			SHOOT_ASSERT(it == m_aComponents.end(), "Trying to add component twice");
			m_aComponents.push_back(component);
			pComponent->SetOwner(this);
		}
		else
		{
			EntityOperationInfo info(EO_AddComponent, this, pComponent);
			Engine::Instance()->AddEntityOperation(info);
		}
	}

	//! removes a component from this entity
	void Entity::RemoveComponent(Component* pComponent, bool bImmediate /*= true*/)
	{		
		if(bImmediate)
		{
			Reference<Component> component(pComponent);
			std::vector< Reference<Component> >::iterator it = std::find(m_aComponents.begin(), m_aComponents.end(), component);
			SHOOT_ASSERT(it != m_aComponents.end(), "Trying to remove unexisting component");
			m_aComponents.erase(it);
		}
		else
		{
			EntityOperationInfo info(EO_RemoveComponent, this, pComponent);
			Engine::Instance()->AddEntityOperation(info);
		}
	}

	//! Loads the entity and its hierarchy from an XML
	void Entity::LoadFromXML(const char* strPath, Entity* pParent /*= NULL*/)
	{
		SHOOT_ASSERT(!m_bInitialized, "Calling LoadFromXML on an initialized Entity");
		rapidxml::xml_document<> document;
		try
		{
			document.parseFile<0>(strPath);
			ReadEntity(document.first_node(), pParent);
		}
		catch(rapidxml::parse_error e)
		{
			SHOOT_WARNING(false, "Could not load file '%s'", strPath);
		}
	}

	//! Loads the object from XML
	void Entity::LoadFromXML(rapidxml::xml_node<char>* pRoot)
	{
		SHOOT_ASSERT(!m_bInitialized, "Calling LoadFromXML on an initialized Entity");
		ReadEntity(pRoot, NULL);
	}

	//! Saves the entity and its hierarchy to an XML
	void Entity::SaveToXML(const char* strPath)
	{
		rapidxml::xml_document<> doc;
		auto root = doc.allocate_node(rapidxml::node_element, GetClassName());		
		doc.append_node(root);

		// if saving an entity, clear the template path to make a regular save
		ObjectManager::Instance()->SetTemplatePath(this, "");

		// write the entity
		WriteEntity(root, this);

		// set the template path
		ObjectManager::Instance()->SetTemplatePath(this, strPath);
		
		std::string xml_as_string;
		rapidxml::print(std::back_inserter(xml_as_string), doc);
		auto file = File::Create(strPath, shoot::File::M_Write);
		if(file->Open())
		{
			file->Write(xml_as_string.c_str(), (int)xml_as_string.length());
			file->Close();
		}
		sdelete(file);
	}

	//! recursive write to XML
	void Entity::WriteEntity(rapidxml::xml_node<char>* pXMLElement, Entity* pEntity)
	{
		// write properties
		PropertyStream stream(SM_Write);
		pEntity->Serialize(stream);

		std::string templatePath = pEntity->GetTemplatePath();
		bool bIsTemplateInstance = !templatePath.empty();
		if(bIsTemplateInstance)
		{
			// set template attribute
			pXMLElement->setAttribute("TemplatePath", templatePath.c_str());

			// for template entities only write overriden properties
			for (size_t i = 0; i<stream.GetNumProperties(); ++i)
			{
				IProperty* pProperty = stream.GetProperty(i);
				if(!pEntity->m_OverridenProperties.GetProperty(pProperty->GetName()))
				{
					stream.RemoveProperty(pProperty);
					--i;
				}
			}
		}

		if(stream.GetNumProperties() > 0)
		{
			auto pXMLProperties = pXMLElement->document()->allocate_node(rapidxml::node_element, "Properties");
			pXMLElement->append_node(pXMLProperties);
			stream.WriteToXML(pXMLProperties);
		}	

		// for now component and children definition/overriding is disabled for template instances
		if(!bIsTemplateInstance)
		{
			// write components
			auto numComponents = pEntity->GetComponentCount();
			if(numComponents > 0)
			{
				auto pXMLComponents = pXMLElement->document()->allocate_node(rapidxml::node_element, "Components");
				pXMLElement->append_node(pXMLComponents);
				PropertyStream stream(SM_Write);
				for(size_t i=0; i<numComponents; ++i)
					stream.Serialize("", &pEntity->m_aComponents[i]);
				stream.WriteToXML(pXMLComponents);
			}

			// write children
			for (size_t i = 0; i<pEntity->GetChildCount(); ++i)
			{
				Entity* pChild = pEntity->GetChild(i);
				if(pChild->m_bSavableToXML)
				{					
					auto pXMLChild = pXMLElement->document()->allocate_node(rapidxml::node_element, pChild->GetClassName());
					pXMLElement->append_node(pXMLChild);
					WriteEntity(pXMLChild, pChild);
				}				
			}
		}
	}

	//! recursive read from XML
	void Entity::ReadEntity(rapidxml::xml_node<char>* pXMLElement, Entity* pParent)
	{
		Entity* pEntity = NULL;
		const char* strClassName = pXMLElement->name();
		const char* strTemplatePath = pXMLElement->getAttribute("TemplatePath");

		if(pParent)
		{
			// create entity
			Object* pObject = ObjectFactory::Instance()->Create(strClassName);
			if(!pObject)			
				return;

			pEntity = DYNAMIC_CAST(pObject, Entity);
			SHOOT_ASSERT(pEntity, "Cannot create an Entity from Object");
		}
		else
		{
			auto c = GetClassName();
			SHOOT_ASSERT(std::string(GetClassName()) == strClassName, "Trying to load and entity into an entity of incompatible type");
			pEntity = this;
		}		

		pEntity->m_bSavableToXML = true;

		if(strTemplatePath)
		{
			ObjectManager::Instance()->SetTemplatePath(pEntity, strTemplatePath);
			pEntity->LoadFromXML(strTemplatePath);
		}
		
		// read properties
		auto pXMLProperties = pXMLElement->first_node();
		if(pXMLProperties)
		{
			SHOOT_ASSERT(std::string(pXMLProperties->name()) == std::string("Properties"), "properties tag not found"); 
			PropertyStream stream(SM_Read, pEntity);
			stream.ReadFromXML(pXMLProperties);

			// if this is a template instance, mark the properties as overriden and ignore the ID
			if(strTemplatePath)
			{
				for (size_t i = 0; i<stream.GetNumProperties(); ++i)
				{
					auto property = stream.GetProperty(i);
					if(property->GetName() == "ID")
					{
						stream.RemoveProperty(property);
						--i;
						continue;
					}
					
					pEntity->m_OverridenProperties.AddProperty(stream.GetProperty(i)->Copy());
				}
			}
			
			if(stream.GetNumProperties())
				pEntity->Serialize(stream);
		}
		else
		{
			SHOOT_ASSERT(strTemplatePath, "properties tag not found on an entity that is not a template instance");
		}

		if(pParent)
		{
			// Add entity after it has been serialized to ensure proper event handling inside of AddChild()
			pParent->AddChild(pEntity, true);
		}

		// read components
		auto pXMLChild = pXMLProperties ? pXMLProperties->next_sibling() : NULL;
		if(pXMLChild)
		{
			const char* strChildName = pXMLChild->name();
			if(std::string(strChildName) == "Components")
			{
				SHOOT_ASSERT(!strTemplatePath, "Entity template instance is not supposed to define components");
				PropertyStream stream(SM_Read, pEntity);
				stream.ReadFromXML(pXMLChild);
				for (size_t i = 0; i<stream.GetNumProperties(); ++i)
				{
					IProperty* pProperty = stream.GetProperty(i);
					SHOOT_ASSERT(pProperty->GetType() == PT_Reference, "Invalid Reference to Component");
					ReferenceProperty* pRefProperty = static_cast<ReferenceProperty*>(pProperty);
					Reference<Component> component;
					stream.FillReferenceFromProperty(&component, pRefProperty);

					Component* pComponent = component.Get();
					if(!pRefProperty->GetTemplatePath().empty())
					{
						// must add a unique component instance per entity, because components can't have multiple parents.
						pComponent = static_cast<Component*>(pComponent->Copy());
					}
					
					pEntity->AddComponent(pComponent, true);
				}
				pXMLChild = pXMLChild->next_sibling();
			}
		}

		// read children		
		while(pXMLChild)
		{
			SHOOT_ASSERT(!strTemplatePath, "Entity template instance is not supposed to define children");
			ReadEntity(pXMLChild, pEntity);
			pXMLChild = pXMLChild->next_sibling();
		}
	}

	//! clears children and components
	void Entity::Clear()
	{
		m_aComponents.clear();

		for(auto& child : m_aChildren)
			child->SetParent(NULL);

		m_aChildren.clear();
	}

	//! sets the parent of this entity
	void Entity::SetParent(Entity* pParent) 
	{ 
		m_pParent = pParent;
		m_pPendingParent = NULL;
	}
}

