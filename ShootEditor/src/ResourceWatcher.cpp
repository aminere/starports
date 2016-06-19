/* 

Amine Rehioui
Created: September 9th 2012

*/

#include "ShootEditorCommon.h"

#include "EditorUtils.h"
#include "ResourceWatcher.h"

namespace shoot
{
	//! constructor
	ResourceWatcher::ResourceWatcher()
	{
		_fsWatcher = new wxFileSystemWatcher();
		_fsWatcher->SetOwner(this);
		_fsWatcher->AddTree(wxFileName("./data/"));

	    Connect(wxEVT_FSWATCHER, wxFileSystemWatcherEventHandler(ResourceWatcher::OnFileSystemEvent));
	}

	//! destructor
	ResourceWatcher::~ResourceWatcher()
	{
		delete _fsWatcher;
	}

	void ResourceWatcher::OnFileSystemEvent(wxFileSystemWatcherEvent& event)
	{
		if(event.GetChangeType() != wxFSW_EVENT_MODIFY)
			return;

		std::string path(event.GetPath().GetFullPath());
		auto isDirectoy = (path.back() == '\\' || path.back() == '/');
		if(isDirectoy)
			return;

		std::string relative;
		if(shoot::EditorUtils::GetRelativePath(path, relative))
			shoot::ResourceManager::Instance()->RequestReloadResource(relative);
	}
}

