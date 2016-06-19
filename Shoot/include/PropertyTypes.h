/* 

Amine Rehioui
Created: February 24th 2010

*/

#pragma once


#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Point.h"
#include "AABBox2D.h"
#include "AABBox3D.h"
#include "Size.h"
#include "Color.h"
#include "Range.h"
#include "Array.h"

namespace shoot
{
	//! Property types
	enum E_PropertyType
	{
		PT_Int,
		PT_UInt,
		PT_Float,
		PT_String,
		PT_Bool,
		PT_Vec2,
		PT_Vec3,
		PT_Point,
		PT_AABBox2D,
		PT_AABBox3D,
		PT_Size,
		PT_Enum,
		PT_File,
		PT_Array,
		PT_Struct,		
		PT_Color,
		PT_Link,
		PT_Range,
		PT_Reference,
		PT_Event,
		PT_WString,
		PT_Count,
		PT_Undefined
	};

	//! user defined structs must inherit from this to become serializable
	struct ISerializableStruct
	{
		//! destructor
		virtual ~ISerializableStruct()
		{
		}

		//! Reads/Writes struct properties from/to a stream
		virtual void Serialize(PropertyStream& stream) = 0;
	};

	//! Link interface
	class ILink
	{
	public:

		//! returns the Object ID
		virtual uint GetObjectID() const = 0;

		//! sets the Object ID
		virtual void SetObjectID(uint ID) = 0;

		//! returns the object class name
		virtual const char* GetClassName() const = 0;

		//! initializes the link
		void Init(Object* pParent);

	protected:

		//! sets the object pointer
		virtual void SetObject(Object* pObject) = 0;

		//! gets the object pointer
		virtual Object* GetObjectPtr() const = 0;
	};	

	//! String interface
	class IString
	{
	public:

		//! operators
		inline operator std::string() const { return m_String; }
		inline operator const char*() const { return m_String.c_str(); }
		inline bool operator == (const std::string& other) const { return m_String == other; }
		inline bool operator != (const std::string& other) const { return m_String != other; }
		inline const std::string* operator ->() const { return &m_String; }
		inline const std::string& GetString() const { return m_String; }
		
	protected:

		std::string m_String;
	};

	//! Event interface
	class IEvent : public IString
	{
	public:

		IEvent(const char* name)
		{
			m_String = name;
		}
	};

	//! Path interface
	class IPath : public IString
	{
	public:

		//! operators
		inline IPath& operator = (const char* str) { m_String = str; return *this; }
		inline IPath& operator = (const std::string& str) { m_String = str; return *this; }
	};

	//! enum params
	struct EnumParams
	{
		EnumParams(const char* const* pLiterals, const int* pValues = NULL)
			: m_pLiterals(pLiterals)
			, m_pValues(pValues)
		{
		}

		const char* const* m_pLiterals;
		const int* m_pValues;
	};

	//! serialization helpers
	inline E_PropertyType GetPropertyType(float* pData)					{ return PT_Float; }
	inline E_PropertyType GetPropertyType(uint* pValue)					{ return PT_UInt; }
	inline E_PropertyType GetPropertyType(int* pValue)					{ return PT_Int; }
	inline E_PropertyType GetPropertyType(std::string* pValue)			{ return PT_String; }
	inline E_PropertyType GetPropertyType(bool* pValue)					{ return PT_Bool; }
	inline E_PropertyType GetPropertyType(Vector2* pValue)				{ return PT_Vec2; }
	inline E_PropertyType GetPropertyType(Vector3* pValue)				{ return PT_Vec3; }
	inline E_PropertyType GetPropertyType(Point* pValue)				{ return PT_Point; }	
	inline E_PropertyType GetPropertyType(Color* pValue)				{ return PT_Color; }
	inline E_PropertyType GetPropertyType(Range* pValue)				{ return PT_Range; }
	inline E_PropertyType GetPropertyType(ISerializableStruct* pValue)	{ return PT_Struct; }
	inline E_PropertyType GetPropertyType(AABBox2D* pValue)				{ return PT_AABBox2D; }
	inline E_PropertyType GetPropertyType(AABBox3D* pValue)				{ return PT_AABBox3D; }
	inline E_PropertyType GetPropertyType(Size* pValue)					{ return PT_Size; }
	inline E_PropertyType GetPropertyType(ILink* pValue)				{ return PT_Link; }
	inline E_PropertyType GetPropertyType(IEvent* pValue)				{ return PT_Event; }
	inline E_PropertyType GetPropertyType(IPath* pValue)				{ return PT_File; }
	inline E_PropertyType GetPropertyType(std::wstring* pValue)			{ return PT_WString; }
}



