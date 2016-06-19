/* 

Amine Rehioui
Created: February 5th 2010

*/

#pragma once


namespace shoot
{
	//! Template Singleton Class
	template <class T> 
	class Singleton
	{
	public:

		//! Return the unique instance of class T
		inline static T* Instance() 
		{
			SHOOT_ASSERT(m_spInstance, "%s singleton instance not created yet", T::GetSingletonClassName());
			return m_spInstance;
		}

		//! Creates the instance of class T
		static void CreateInstance() 
		{
			SHOOT_ASSERT(!m_spInstance, "%s singleton instance already created", T::GetSingletonClassName());
			m_spInstance = snew T();
		}

		//! Destroys the instance of class T
		static void DestroyInstance() 
		{
			SHOOT_ASSERT(m_spInstance, "%s Singleton instance already destroyed", T::GetSingletonClassName());
			sdelete(m_spInstance);
		}

		//! Overrides an instance
		static void OverrideInstance(T* pOverride)
		{
			if(m_spInstance)
				DestroyInstance();

			m_spInstance = pOverride; 
		}

		//! returns true if the instance was created
		inline static bool IsCreated() { return m_spInstance != 0; }

	protected:

		static T* m_spInstance;

#define DECLARE_SINGLETON(ClassName) \
	public: \
		ClassName(); \
		virtual ~ClassName(); \
		ClassName(const ClassName&); \
		ClassName& operator = (const ClassName&); \
		static const char* GetSingletonClassName() { return #ClassName; }
	};

	template <class T> T* Singleton<T>::m_spInstance = 0;
}



