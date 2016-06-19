/* 

Amine Rehioui
Created: October 18th 2011

*/

#pragma once


namespace shoot
{
	//! memory manager
	class MemoryManager
	{
	public:

		//! initializes the memory manager
		static void Init() { m_bInitialized = true; }
		
		//! uninitialize the memory manager
		static void Uninit() 
		{
			LeakCheck();
			OutputStats();
			m_bInitialized = false;
			sdelete(m_pAllocMap);
			sdelete(m_pStatMap);
		}
		
		//! allocates memory
		static void* Alloc(size_t size, const char* strFileName, int line);
		
		//! frees memory
		static void Free(void* pMemory);

		//! checks for leaks
		static void LeakCheck();

		//! OutputStats
		static void OutputStats();

		//! Get used memory in bytes
		inline static size_t GetUsedMemory() { return m_UsedMemory; }

		//! GetPeakMemory
		inline static size_t GetPeakMemory() { return m_PeakMemory; }

	private:

		//! Allocation info
		struct AllocInfo
		{
			size_t Size;
			const char* strFileName;
			size_t Line;
		};		

		//! StatInfoValue
		struct StatInfoValue
		{
			const char* strFileName;
			size_t Line;
			size_t Used;
			size_t Peak;
		};
		
		typedef std::map<void*, AllocInfo> AllocMap;
		static AllocMap* m_pAllocMap;
		static size_t m_UsedMemory;
		static size_t m_PeakMemory;
		static bool m_bInitialized;

		typedef std::map<size_t, StatInfoValue> StatMap;
		static StatMap* m_pStatMap;
	};
}



