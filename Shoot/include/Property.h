/* 

Amine Rehioui
Created: February 24th 2010

*/

#pragma once


namespace shoot
{
	// forwards
	class Object;

	// serialization operators
	std::stringstream& operator << (std::stringstream& ss, const Vector2& vec2D);
	std::stringstream& operator >> (std::stringstream& ss, Vector2& vec2D);
	std::stringstream& operator << (std::stringstream& ss, const Vector3& vec3D);
	std::stringstream& operator >> (std::stringstream& ss, Vector3& vec3D);
	std::stringstream& operator << (std::stringstream& ss, const Vector4& vec4D);
	std::stringstream& operator >> (std::stringstream& ss, Vector4& vec4D);
	std::stringstream& operator << (std::stringstream& ss, const Point& point);
	std::stringstream& operator >> (std::stringstream& ss, Point& point);
	std::stringstream& operator << (std::stringstream& ss, const AABBox2D& bbox2D);
	std::stringstream& operator >> (std::stringstream& ss, AABBox2D& bbox2D);
	std::stringstream& operator << (std::stringstream& ss, const AABBox3D& bbox3D);
	std::stringstream& operator >> (std::stringstream& ss, AABBox3D& bbox3D);
	std::stringstream& operator << (std::stringstream& ss, const Size& size);
	std::stringstream& operator >> (std::stringstream& ss, Size& size);
	std::stringstream& operator << (std::stringstream& ss, const Color& color);
	std::stringstream& operator >> (std::stringstream& ss, Color& color);
	std::stringstream& operator << (std::stringstream& ss, const Range& range);
	std::stringstream& operator >> (std::stringstream& ss, Range& range);
	std::stringstream& operator << (std::stringstream& ss, const std::wstring& wstr);
	std::stringstream& operator >> (std::stringstream& ss, std::wstring& wstr);
	
	//! Property interface
	/** A property is used to expose a variable to XML and to visual editors */
	class IProperty
	{
	public:
			
		//! Destructor
		virtual ~IProperty()
		{
		}
			
		//! returns the string representation of this property		
		virtual std::string GetString() const = 0;

		//! sets the data of this property from a string
		virtual void SetData(const std::string& strData) = 0;

		//! returns the data stored in this property
		virtual bool GetData(void* pData) const = 0;

		//! sets the data of this property from a void pointer
		virtual void SetData(const void* pData) = 0;

		//! returns the type of this property
		inline E_PropertyType GetType() const { SHOOT_ASSERT(m_eType != PT_Undefined, "Undefined Property type"); return m_eType; }

		//! copies this property
		virtual IProperty* Copy() const = 0;

		//! return a copy of the data from this property
		virtual void* CopyData() const = 0;

		//! passes extra params to the property
		virtual void SetParams(const void* pParams) { }

		//! returns the type name of this property
		std::string GetClassName() const;

		//! sets the name of this property
		void SetName(const std::string& strName) { m_strName = strName; }

		//! returns the name of this property
		const std::string& GetName() const { return m_strName; }

		//! maximum size of the string representation of properties
		static const int MaxStringSize = 256;

		//! creates a property from a type enum
		static IProperty* CreateFromType(E_PropertyType eType);

		//! creates a property from a type name
		static IProperty* CreateFromTypeName(const std::string& strType);

	protected:

		//! Constructor
		IProperty()
			: m_eType(PT_Undefined)
		{
		}

		std::string m_strName;
		E_PropertyType m_eType;
	};
	
	// Property class containing a data of type DataType
	template <class DataType>
	class Property : public IProperty
	{
	public:

		//! returns the string representation of this property			
		virtual std::string GetString() const
		{
			std::stringstream ss(std::stringstream::in | std::stringstream::out);
			char buf[MaxStringSize];
			ss << m_Data;
			ss.get(buf, MaxStringSize);
			return std::string(buf);
		}

		//! sets the data of this property from a string		
		virtual void SetData(const std::string& strData)
		{
			std::stringstream ss(std::stringstream::in | std::stringstream::out);
			ss << strData;
			ss >> m_Data;
		}

		//! returns the data stored in this property
		virtual bool GetData(void* pData) const
		{ 
			bool bChanged = (*static_cast<DataType*>(pData) != m_Data);
			*static_cast<DataType*>(pData) = m_Data;
			return bChanged;
		}

		//! sets the data of this property from a void pointer
		virtual void SetData(const void* pData) { m_Data = *static_cast<const DataType*>(pData); }	

		//! copies this property
		virtual IProperty* Copy() const
		{
			Property<DataType>* pCopy = snew Property<DataType>();
			*pCopy = *this;
			return pCopy;
		}

		//! return a copy of the data from this property
		virtual void* CopyData() const
		{
			auto copy = snew DataType;
			*copy = m_Data;
			return copy;
		}

	protected:

		DataType m_Data;
	};

	//! Unsigned Integer property class
	class UIntProperty : public Property<uint>
	{
	public:

		//! returns the string representation of this property			
		virtual std::string GetString() const
		{
			std::stringstream ss(std::stringstream::in | std::stringstream::out);
			char buf[MaxStringSize];
			ss << std::hex << m_Data;
			ss.get(buf, MaxStringSize);
			return std::string(buf);
		}

		//! sets the data of this property from a string		
		virtual void SetData(const std::string& strData)
		{
			std::stringstream ss(std::stringstream::in | std::stringstream::out);
			ss << std::hex << strData;
			ss >> m_Data;
		}
	};

	//! String property class
	class StringProperty : public Property<std::string>
	{
	public:

		//! sets the data of this property from a string		
		void SetData(const std::string& strData) override
		{ 
			m_Data = strData; 
		}
	};

	//! String property class
	class WStringProperty : public Property<std::wstring>
	{
	public:

		//! sets the data of this property from a string		
		void SetData(const std::string& strData) override 
		{ 
			m_Data = std::wstring(strData.begin(), strData.end()); 
		}
	};

	//! Color property class
	class ColorProperty : public Property<Color>
	{
	public:
		//! Constructor
		ColorProperty()
		{
			m_Data = Color::White;
		}
	};

	//! Macro to pass EnumProperty params
	#ifdef SHOOT_EDITOR
		#define ENUM_PARAMS2(literals, values) snew EnumParams(literals, values)
		#define ENUM_PARAMS1(literals) ENUM_PARAMS2(literals, 0)
	#else
		#define ENUM_PARAMS2(literals, values) 0
		#define ENUM_PARAMS1(literals) 0
	#endif // SHOOT_EDITOR

	//! Enum property class
	class EnumProperty : public Property<int>
	{
	public:

		//! Constructor
		EnumProperty() : m_Params(NULL, NULL)
		{
			m_Data = 0;
		}

		//! passes extra params to the property
		void SetParams(const void* pParams) 
		{
			if(pParams)
				m_Params = *static_cast<const EnumParams*>(pParams);		
		}

		//! returns the enumeration literals
		const char* const* GetLiterals() const { return m_Params.m_pLiterals; }

		//! returns the enumeration values
		const int* GetValues() const { return m_Params.m_pValues; }

		//! copies this property
		virtual IProperty* Copy() const
		{
			auto copy = snew EnumProperty();
			*copy = *this;
			return copy;
		}

	private:

		EnumParams m_Params;
	};

	//! File property class
	class FileProperty : public StringProperty
	{
	public:
		//! Constructor
		FileProperty() : m_strFilter(NULL)
		{			
			m_eType = PT_File;
		}

		//! copies this property
		virtual IProperty* Copy() const
		{
			auto copy = snew FileProperty();
			*copy = *this;
			return copy;
		}

		//! passes extra params to the property
		void SetParams(const void* pParams) 
		{ 
			m_strFilter = static_cast<const char*>(pParams);
		}

		const char* GetFilter() const { return m_strFilter; }

	private:

		const char* m_strFilter;
	};

	//! Array property class
	/** Contains an array of other properties */
	class ArrayProperty : public IProperty
	{
	public:
		
		//! Constructor
		ArrayProperty() : m_eSubType(PT_Undefined), m_pArray(NULL)
		{
		}

		//! Destructor
		virtual ~ArrayProperty()
		{
			for (size_t i=0; i<m_aProperties.size(); ++i)
			{
				IProperty* pProperty = m_aProperties[i];
				delete pProperty;
			}
		}

		//! returns the string representation of this property		
		std::string GetString() const { return ""; }

		//! sets the data of this property from a string
		void SetData(const std::string& strData) { }

		//! returns the data stored in this property
		bool GetData(void* pData) const { return false; }

		//! sets the data of this property from a void pointer
		virtual void SetData(const void* pData) { }

		//! returns the properties
		std::vector<IProperty*>& GetProperties() { return m_aProperties; }

		//! returns the properties
		const std::vector<IProperty*>& GetProperties() const { return m_aProperties; }

		//! sets the subtype of this property
		void SetSubType(E_PropertyType eSubType) { m_eSubType = eSubType; }

		//! returns the subtype
		E_PropertyType GetSubType() const { return m_eSubType; } 

		//! sets the array interface
		void SetArray(IArray* pArray) { m_pArray = pArray; }

		//! returns the array interface
		IArray* GetArray() { return m_pArray; }

		//! copies this property
		virtual IProperty* Copy() const
		{
			ArrayProperty* pCopy = snew ArrayProperty();
			pCopy->m_strName = m_strName;
			pCopy->m_eType = m_eType;
			pCopy->m_eSubType = m_eSubType;
			pCopy->m_pArray = m_pArray;
			for (size_t i=0; i<m_aProperties.size(); ++i)
			{
				IProperty* pPropertyCopy = m_aProperties[i]->Copy();
				pCopy->m_aProperties.push_back(pPropertyCopy);
			}
			return pCopy;
		}

		//! return a copy of the data from this property
		virtual void* CopyData() const { return NULL; }

	private:

		std::vector<IProperty*> m_aProperties;
		E_PropertyType m_eSubType;
		IArray* m_pArray;
	};

	//! Struct property class
	/** represents a user defined structure that can be composed of any property type 
		Example use:

		struct MyStruct : public ISerializableStruct
		{
			int myInt;
			std::string myString;
			MyOtherStruct myOtherStruct;

			void Serialize(PropertyStream& stream)
			{
				stream.Serialize("MyInt", &myInt);
				stream.Serialize("MyString", &myString);
				stream.Serialize("MyOtherStruct", &myOtherStruct);
			}
		};

		MyStruct myStruct;

		PropertyStream.Serialize("MyStruct", &myStruct);
	*/
	class StructProperty : public IProperty
	{
	public:
		
		//! Constructor
		StructProperty() : m_Stream(SM_Read)
		{
		}

		//! returns the string representation of this property		
		std::string GetString() const { return ""; }

		//! sets the data of this property from a string
		void SetData(const std::string& strData) { }

		//! returns the data stored in this property
		bool GetData(void* pData) const { return false; }

		//! sets the data of this property from a void pointer
		virtual void SetData(const void* pData) { }

		//! returns the property stream 
		PropertyStream& GetStream() { return m_Stream; }

		//! copies this property
		virtual IProperty* Copy() const
		{
			StructProperty* pCopy = snew StructProperty();
			*pCopy = *this;
			return pCopy;
		}

		//! return a copy of the data from this property
		virtual void* CopyData() const { return NULL; }		

	protected:

		PropertyStream m_Stream;
	};

	//! Link property class
	class LinkProperty : public IProperty
	{
	public:
		//! Constructor
		LinkProperty() : m_pLink(NULL), m_ID(0)
		{
		}

		//! returns the string representation of this property		
		std::string GetString() const
		{ 
			std::stringstream ss(std::stringstream::in | std::stringstream::out);
			char buf[MaxStringSize];
			ss << std::hex << m_ID;
			ss.get(buf, MaxStringSize);
			return std::string(buf);			
		}

		//! sets the data of this property from a string
		void SetData(const std::string& strData)
		{ 
			std::stringstream ss(std::stringstream::in | std::stringstream::out);
			ss << std::hex << strData;
			ss >> m_ID;
		}

		//! returns the data stored in this property
		bool GetData(void* pData) const
		{
			bool bChanged = (static_cast<ILink*>(pData)->GetObjectID() != m_ID);
			static_cast<ILink*>(pData)->SetObjectID(m_ID);
			return bChanged;
		}

		//! sets the data of this property from a void pointer
		void SetData(const void* pData)
		{
			m_pLink = static_cast<const ILink*>(pData);
			m_ID = m_pLink->GetObjectID();
		}

		//! returns the link interface
		const ILink* GetLink() const { return m_pLink; }

		//! copies this property
		virtual IProperty* Copy() const
		{
			LinkProperty* pCopy = snew LinkProperty();
			*pCopy = *this;
			return pCopy;
		}

		//! return a copy of the data from this property
		virtual void* CopyData() const { return NULL; }

	private:

		const ILink* m_pLink;
		uint m_ID;
	};

	//! Concrete link template
	template<class T>
	class Link : public ILink
	{
	public:

		//! constructor
		Link() : m_ID(0)
		{
		}

		//! returns the Object ID
		uint GetObjectID() const { return m_ID; }

		//! sets the Object ID
		void SetObjectID(uint ID) { m_ID = ID; }

		//! returns the object class name
		const char* GetClassName() const { return T::GetStaticClassName(); }

		//! returns the target object
		T* Get() { return m_Object; }

		//! returns the target object
		T* Get() const { return m_Object; }

		//! -> operator
		inline T* operator ->()
		{
			return m_Object;
		}

		//! const -> operator
		inline T* operator ->() const
		{
			return m_Object;
		}

		//! T* cast operator
		inline operator T*()
		{
			return m_Object;
		}

		//! const T* cast operator
		inline operator T*() const
		{
			return m_Object;
		}

		//! clears the link
		void Clear()
		{
			m_Object = NULL;
			m_ID = 0;
		}

		//! sets the object pointer
		void SetObject(Object* pObject)
		{
			m_Object = (pObject && pObject->IsA(T::TypeID)) ? static_cast<T*>(pObject) : NULL;
			if(pObject)
			{
				SHOOT_ASSERT(m_Object.IsValid(), "Invalid Link Object type (expected '%s', got '%s'", T::GetStaticClassName(), pObject->GetClassName());
			}
			m_ID = pObject ? m_Object->GetID() : 0;
		}

		//! gets the object pointer
		Object* GetObjectPtr() const { return m_Object; }

	private:

		Handle<T> m_Object;
		uint m_ID;
	};

	// forwards
	class IReference;

	//! Reference property class
	class ReferenceProperty : public StructProperty
	{
	public:

		//! constructor
		ReferenceProperty() : m_pRefInterface(NULL)
		{
		}
			
		//! sets the reference interface
		inline void SetRefInterface(IReference* pRefInterface) { m_pRefInterface = pRefInterface; }

		//! returns the reference interface
		inline IReference* GetRefInterface() { return m_pRefInterface; }

		//! returns the class name of the referenced object
		std::string GetClassName() const { return m_strClassName; }

		//! sets the base class name
		inline void SetClassName(std::string strClassName) { m_strClassName = strClassName; }

		//! sets the source template path
		inline void SetTemplatePath(const std::string& strPath) { m_strTemplatePath = strPath; }

		//! returns the source template
		inline const std::string& GetTemplatePath() const { return m_strTemplatePath; }

		//! copies this property
		virtual IProperty* Copy() const
		{
			ReferenceProperty* pCopy = snew ReferenceProperty();
			*pCopy = *this;
			return pCopy;
		}

	private:
				
		IReference* m_pRefInterface;
		std::string m_strClassName;
		std::string m_strTemplatePath;
	};

	//! Event property class
	class EventProperty : public StringProperty
	{
	public:
		//! Constructor
		EventProperty()
		{
			m_eType = PT_Event;
		}
	};
}



