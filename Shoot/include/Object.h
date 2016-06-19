/* 

Amine Rehioui
Created: April 2nd 2011

*/

#pragma once


namespace rapidxml
{
	template<class Ch> class xml_node;
}

namespace shoot
{
	class PropertyStream;

	//! Base Object class
	class Object
	{
	public:

		//! Constructor
		Object();

		//! Destructor
		virtual ~Object();

		//! serializes the object to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! returns the ID of this entity
		inline uint GetID() const { return m_ID; }

		//! sets the ID of this entity
		inline void SetID(uint ID) { m_ID = ID; }

		//! returns the name of this entity
		inline const std::string& GetName() const { return m_strName; }

		//! sets the name of this entity
		inline void SetName(const std::string& strName) { m_strName = strName; }

		//! Loads the object from XML
		virtual void LoadFromXML(rapidxml::xml_node<char>* pRoot);

		//! returns the object's template path
		const std::string& GetTemplatePath() const;

		//! create a copy of the object
		virtual Object* Copy() const;

		//! Saves the object to XML
		virtual void SaveToXML(const char* strPath);

		//! Macro to provide type information
		#define DECLARE_OBJECT(ClassName, ParentClassName) \
		typedef ParentClassName super; \
		public: \
		virtual const char* GetClassName() const { return ClassName::GetStaticClassName(); } \
		static const char* GetStaticClassName() { return #ClassName; } \
		static const char* GetStaticParentClassName() { return #ParentClassName; } \
		static Object* Create() { return snew ClassName(); } \
		virtual bool IsA(int typeID) const \
		{ \
			if(ClassName::TypeID == typeID) return true; \
			else \
			{ \
				if(ClassName::TypeID == Object::TypeID) return false; \
				else return super::IsA(typeID); \
			} \
		} \
		virtual int GetTypeID() const { return ClassName::TypeID; } \
		static int TypeID;

		#define DEFINE_OBJECT(ClassName) int ClassName::TypeID = (m_TypeCount++);

		#define DYNAMIC_CAST(object, TargetClassName) ((object && object->IsA(TargetClassName::TypeID)) ? static_cast<TargetClassName*>(object) : 0)

		DECLARE_OBJECT(Object, Object);

	protected:

		static int m_TypeCount;

		// properties
		uint m_ID;
		std::string m_strName;
	};
}



