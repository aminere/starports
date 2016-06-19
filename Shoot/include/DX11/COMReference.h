/* 

Amine Rehioui
Created: July 28th 2013

*/

#pragma once

#include "UnknwnBase.h"

namespace shoot
{
	//! ICOMReference
	class ICOMReference
	{
	public:

		//! constrcutor
		ICOMReference()
			: m_pObject(NULL)
		{
		}

		//! destructor
		virtual ~ICOMReference()
		{
			SetObject(NULL);
		}

		//! Sets the referenced object
		void SetObject(IUnknown* pObject);

	protected:

		IUnknown* m_pObject;
	};

	//! COMReference
	template <class T>
	class COMReference : public ICOMReference
	{
	public:

		//! constructor
		COMReference()
		{
		}

		//! constructor
		COMReference(T* pObject)
		{
			SetObject(pObject);
		}

		//! copy constructor
		COMReference(const COMReference<T>& other)
		{
			*this = other;
		}

		//! assignment operator
		COMReference<T>& operator = (const COMReference<T>& other)
		{
			SetObject(other.m_pObject);
			return *this;
		}

		//! assignment operator
		COMReference<T>& operator = (T* pObject)
		{
			SetObject(pObject);
			return *this;
		}

		//! T* cast operator
		inline operator T*()
		{
			return static_cast<T*>(m_pObject);
		}

		//! const T* cast operator
		inline operator T*() const
		{
			return static_cast<T*>(m_pObject);
		}

		//! -> operator
		inline T* operator ->()
		{ 
			return static_cast<T*>(m_pObject);
		}

		//! const -> operator
		inline T* operator ->() const
		{
			return static_cast<T*>(m_pObject);
		}

		//! returns the object pointed at
		inline T* Get() 
		{ 
			return static_cast<T*>(m_pObject);
		}

		//! returns the object pointed at
		inline T* Get() const 
		{
			return static_cast<T*>(m_pObject);
		}
	};
}

