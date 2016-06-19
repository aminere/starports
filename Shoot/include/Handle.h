/* 

Amine Rehioui
Created: April 1st 2013

*/

#pragma once


namespace shoot
{
	// forwards
	class Object;

	//! Handle interface
	class IHandle
	{
	public:

		//! constructor
		IHandle();

		//! destructor
		virtual ~IHandle()
		{
			Clear();
		}
		
		//! Sets the referenced object
		void SetObject(Object* pObject);

		//! Returns a pointer to the contained element
		inline Object* Get()
		{
			return m_pObject;
		}

		//! Returns a pointer to the contained element
		inline Object* Get() const
		{ 
			return m_pObject;
		}

		//! returns true if the reference is valid
		inline bool IsValid() const
		{
			return m_pObject != 0;
		}

		//! clears the handle
		inline void Clear()
		{
			SetObject(0);
		}

		//! clears the handle - for use by the ObjectManager
		inline void ClearByObjectManager()
		{
			m_pObject = 0;
		}

	protected:

		Object* m_pObject;
	};

	//! class to hold a handle to an object
	template <class T>
	class Handle : public IHandle
	{
	public:

		//! constructor
		Handle()
		{
		}

		//! constructor
		Handle(T* pObject)
		{
			SetObject(pObject);
		}

		//! copy constructor
		Handle(const Handle<T>& other)
		{
			*this = other;
		}

		//! assignment operator
		Handle<T>& operator = (const Handle<T>& other)
		{
			SetObject(other.m_pObject);
			return *this;
		}

		//! assignment operator
		Handle<T>& operator = (T* pObject)
		{
			SetObject(pObject);
			return *this;
		}

		//! comparison operator
		bool operator == (const Handle<T>& other) const
		{
			return (m_pObject == other.m_pObject);
		}

		//! comparison operator
		bool operator == (const T* pObject) const
		{
			return (m_pObject == pObject);
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



