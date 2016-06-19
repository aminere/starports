/* 

Amine Rehioui
Created: October 17th 2011

*/

#pragma once


#include "Text.h"

namespace shoot
{
	//! memory stats manager
	class StatsManager : public Singleton<StatsManager>
	{
		DECLARE_SINGLETON(StatsManager);

	public:

		//! set status
		void SetEnabled(bool bEnabled) { m_bEnabled = bEnabled; }

		//! get status
		bool IsEnabled() const { return m_bEnabled; }

		//! update
		void Update();

		//! registers for rendering
		void RegisterForRendering();

	private:

		bool m_bEnabled;

		Reference<Text> m_FPS;
		Reference<Text> m_MemoryUsed;
		Reference<Text> m_NumDrawCalls;
		Reference<Text> m_QuadTreeInfo;
		Reference<Text> m_FileSystemInfo;
	};
}



