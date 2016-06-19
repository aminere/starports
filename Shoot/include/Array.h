/* 

Amine Rehioui
Created: July 7th 2012

*/

#pragma once


namespace shoot
{
	//! Serializable Array interface
	class IArray
	{
	public:	
		//! Allocates one element, calls the approriate constructor
		virtual void* Alloc() = 0;
		
		//! Adds one element, the pointer is cast to the appropriate type and the result is copied into the array
		virtual void Add(void* pElement) = 0;

		//! Returns a pointer to an element, the properties are responsible for casting to the appropriate type
		virtual void* GetPtr(size_t index) = 0;

		//! Returns the number of elements
		virtual size_t GetSize() const = 0;

		//! Explicitly clear the array
		virtual void Clear() = 0;

		//! removes one element from the array
		virtual void* Remove(size_t index) = 0;

		//! deletes one element from the array
		virtual void Delete(size_t index) = 0;

		//! inserts one element in the array
		virtual void Insert(size_t index, void* pElement) = 0;

		//! copies an element
		virtual void* CopyElement(size_t index) = 0;
	};

	//! Concrete array class to be used to expose arrays of user data
	template <class T>
	class Array : public IArray
	{
	public:

		//! constructor
		Array()
		{
		}

		//! copy constructor
		Array(const Array<T>& other)
		{
			*this = other;
		}

		//! destructor
		~Array()
		{
			Clear();
		}

		//! returns a specific element
		inline T& operator[](size_t index) { return *aElements[index]; }

		//! returns a specific element
		inline const T& operator[](size_t index) const { return *aElements[index]; }

		//! Allocates one element, calls the approriate constructor
		void* Alloc() {	return snew T(); }		

		//! Adds one element
		void Add(void* pElement)
		{
			aElements.push_back(static_cast<T*>(pElement));
		}

		//! Returns a pointer to an element
		void* GetPtr(size_t index)	{ return aElements[index]; }		

		//! Returns the number of elements
		size_t GetSize() const { return aElements.size(); }

		//! Clear the array
		void Clear() 
		{ 
			for (size_t i=0; i<aElements.size(); ++i)
				delete aElements[i];
			
			aElements.clear();
		}

		//! removes one element from the array
		void* Remove(size_t index)
		{
			void* pElement = aElements[index];
			aElements.erase(aElements.begin()+index);
			return pElement;
		}

		//! deletes one element from the array
		void Delete(size_t index)
		{
			delete aElements[index];
			aElements.erase(aElements.begin()+index);
		}

		//! inserts one element in the array
		void Insert(size_t index, void* pElement)
		{
			aElements.insert(aElements.begin()+index, static_cast<T*>(pElement));
		}

		//! copies an element
		void* CopyElement(size_t index)
		{
			auto elemCopy = static_cast<T*>(Alloc());
			auto elem = static_cast<T*>(GetPtr(index));
			*elemCopy = *elem;
			return elemCopy;
		}

		//! returns the elements
		std::vector<T*>& Elements() { return aElements; }
		const std::vector<T*>& Elements() const { return aElements; }
			
		//! comparison operator
		bool operator == (const Array<T>& other) const
		{
			if(aElements.size() != other.aElements.size())
				return false;
			
			for (size_t i=0; i<aElements.size(); ++i)
			{
				if(!(*(aElements[i]) == *(other.aElements[i])))
					return false;
			}
			return true;
		}

		//! assignment operator
		Array<T>& operator = (const Array<T>& other)
		{
			Clear();
			for (size_t i = 0; i<other.GetSize(); ++i)
			{
				T* pElem = static_cast<T*>(Alloc());
				*pElem = other[i];
				Add(pElem);
			}
			return *this;
		}

		// iterator
		class iterator : public std::iterator<std::input_iterator_tag, T*>
		{
		public:

			iterator(T** pData) : m_pData(pData) { }
			iterator(const iterator& other) : m_pData(other.m_pData) { }
			iterator& operator++() { ++m_pData; return *this; }
			iterator operator++(int) { iterator tmp(*this); operator++(); return tmp; }			
			iterator operator+ (const std::size_t& n)  { iterator tmp(*this); tmp += n; return tmp; }
			iterator operator+ (const iterator& other) { iterator tmp(*this); tmp += other; return tmp; }
			void     operator+=(const std::size_t& n)  { m_pData += n; }
			void     operator+=(const iterator& other) { m_pData += other.m_pData; }		
			iterator& operator--() { m_pData--; return *this; }
			iterator operator--(int) { iterator tmp(*this); operator--(); return tmp;}
			iterator operator- (const std::size_t& n)  {iterator tmp(*this); tmp -= n; return tmp;}
			std::size_t operator- (const iterator& other) { return m_pData - other.m_pData; }
			void        operator-=(const std::size_t& n)  { m_pData -= n; }
			void        operator-=(const iterator& other) { m_pData -= other.m_pData; }
			bool operator < (const iterator& other) { return (m_pData - other.m_pData)< 0; }
			bool operator <=(const iterator& other) { return (m_pData - other.m_pData)<=0; }
			bool operator > (const iterator& other) { return (m_pData - other.m_pData)> 0; }
			bool operator >=(const iterator& other) { return (m_pData - other.m_pData)>=0; }
			bool operator ==(const iterator& other) { return  m_pData == other.m_pData; }
			bool operator !=(const iterator& other) { return  m_pData != other.m_pData; }
			T*& operator*() { return *m_pData; }

		private:

			T** m_pData;
		};

		// const_iterator
		class const_iterator : public std::iterator<std::random_access_iterator_tag, const T*>
		{
		public:

			const_iterator(const T* const* pData) : m_pData(pData) { }
			const_iterator(const const_iterator& other) : m_pData(other.m_pData) { }
			const_iterator& operator++() { ++m_pData; return *this; }
			const_iterator operator++(int) { const_iterator tmp(*this); operator++(); return tmp; }			
			const_iterator operator+ (const std::size_t& n) { const_iterator tmp(*this); tmp += n; return tmp; }
			const_iterator operator+ (const const_iterator& other) { const_iterator tmp(*this); tmp += other; return tmp; }
			void     operator+=(const std::size_t& n)  { m_pData += n; }
			void     operator+=(const const_iterator& other) { m_pData += other.m_pData; }		
			const_iterator& operator--() { m_pData--; return *this; }
			const_iterator operator--(int) { const_iterator tmp(*this); operator--(); return tmp; }
			const_iterator operator- (const std::size_t& n)  { const_iterator tmp(*this); tmp -= n; return tmp; }
			std::size_t operator- (const const_iterator& other) { return m_pData - other.m_pData; }
			void        operator-=(const std::size_t& n)  { m_pData -= n; }
			void        operator-=(const const_iterator& other) { m_pData -= other.m_pData; }
			bool operator < (const const_iterator& other) { return (m_pData - other.m_pData)< 0; }
			bool operator <=(const const_iterator& other) { return (m_pData - other.m_pData)<=0; }
			bool operator > (const const_iterator& other) { return (m_pData - other.m_pData)> 0; }
			bool operator >=(const const_iterator& other) { return (m_pData - other.m_pData)>=0; }
			bool operator ==(const const_iterator& other) { return  m_pData == other.m_pData; }
			bool operator !=(const const_iterator& other) { return  m_pData != other.m_pData; }
			const T* const& operator*() { return *m_pData; }

		private:

			const T* const* m_pData;
		};

		iterator begin() { return iterator(aElements.data()); }
		iterator end() { return iterator(aElements.data()+aElements.size()); }
		const_iterator begin() const { return const_iterator(aElements.data()); }
		const_iterator end() const { return const_iterator(aElements.data()+aElements.size()); }
		T* last() { return aElements.back(); }
		const T* last() const { return aElements.back(); }
		T* first() { return aElements.front(); }
		const T* first() const { return aElements.front(); }

	private:

		std::vector<T*> aElements;
	};
}



