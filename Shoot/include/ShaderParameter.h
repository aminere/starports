/* 

Amine Rehioui
Created: September 4th 2012

*/

#pragma once


#include "Color.h"
#include "Vector4.h"

namespace shoot
{
	//! IShaderData
	class IShaderData
	{
	public:

		//! sets the data
		virtual void Set(const void* pData, size_t count) = 0;

		//! returns the data
		virtual const void* Get(int& count) = 0;
	};

	//! ShaderData
	template<class T>
	class ShaderData : public IShaderData
	{
	public:

		//! sets the data
		void Set(const void* pData, size_t count)
		{
			m_Data.clear();
			for (size_t i = 0; i<count; ++i)
				m_Data.push_back(*(static_cast<const T*>(pData)+i));			
		}

		//! returns the data
		const void* Get(int& count)
		{
			if(m_Data.size())
			{
				count = (int)m_Data.size();
				return &m_Data[0];
			}
			count = 0;
			return NULL;
		}

	private:

		std::vector<T> m_Data;
	};

	//! base ShaderParameter class
	class ShaderParameter : public Object
	{
		DECLARE_OBJECT(ShaderParameter, Object);

	public:

		//! parameter type
		enum E_Type
		{
			T_Float,
			T_Vec4,
			T_Color,
			T_Mat44,
			T_Int,
			T_Vec2,
			T_Vec3,
			T_None
		};

		//! constructor
		ShaderParameter();

		//! destructor
		virtual ~ShaderParameter();

		//! serializes the object to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! returns the type
		inline E_Type GetType() const { return m_eType; }

		//! data setters
		inline void Set(const float* pData, size_t count) { InternalSet<T_Float>(pData, count); }
		inline void Set(const Vector4* pData, size_t count) { InternalSet<T_Vec4>(pData, count); }
		inline void Set(const Color* pData, size_t count) { InternalSet<T_Color>(pData, count); }
		inline void Set(const Matrix44* pData, size_t count) { InternalSet<T_Mat44>(pData, count); }
		inline void Set(const int* pData, size_t count) { InternalSet<T_Int>(pData, count); }
		inline void Set(const Vector2* pData, size_t count) { InternalSet<T_Vec2>(pData, count); }
		inline void Set(const Vector3* pData, size_t count) { InternalSet<T_Vec3>(pData, count); }

		//! get the data pointer
		inline const void* Get(int& count) { return m_pData->Get(count); }

	private:

		IShaderData* m_pData;

		//! sets the data
		template <E_Type type>
		void InternalSet(const void* pData, size_t count)
		{
			SHOOT_ASSERT(m_eType == type, "ShaderParameter type expected '%d', found '%d'", type, m_eType);
			m_pData->Set(pData, count);
		}

		// propeties
		E_Type m_eType;
	};
}



