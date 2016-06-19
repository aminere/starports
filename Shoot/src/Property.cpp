/* 

Amine Rehioui
Created: February 24th 2010

*/

#include "Shoot.h"

namespace shoot
{
	std::map<std::string, E_PropertyType> PropertyTypeFromString()
	{
		std::map<std::string, E_PropertyType> m;
		m[std::string("int")] = PT_Int;
		m[std::string("uint")] = PT_UInt;
		m[std::string("float")] = PT_Float;
		m[std::string("string")] = PT_String;
		m[std::string("bool")] = PT_Bool;
		m[std::string("vec2D")] = PT_Vec2;
		m[std::string("vec3D")] = PT_Vec3;
		m[std::string("point")] = PT_Point;
		m[std::string("aabb2D")] = PT_AABBox2D;
		m[std::string("aabb3D")] = PT_AABBox3D;
		m[std::string("size")] = PT_Size;
		m[std::string("enum")] = PT_Enum;
		m[std::string("file")] = PT_File;
		m[std::string("array")] = PT_Array;
		m[std::string("struct")] = PT_Struct;
		m[std::string("color")] = PT_Color;
		m[std::string("link")] = PT_Link;
		m[std::string("range")] = PT_Range;
		m[std::string("ref")] = PT_Reference;
		m[std::string("event")] = PT_Event;
		m[std::string("wstring")] = PT_WString;
		return m;
	}

	std::map<std::string, E_PropertyType> g_PropertyTypeFromString = PropertyTypeFromString();

	std::map<E_PropertyType, std::string> PropertyStringFromType()
	{
		std::map<E_PropertyType, std::string> m;
		for (auto it = g_PropertyTypeFromString.begin(); it != g_PropertyTypeFromString.end(); ++it)
			m[it->second] = it->first;
		return m;
	}

	std::map<E_PropertyType, std::string> g_PropertyStringFromType = PropertyStringFromType();

	template <class T> IProperty* CreateProperty() { return snew Property<T>(); }
	IProperty* CreateUInt() { return snew UIntProperty(); }
	IProperty* CreateString() { return snew StringProperty(); }
	IProperty* CreateEnum() { return snew EnumProperty(); }
	IProperty* CreateFileP() { return snew FileProperty(); }
	IProperty* CreateArray() { return snew ArrayProperty(); }
	IProperty* CreateStruct() { return snew StructProperty(); }
	IProperty* CreateColor() { return snew ColorProperty(); }
	IProperty* CreateLink() { return snew LinkProperty(); }
	IProperty* CreateReference() { return snew ReferenceProperty(); }
	IProperty* CreateEventP() { return snew EventProperty(); }
	IProperty* CreateWString() { return snew WStringProperty(); }

	typedef IProperty* PropertyCreator();
	PropertyCreator* g_PropertyCreators[PT_Count] = 
	{
		&CreateProperty<int>,		// PT_Int
		&CreateUInt,				// PT_UInt
		&CreateProperty<float>,		// PT_Float
		&CreateString,				// PT_String
		&CreateProperty<bool>,		// PT_Bool
		&CreateProperty<Vector2>,	// PT_Vec2
		&CreateProperty<Vector3>,	// PT_Vec3
		&CreateProperty<Point>,		// PT_Point
		&CreateProperty<AABBox2D>,	// PT_AABBox2D
		&CreateProperty<AABBox3D>,	// PT_AABBox3D
		&CreateProperty<Size>,		// PT_Size
		&CreateEnum,				// PT_Enum
		&CreateFileP,				// PT_File
		&CreateArray,				// PT_Array
		&CreateStruct,				// PT_Struct
		&CreateColor,				// PT_Color
		&CreateLink,				// PT_Link
		&CreateProperty<Range>,		// PT_Range
		&CreateReference,			// PT_Reference
		&CreateEventP,				// PT_Event
		&CreateWString				// PT_WString
	};

	//! creates a property from a type enum-
	IProperty* IProperty::CreateFromType(E_PropertyType eType)
	{
		IProperty* pProperty = g_PropertyCreators[eType]();
		pProperty->m_eType = eType;
		return pProperty;
	}

	//! creates a property from a type name: TODO use a map to look up
	IProperty* IProperty::CreateFromTypeName(const std::string& strType)
	{
		auto it = g_PropertyTypeFromString.find(strType);
		if (it == g_PropertyTypeFromString.end())
		{
			SHOOT_ASSERT(0, "Invalid property type name '%s'", strType.c_str());
			return NULL;
		}

		auto type = (*it).second;
		return CreateFromType(type);
	}

	//! returns the type name of this property
	std::string IProperty::GetClassName() const
	{
		return g_PropertyStringFromType[GetType()];
	}

	//! operator to format Vector2 into a string stream
	std::stringstream& operator << (std::stringstream& ss, const Vector2& vec2D)
	{
		ss << vec2D.X << " " << vec2D.Y;
		return ss;
	}

	//! operator to build Vector2 from a string stream
	std::stringstream& operator >> (std::stringstream& ss, Vector2& vec2D)
	{
		ss >> vec2D.X >> vec2D.Y;
		return ss;
	}

	//! operator to format Vector3 into a string stream
	std::stringstream& operator << (std::stringstream& ss, const Vector3& vec3D)
	{
		ss << vec3D.X << " " << vec3D.Y << " " << vec3D.Z;
		return ss;
	}

	//! operator to build Vector3 from a string stream
	std::stringstream& operator >> (std::stringstream& ss, Vector3& vec3D)
	{
		ss >> vec3D.X >> vec3D.Y >> vec3D.Z;
		return ss;
	}

	//! operator to format Vector4 into a string stream
	std::stringstream& operator << (std::stringstream& ss, const Vector4& vec4D)
	{
		ss << vec4D.X << " " << vec4D.Y << " " << vec4D.Z << " " << vec4D.W;
		return ss;
	}

	//! operator to build Vector4 from a string stream
	std::stringstream& operator >> (std::stringstream& ss, Vector4& vec4D)
	{
		ss >> vec4D.X >> vec4D.Y >> vec4D.Z >> vec4D.W;
		return ss;
	}


	//! operator to format Point into a string stream
	std::stringstream& operator << (std::stringstream& ss, const Point& point)
	{
		ss << point.X << " " << point.Y;
		return ss;
	}

	//! operator to build Point from a string stream
	std::stringstream& operator >> (std::stringstream& ss, Point& point)
	{
		ss >> point.X >> point.Y;
		return ss;
	}

	//! operator to format AABBox2D into a string stream
	std::stringstream& operator << (std::stringstream& ss, const AABBox2D& bbox2D)
	{
		// TODO: find out why this doesn't work: ss << bbox2D.Min() << " " << bbox2D.Max();
		ss << bbox2D.Min() << " ";
		ss << bbox2D.Max();
		return ss;
	}

	//! operator to build AABBox2D from a string stream
	std::stringstream& operator >> (std::stringstream& ss, AABBox2D& bbox2D)
	{
		Vector2 vMin, vMax;
		ss >> vMin >> vMax;
		bbox2D.Set(vMin, vMax);
		return ss;
	}

	//! operator to format AABBox3D into a string stream
	std::stringstream& operator << (std::stringstream& ss, const AABBox3D& bbox3D)
	{
		// TODO: find out why this doesn't work: ss << bbox3D.Min() << " " << bbox3D.Max();
		ss << bbox3D.Min() << " ";
		ss << bbox3D.Max();
		return ss;
	}

	//! operator to build AABBox3D from a string stream
	std::stringstream& operator >> (std::stringstream& ss, AABBox3D& bbox3D)
	{
		Vector3 vMin, vMax;
		ss >> vMin >> vMax;
		bbox3D.Set(vMin, vMax);
		return ss;
	}

	//! operator to format Size into a string stream
	std::stringstream& operator << (std::stringstream& ss, const Size& size)
	{
		ss << size.Width << " " << size.Height;
		return ss;
	}

	//! operator to build Size from a string stream
	std::stringstream& operator >> (std::stringstream& ss, Size& size)
	{
		ss >> size.Width >> size.Height;
		return ss;
	}	
	
	//! operator to format Color into a string stream
	std::stringstream& operator << (std::stringstream& ss, const Color& color)
	{
		// TODO: implement/get RGBA dialog editor in wxWidgets
		ss << color.R << " " << color.G << " " << color.B; // << " " << color.A; 
		return ss;
	}

	//! operator to build Color from a string stream
	std::stringstream& operator >> (std::stringstream& ss, Color& color)
	{
		// TODO: implement/get RGBA dialog editor in wxWidgets
		ss >> color.R >> color.G >> color.B; // >> color.A;
		return ss;
	}

	//! operator to format Size into a string stream
	std::stringstream& operator << (std::stringstream& ss, const Range& range)
	{
		ss << range.Min << " " << range.Max;
		return ss;
	}

	//! operator to build Size from a string stream
	std::stringstream& operator >> (std::stringstream& ss, Range& range)
	{
		ss >> range.Min >> range.Max;
		return ss;
	}

	//! operator to wstring into a string stream
	std::stringstream& operator << (std::stringstream& ss, const std::wstring& wstr)
	{
		std::string cstr(wstr.begin(), wstr.end());
		ss << cstr;
		return ss;
	}

	//! operator to wstring Size from a string stream
	std::stringstream& operator >> (std::stringstream& ss, std::wstring& wstr)
	{
		SHOOT_ASSERT(false, "Not implemented");
		return ss;
	}

	//! initializes the link
	void ILink::Init(Object* pParent)
	{
		if(GetObjectPtr() && GetObjectPtr()->GetID() == GetObjectID())
			return;

		if(!GetObjectID())
		{
			SetObject(NULL);
			return;
		}

		SHOOT_ASSERT(pParent->IsA(Entity::TypeID), "Link parent not of type Entity");
		Object* pObject = static_cast<Entity*>(pParent)->GetRoot()->GetChildByID(GetObjectID());
		SHOOT_WARNING(pObject, "Unresolved object Link for ID: 0x%x", GetObjectID());
		SetObject(pObject);
	}
}

