/* 

Amine Rehioui
Created: February 24th 2010

*/

#include "Shoot.h"

#include "PropertyStream.h"

#include "MaterialProvider.h"

#include "rapidxml.hpp"

namespace shoot
{
	//! Destructor
	PropertyStream::~PropertyStream()
	{
		Clear();	
	}

	//! reads/write from a property from this stream
	void PropertyStream::Serialize(int eType, const std::string& strName, void* pValue, const void *pParams /*= NULL*/)
	{
		SHOOT_ASSERT(eType != PT_Array, "Use SerializeArray for serializing arrays");
		SHOOT_ASSERT(eType != PT_Reference, "Use SerializeReference for serializing references");

		switch(m_eMode)
		{
		case SM_Read:

			// check if this property was marked for update
			if(m_strPropertyToUpdate.length() == 0
			|| m_strPropertyToUpdate == strName)
			{
				IProperty* pProperty = GetProperty(strName);
				if(pProperty)
				{
					SHOOT_ASSERT(pProperty->GetType() == eType, "PropertyType mismatch");
					if(pProperty->GetType() == PT_Struct)
					{
						StructProperty* pStructProperty = static_cast<StructProperty*>(pProperty);
						ISerializableStruct* pStruct = static_cast<ISerializableStruct*>(pValue);
						pStructProperty->GetStream().SetMode(SM_Read);
						pStructProperty->GetStream().SetTargetObject(m_pTargetObject);
						pStruct->Serialize(pStructProperty->GetStream());
					}
					else if(pProperty->GetType() == PT_Link)
					{
						if(m_pIDMap_OriginalToCopy)
						{
							Link<Object> link;
							pProperty->GetData(&link);
							if (m_pIDMap_OriginalToCopy->find(link.GetObjectID()) != m_pIDMap_OriginalToCopy->end())
								static_cast<ILink*>(pValue)->SetObjectID((*m_pIDMap_OriginalToCopy)[link.GetObjectID()]);
							else
								static_cast<ILink*>(pValue)->SetObjectID(link.GetObjectID());
						}
						else
							pProperty->GetData(pValue);
					}
					else
					{
						pProperty->GetData(pValue);
					}
				}
			}
			break;

		case SM_Write:
			{
				IProperty* pProperty = IProperty::CreateFromType(E_PropertyType(eType));
				pProperty->SetName(strName);
				pProperty->SetParams(pParams);

				if(pProperty->GetType() == PT_Struct)
				{
					StructProperty* pStructProperty = static_cast<StructProperty*>(pProperty);					
					ISerializableStruct* pStruct = static_cast<ISerializableStruct*>(pValue);
					pStructProperty->GetStream().SetMode(SM_Write);
					pStructProperty->GetStream().SetUsedForObjectCopy(m_bUsedForObjectCopy);
					pStruct->Serialize(pStructProperty->GetStream());
				}
				else
				{
					pProperty->SetData(pValue);	
				}

				m_aProperties.push_back(pProperty);
			}
			break;

		default:
			SHOOT_ASSERT(false, "Invalid E_SerializationMode");
		}

		if(eType == PT_Enum)
		{
			EnumParams* _pParams = static_cast<EnumParams*>(const_cast<void*>(pParams));
			sdelete(_pParams);
		}
	}

	//! returns the property with the specified name
	IProperty* PropertyStream::GetProperty(const std::string& strName) const
	{
		for (size_t i=0; i<m_aProperties.size(); ++i)
		{
			IProperty* pProperty = m_aProperties[i];
			if(pProperty->GetName() == strName)
				return pProperty;
		}
		return 0;
	}

	//! adds a property to the stream
	void PropertyStream::AddProperty(IProperty* pProperty)
	{
		std::vector<IProperty*>::iterator it = std::find(m_aProperties.begin(), m_aProperties.end(), pProperty);
		SHOOT_ASSERT(it == m_aProperties.end(), "Trying to add a property twice");
		m_aProperties.push_back(pProperty);
	}

	//! removes a propety from the stream
	void PropertyStream::RemoveProperty(IProperty* pProperty)
	{
		std::vector<IProperty*>::iterator it = std::find(m_aProperties.begin(), m_aProperties.end(), pProperty);
		SHOOT_ASSERT(it != m_aProperties.end(), "Trying to remove unexisting property");
		m_aProperties.erase(it);
		sdelete(pProperty);
	}

	//! fills this stream from xml element
	void PropertyStream::ReadFromXML(rapidxml::xml_node<char>* pXMLElement, ArrayProperty* pParentArrayProperty /*= NULL*/)	
	{		
		auto pChild = pXMLElement->first_node();
		while(pChild)
		{
			// get the property type name
			std::string strPropertyTypeName = pChild->name();
			IProperty* pProperty = IProperty::CreateFromTypeName(strPropertyTypeName);

			// get the property name
			const char* strPropertyName = pChild->getAttribute("Name");
			if(strPropertyName)
			{
				pProperty->SetName(std::string(strPropertyName));
			}

			switch(pProperty->GetType())
			{
			case PT_Array: 
				ReadFromXML(pChild, static_cast<ArrayProperty*>(pProperty));
				break;

			case PT_Reference:
				{
					ReferenceProperty* pRefProperty = static_cast<ReferenceProperty*>(pProperty);					
					if (const char* strTemplatePath = pChild->getAttribute("TemplatePath"))
					{
						pRefProperty->SetTemplatePath(strTemplatePath);
					}
					else if (const char* strClassName = pChild->getAttribute("ClassName"))
					{
						pRefProperty->SetClassName(strClassName);
						pRefProperty->GetStream().ReadFromXML(pChild);
					}
				}				
				break;

			case PT_Struct:
				{
					StructProperty* pStructProperty = static_cast<StructProperty*>(pProperty);
					pStructProperty->GetStream().ReadFromXML(pChild);
				}
				break;

			default:
				{
					// get the property value
					const char* pStrPropertyValue = pChild->getAttribute("Value");
					SHOOT_ASSERT(pStrPropertyValue, "property missing Value attribute");					
					pProperty->SetData(std::string(pStrPropertyValue));
				}
			}
			
			// add the property to the stream
			if(pParentArrayProperty)
			{				
				if(pParentArrayProperty->GetSubType() == PT_Undefined)
				{
					pParentArrayProperty->SetSubType(pProperty->GetType());
				}
				else
				{
					SHOOT_ASSERT(pParentArrayProperty->GetSubType() == pProperty->GetType(), "Can't have multiple types in an array property");
				}
				pParentArrayProperty->GetProperties().push_back(pProperty);
			}			
			else
			{
				m_aProperties.push_back(pProperty);
			}

			pChild = pChild->next_sibling();
		}
	}

	// writes this stream to xml element
	void PropertyStream::WriteToXML(rapidxml::xml_node<char>* pXMLElement)	
	{
		for (size_t i=0; i<m_aProperties.size(); ++i)
		{
			WritePropertyToXML(pXMLElement, m_aProperties[i]);			
		}		
	}	

	//! reads/writes an array property from/to this stream
	void PropertyStream::SerializeArray(const std::string& strName, IArray* pArray, int eSubType)
	{
		SHOOT_ASSERT(eSubType != PT_Array, "Arrays of arrays not supported. Encapsulate your inner array into a struct to get around this.");

		switch(m_eMode)
		{
		case SM_Read:

			// check if this property was marked for update
			if(m_strPropertyToUpdate.length() == 0
			|| m_strPropertyToUpdate == strName)
			{
				ArrayProperty* pProperty = static_cast<ArrayProperty*>(GetProperty(strName));
				if(pProperty)
				{
					// update existing elements					
					for (size_t i = 0; i<pArray->GetSize() && i<pProperty->GetProperties().size(); ++i)
					{
						IProperty* pSubProperty = pProperty->GetProperties()[i];
						SHOOT_ASSERT(pSubProperty->GetType() == eSubType, "Actual sub property type differs from expected type");
						void* pElement = pArray->GetPtr(i);
						FillArrayElement(pElement, pSubProperty);						
					}

					// attempt to add new elements
					for (size_t i = pArray->GetSize(); i<pProperty->GetProperties().size(); ++i)
					{
						IProperty* pSubProperty = pProperty->GetProperties()[i];
						SHOOT_ASSERT(pSubProperty->GetType() == eSubType, "Actual sub property type differs from expected type");						
						void* pElement = pArray->Alloc();
						FillArrayElement(pElement, pSubProperty);
						pArray->Add(pElement);
					}

					// attempt to remove elements
					for (size_t i = pProperty->GetProperties().size(); i<pArray->GetSize(); ++i)
					{
						pArray->Delete(i);
						--i;
					}
				}
			}
			break;

		case SM_Write:
			{
				ArrayProperty* pProperty = static_cast<ArrayProperty*>(IProperty::CreateFromType(PT_Array));
				pProperty->SetName(strName);
				pProperty->SetSubType(E_PropertyType(eSubType));
				pProperty->SetArray(pArray);

				for (size_t i = 0; i<pArray->GetSize(); ++i)
				{
					IProperty* pSubProperty = IProperty::CreateFromType(E_PropertyType(eSubType));
					void* pElement = pArray->GetPtr(i);

					if(eSubType == PT_Struct)
					{
						StructProperty* pStructProperty = static_cast<StructProperty*>(pSubProperty);						
						ISerializableStruct* pStruct = static_cast<ISerializableStruct*>(pElement);
						pStructProperty->GetStream().SetMode(SM_Write);
						pStruct->Serialize(pStructProperty->GetStream());
					}
					else if(eSubType == PT_Reference)
					{
						IReference* pReference = static_cast<IReference*>(pElement);
						ReferenceProperty* pRefProperty = static_cast<ReferenceProperty*>(pSubProperty);
						FillPropertyFromReference(pRefProperty, pReference);						
					}
					else
					{
						pSubProperty->SetData(pElement);
					}									

					pProperty->GetProperties().push_back(pSubProperty);
				}

				m_aProperties.push_back(pProperty);
			}
			break;		
		}
	}

	//! reads/writes a reference property from/to this stream
	void PropertyStream::SerializeReference(const std::string& strName, IReference* pReference)
	{
		switch(m_eMode)
		{
		case SM_Read:
			// check if this property was marked for update
			if(m_strPropertyToUpdate.length() == 0
			|| m_strPropertyToUpdate == strName)
			{
				IProperty* pProperty = GetProperty(strName);
				if(pProperty && pProperty->GetType() == PT_Reference)
				{
					FillReferenceFromProperty(pReference, static_cast<ReferenceProperty*>(pProperty));					
				}
			}
			break;

		case SM_Write:
			{
				ReferenceProperty* pProperty = static_cast<ReferenceProperty*>(IProperty::CreateFromType(PT_Reference));
				pProperty->SetName(strName);
				FillPropertyFromReference(pProperty, pReference);
				m_aProperties.push_back(pProperty);
			}
			break;
		}
	}

	//! explicitly clear this stream
	void PropertyStream::Clear()
	{
		for (size_t i=0; i<m_aProperties.size(); ++i)
		{
			IProperty* pProperty = m_aProperties[i];
			delete pProperty;
		}
		m_aProperties.clear();
	}

	//! writes a property to XML
	void PropertyStream::WritePropertyToXML(rapidxml::xml_node<char>* pXMLElement, IProperty* pProperty, bool bWriteName /*= true*/)
	{		
		auto pElement = pXMLElement->document()->allocate_node(rapidxml::node_element, pXMLElement->document()->allocate_string(pProperty->GetClassName().c_str()));
		pXMLElement->append_node(pElement);
		
		if(bWriteName)
			pElement->setAttribute("Name", pProperty->GetName().c_str());		
		
		switch(pProperty->GetType())
		{
		case PT_Array:
			{				
				ArrayProperty* pArrayProperty = static_cast<ArrayProperty*>(pProperty);
				for(size_t i=0; i<pArrayProperty->GetProperties().size(); ++i)
				{
					IProperty* pSubProperty = pArrayProperty->GetProperties()[i];
					WritePropertyToXML(pElement, pSubProperty, false); // don't write the name for array sub properties
				}	
			}
			break;

		case PT_Reference:
			{
				ReferenceProperty* pRefProperty = static_cast<ReferenceProperty*>(pProperty);
				if(pRefProperty->GetTemplatePath().empty())
				{
					pElement->setAttribute("ClassName", pRefProperty->GetClassName().c_str());
					pRefProperty->GetStream().WriteToXML(pElement);
				}
				else
				{
					pElement->setAttribute("TemplatePath", pRefProperty->GetTemplatePath().c_str());
				}				
			}
			break;

		case PT_Struct:
			{
				StructProperty* pStructProperty = static_cast<StructProperty*>(pProperty);
				pStructProperty->GetStream().WriteToXML(pElement);
			}
			break;

		default:
			{
				pElement->setAttribute("Value", pProperty->GetString().c_str());
			}
		}		
	}

	//! fills a user reference from a ReferenceProperty
	void PropertyStream::FillReferenceFromProperty(IReference* pReference, ReferenceProperty* pProperty)
	{
		bool bFromTemplate = !pProperty->GetTemplatePath().empty();
		bool bObjectSerialized = false;
		if (pProperty->GetStream().GetNumProperties() == 0 && !bFromTemplate)
			return;

		Object* pObject = pProperty->GetRefInterface() ? pProperty->GetRefInterface()->Get() : NULL;
		if (!pObject || m_bUsedForObjectCopy)
		{
			if (bFromTemplate)
			{
				if (!pObject)
				{
					auto templatePath = pProperty->GetTemplatePath();
					pObject = ObjectManager::Instance()->Find(templatePath);
					if (!pObject)
					{
						pObject = ObjectManager::Instance()->Load(templatePath);
						if (auto resource = DYNAMIC_CAST(pObject, Resource))
							resource->ResourceInit();
					}
				}
			}
			else
			{
				// hack because materials must only be created through MaterialProvider
				// TODO find a better way
				if (pReference->GetTypeID() == Material::TypeID)
				{
					if (m_bUsedForObjectCopy)
					{
						if (auto material = DYNAMIC_CAST(pObject, Material))						
							material->SetColor(material->GetCreationInfo().m_Color);						
					}
					else
					{
						Material::CreationInfo materialInfo;
						pProperty->GetStream().SetMode(SM_Read);
						pProperty->GetStream().Serialize("CreationInfo", &materialInfo);

						pObject = MaterialProvider::Instance()->FindMaterial(materialInfo);
						if (!pObject)
						{
							pObject = MaterialProvider::Instance()->CreateMaterial(materialInfo);

							uint ID = pObject->GetID();
							pProperty->GetStream().Serialize("ID", &ID);
							pObject->SetID(ID);
						}
					}
					bObjectSerialized = true;
				}
				else
				{
					pObject = ObjectFactory::Instance()->Create(pProperty->GetClassName());
				}
			}
		}

		#ifdef SHOOT_EDITOR
			pReference->SetOwner(m_pTargetObject);
			if (!ObjectFactory::Instance()->IsDerived(pObject->GetClassName(), pReference->GetClassName()))
			{
				SHOOT_WARNING(false, "Invalid Reference, expecting '%s', found '%s'", pReference->GetClassName(), pObject->GetClassName());
				return;
			}
		#endif

		pReference->SetObject(pObject);
		if (pProperty->GetStream().GetNumProperties() && !bObjectSerialized)
		{
			pProperty->GetStream().SetMode(SM_Read);
			pProperty->GetStream().SetIDMap_OriginalToCopy(m_pIDMap_OriginalToCopy);
			pProperty->GetStream().SetUsedForObjectCopy(m_bUsedForObjectCopy);

			int ID = pObject->GetID();
			pObject->Serialize(pProperty->GetStream());

			// If copied, use the ID from the new instance
			if (m_bUsedForObjectCopy)
				pObject->SetID(ID);
		}
	}

	//! fills a ReferenceProperty from a user reference
	void PropertyStream::FillPropertyFromReference(ReferenceProperty* pProperty, IReference* pReference)
	{
		pProperty->SetRefInterface(pReference);
		Object* pObject = pReference->Get();
		if(pObject)
		{
			std::string strTemplatePath = pObject->GetTemplatePath();
			if(!strTemplatePath.empty())
				pProperty->SetTemplatePath(strTemplatePath);			
			else
				pProperty->SetClassName(pObject->GetClassName());			

			pProperty->GetStream().SetMode(SM_Write);
			pProperty->GetStream().SetUsedForObjectCopy(m_bUsedForObjectCopy);
			pObject->Serialize(pProperty->GetStream());
		}						
		else
		{
			pProperty->SetClassName(pReference->GetClassName());
			pProperty->SetTemplatePath("");
		}
	}

	//! fills an array element from a property
	void PropertyStream::FillArrayElement(void* pElement, IProperty* pProperty)
	{
		if(pProperty->GetType() == PT_Struct)
		{
			StructProperty* pStructProperty = static_cast<StructProperty*>(pProperty);			
			ISerializableStruct* pStruct = static_cast<ISerializableStruct*>(pElement);		
			pStructProperty->GetStream().SetMode(SM_Read);
			pStruct->Serialize(pStructProperty->GetStream());
		}
		else if(pProperty->GetType() == PT_Reference)
		{
			IReference* pReference = static_cast<IReference*>(pElement);
			ReferenceProperty* pRefProperty = static_cast<ReferenceProperty*>(pProperty);
			FillReferenceFromProperty(pReference, pRefProperty);								
		}
		else
		{
			pProperty->GetData(pElement);
		}
	}

	//! assignment operator
	PropertyStream& PropertyStream::operator = (const PropertyStream& other)
	{
		Clear();
		for (size_t i = 0; i<other.GetNumProperties(); ++i)
		{
			IProperty* pPropertyCopy = other.GetProperty(i)->Copy();
			m_aProperties.push_back(pPropertyCopy);
		}
		return *this;
	}
}

