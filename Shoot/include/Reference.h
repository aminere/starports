/* 

Amine Rehioui
Created: April 16th 2011

*/

#pragma once


namespace shoot
{
	// forwards
	class Object;

	//! Reference interface
	class IReference
	{
	public:

		//! constructor
		IReference();

		//! destructor
		virtual ~IReference()
		{
			Clear();
		}		
		
		void SetObject(Object* pObject);
				
		inline Object* Get() const { return m_pObject; }		
		inline bool IsValid() const	{ return m_pObject != 0; }		
		inline void Clear() { SetObject(0);	}
		inline Object* GetOwner() const { return m_pOwner; }
		
		virtual IReference* Copy() = 0;		
		virtual const char* GetClassName() const = 0;	
		virtual int GetTypeID() const = 0;

		#ifdef SHOOT_EDITOR
		inline void SetOwner(Object* owner) { m_pOwner = owner; }
		#endif

	protected:

		Object* CopyObject() const;

		Object* m_pObject;
		Object* m_pOwner;
	};

	//! class to hold a reference to an object
	template <class T>
	class Reference : public IReference
	{
	public:
		
		Reference() {}

		Reference(T* pObject)
		{ 
			SetObject(pObject);	
		}

		Reference(T* pObject, Object* owner)
		{
			m_pOwner = owner;
			SetObject(pObject);
		}

		Reference(const Reference<T>& other)
		{
			m_pOwner = other.m_pOwner;
			SetObject(other.m_pObject);
		}

		Reference<T>& operator = (const Reference<T>& other)
		{
			SetObject(other.m_pObject);
			return *this;
		}

		Reference<T>& operator = (T* pObject) {	SetObject(pObject);	return *this; }
		bool operator == (const Reference<T>& other) const { return (m_pObject == other.m_pObject);	}
		bool operator == (const T* pObject) const {	return (m_pObject == pObject); }
		inline operator T*() { return static_cast<T*>(m_pObject); }
		inline operator T*() const { return static_cast<T*>(m_pObject); }
		const char* GetClassName() const { return T::GetStaticClassName(); }
		int GetTypeID() const { return T::TypeID; }
		inline T* operator ->() { return static_cast<T*>(m_pObject); }
		inline T* operator ->() const {	return static_cast<T*>(m_pObject); }
		inline T* Get()	{ return static_cast<T*>(m_pObject); }
		inline T* Get() const {	return static_cast<T*>(m_pObject); }
		IReference* Copy()
		{
			T* objectCopy = static_cast<T*>(CopyObject());
			auto referenceCopy = snew Reference<T>(objectCopy, m_pOwner);
			return referenceCopy;
		}
	};
}



