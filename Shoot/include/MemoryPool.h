/* 

Amine Rehioui
Created: February 29th 2012

*/

#pragma once


namespace shoot
{
	//! memory pool
	template <class T>
	class MemoryPool
	{
	public:

		//! constructor
		MemoryPool()
			: m_CurrentElem(0)
			, m_ElementCount(0)
		{
		}

		//! destructor
		~MemoryPool()
		{
		}

		//! Creates and object in the pre-allocated buffer
		T* Alloc()
		{
			if (m_CurrentElem == m_Elements.size())
				m_CurrentElem = 0;

			if (GetElement(m_CurrentElem))
			{
				SHOOT_WARNING(false, "MemoryPool overflow");
				return NULL;
			}

			T* addr = &m_Elements[m_CurrentElem];
			++m_CurrentElem;
			++m_ElementCount;
			return addr;
		}

		//! Frees an object from the pre-allocated buffer		
		void Free(void *pAddr)
		{
			memset(pAddr, 0, sizeof(T));
			--m_ElementCount;
			SHOOT_ASSERT(m_ElementCount >= 0, "Negative m_ElementCount detected");
		}

		//! resizes the pool
		void SetCapacity(int capacity)
		{
			m_Elements.resize(capacity);
			memset(m_Elements.data(), 0, sizeof(T)*capacity);
			m_CurrentElem = 0;
		}

		//! GetCapacity
		inline size_t GetCapacity() const { return m_Elements.size(); }

		//! GetElementCount
		inline int GetElementCount() const { return m_ElementCount; }

		//! GetElement
		T* GetElement(size_t index)
		{
			T* elem = &m_Elements[index];
			auto data = *((size_t*)elem);
			if (data != 0)
				return elem;
			else
				return NULL;
		}

		//! Clear
		void Clear()
		{
			memset(m_Elements.data(), 0, sizeof(T)*m_Elements.size());
		}

	private:
		
		std::vector<T> m_Elements;
		int m_CurrentElem;
		int m_ElementCount;
	};
}



