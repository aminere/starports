/* 

Amine Rehioui
Created: September 9th 2012

*/

#pragma once


#include "wx/fswatcher.h"

namespace shoot
{
	//! ResourceWatcher class
	class ResourceWatcher : public Singleton<ResourceWatcher>, public wxEvtHandler
	{
		DECLARE_SINGLETON(ResourceWatcher);

	private:

	    void OnFileSystemEvent(wxFileSystemWatcherEvent& event);

		wxFileSystemWatcher* _fsWatcher;
	};
}




