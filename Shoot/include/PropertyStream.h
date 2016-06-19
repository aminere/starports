/* 

Amine Rehioui
Created: February 24th 2010

*/

#pragma once


namespace rapidxml
{
	template<class Ch> class xml_node;
}

namespace shoot
{
	//! specifies if the PropertyStream is to be read from / written to
	enum E_SerializationMode
	{
		SM_Read,
		SM_Write
	};

	class IProperty;
	class IArray;
	class IReference;
	class ArrayProperty;
	class ReferenceProperty;

	//! Class to encapsulate properties and communicate with XML
	class PropertyStream
	{
	public:		

		//! Constructor
		PropertyStream(E_SerializationMode eMode, Object* targetObject = NULL) 
			: m_eMode(eMode)
			, m_bUsedForObjectCopy(false)
			, m_pIDMap_OriginalToCopy(NULL)
			, m_pTargetObject(NULL)
		{
		}

		//! Destructor
		~PropertyStream();

		//! serialization helpers
		template<class T> void Serialize(const std::string& strName, T* pData) { Serialize(GetPropertyType(pData), strName, pData); }
		template<class T> void Serialize(const std::string& strName, Reference<T>* pData) { SerializeReference(strName, pData); }
		template<class T> void Serialize(const std::string& strName, Array<T>* pArray) { T* dummy = NULL; SerializeArray(strName, pArray, GetPropertyType(dummy)); }
		template<class T> void Serialize(const std::string& strName, Array< Reference<T> >* pArray) { SerializeArray(strName, pArray, PT_Reference); }
		inline void Serialize(const std::string& strName, void* pValue, EnumParams* pParams) { Serialize(PT_Enum, strName, pValue, pParams); }

		#define SERIALIZE(x) stream.Serialize(#x, &##x)
		
		//! returns the property with the specified name
		IProperty* GetProperty(const std::string& strName) const;

		//! returns the property at the specified index
		inline IProperty* GetProperty(size_t index) const { return m_aProperties[index]; }

		//! adds a property to the stream
		void AddProperty(IProperty* pProperty);

		//! removes a propety from the stream
		void RemoveProperty(IProperty* pProperty);

		//! returns the number of properties stored in this stream
		inline size_t GetNumProperties() const { return m_aProperties.size(); }

		//! fills this stream from an xml element
		void ReadFromXML(rapidxml::xml_node<char>* pXMLElement, ArrayProperty* pParentArrayProperty = NULL);

		//! writes this stream to an xml element
		void WriteToXML(rapidxml::xml_node<char>* pXMLElement);

		//! changes the serialization mode
		inline void SetMode(E_SerializationMode eMode) { m_eMode = eMode; }

		//! returns the serialization mode
		inline E_SerializationMode GetMode() const { return m_eMode; }

		//! marks one property for update
		void SetPropertyToUpdate(const std::string& strName) { m_strPropertyToUpdate = strName; }

		//! explicitly clear this stream
		void Clear();

		//! fills a user reference from a ReferenceProperty
		void FillReferenceFromProperty(IReference* pReference, ReferenceProperty* pProperty);

		//! fills a ReferenceProperty from a user reference
		void FillPropertyFromReference(ReferenceProperty* pProperty, IReference* pReference);

		//! assignment operator
		PropertyStream& operator = (const PropertyStream& other);

		//! returns the properties array
		std::vector<IProperty*>& GetProperties() { return m_aProperties; }

		//! set this flag when the stream is used for object copy
		inline void SetUsedForObjectCopy(bool bSet) { m_bUsedForObjectCopy = bSet; }

		//! returns object copy status
		inline bool IsUsedForObjectCopy() const { return m_bUsedForObjectCopy; }

		//! set this flag when the stream is used for object copy
		inline void SetIDMap_OriginalToCopy(std::map<int, int>* pMap) { m_pIDMap_OriginalToCopy = pMap; }

		//! SetTargetObject
		inline void SetTargetObject(Object* target) { m_pTargetObject = target; }

	private:

		//! reads/write a property from/to this stream
		void Serialize(int eType, const std::string& strName, void* pValue, const void *pParams = NULL);

		//! reads/writes an array property from/to this stream
		void SerializeArray(const std::string& strName, IArray* pArray, int eSubType);

		//! reads/writes a reference property from/to this stream
		void SerializeReference(const std::string& strName, IReference* pReference);

		//! writes a property to XML
		void WritePropertyToXML(rapidxml::xml_node<char>* pXMLElement, IProperty* pProperty, bool bWriteName = true);

		//! fills an array element from a property
		void FillArrayElement(void* pElement, IProperty* pProperty);

		std::vector<IProperty*> m_aProperties;
		E_SerializationMode m_eMode;
		std::string m_strPropertyToUpdate;
		bool m_bUsedForObjectCopy;
		std::map<int, int>* m_pIDMap_OriginalToCopy;
		Object* m_pTargetObject;
	};
}

