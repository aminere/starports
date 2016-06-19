/* 

Amine Rehioui
Created: October 18th 2011

*/

#include "Shoot.h"

#include "MemoryManager.h"

#include <sstream>

namespace shoot
{
	// static variables initialization
	MemoryManager::AllocMap* MemoryManager::m_pAllocMap = new MemoryManager::AllocMap();
	MemoryManager::StatMap* MemoryManager::m_pStatMap = new MemoryManager::StatMap();	
	size_t MemoryManager::m_UsedMemory = 0;
	size_t MemoryManager::m_PeakMemory = 0;
	bool MemoryManager::m_bInitialized = false;

	//! allocates memory
	void* MemoryManager::Alloc(size_t size, const char* strFileName, int line)
	{
		void* pMemory = malloc(size);
		SHOOT_ASSERT(pMemory, "Could not allocate memory");	

#if !defined(_RETAIL_) && SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32
		AllocInfo info = { size, strFileName, static_cast<size_t>(line) };

		(*m_pAllocMap)[pMemory] = info;
		m_UsedMemory += size;

		if (m_UsedMemory > m_PeakMemory)
			m_PeakMemory = m_UsedMemory;

		{
			// Stats
			size_t id = (size_t)strFileName;
			id |= (size_t)line;
			auto it = m_pStatMap->find(id);
			if (it == m_pStatMap->end())
			{
				StatInfoValue& value = (*m_pStatMap)[id];
				value.Used = value.Peak = size;
				value.strFileName = strFileName;
				value.Line = line;
			}
			else
			{
				StatInfoValue& value = (*m_pStatMap)[id];
				value.Used += size;
				if (value.Used > value.Peak)
					value.Peak = value.Used;
			}
		}		
#endif

		return pMemory;
	}
		
	//! frees memory
	void MemoryManager::Free(void* pMemory)
	{
#if !defined(_RETAIL_) && SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32
		if(m_bInitialized)
		{
			if(!(*m_pAllocMap).empty())
			{
				std::map<void*, AllocInfo>::iterator it = (*m_pAllocMap).find(pMemory);
				if(it != (*m_pAllocMap).end())
				{
					m_UsedMemory -= it->second.Size;					

					{
						// Stats
						size_t id = (size_t)it->second.strFileName;
						id |= (size_t)it->second.Line;
						(*m_pStatMap)[id].Used -= it->second.Size;
					}

					(*m_pAllocMap).erase(it);
				}
			}
		}
#endif

		free(pMemory);
	}

	//! checks for leaks
	void MemoryManager::LeakCheck()
	{
		if(m_pAllocMap && !(*m_pAllocMap).empty())
		{
			SHOOT_ASSERT(false, "memory leak detected: %d bytes", m_UsedMemory);
			for(std::map<void*, AllocInfo>::iterator it = (*m_pAllocMap).begin(); it != (*m_pAllocMap).end(); it++)
			{
				AllocInfo info = it->second;
				std::stringstream addr;
				addr << "0x" << std::hex << (size_t)it->first;
				Log << info.Size << " bytes @ " << info.strFileName << " line " << info.Line << " addr " << addr.str().c_str() << "\n";				
			}
		}
	}

	//! OutputStats
	void MemoryManager::OutputStats()
	{
		#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32
			FILE* f = fopen("MemoryStats.txt", "wt");
			std::vector<StatInfoValue> stats;
			stats.reserve(m_pStatMap->size());
			for (auto it = m_pStatMap->begin(); it != m_pStatMap->end(); ++it)
			{
				stats.push_back((*it).second);
			}

			std::sort(stats.begin(), stats.end(), [](const StatInfoValue& v1, const StatInfoValue& v2) { return v1.Peak > v2.Peak; });
			float totalPeak = 0.0f;
			for (auto stat : stats)
			{
				totalPeak += (float)stat.Peak;
			}

			// Not sure which peak is more accurate, so picking the highest one.
			auto peak = Math::Max<float>((float)m_PeakMemory, totalPeak);
			fprintf(f, "** PeakMemory: %.2f KB ** \n", (float)peak / 1024.0f);
			for (auto stat : stats)
			{
				auto percentPeak = ((float)stat.Peak / totalPeak) * 100.0f;
				fprintf(f, "[%s][%d] => %.2f KB (%f%%)\n", stat.strFileName, stat.Line, (float)stat.Peak / 1024.0f, percentPeak);
			}
			fclose(f);
		#endif
	}
}

