/* 

Amine Rehioui
Created: August 22nd 2011

*/

#pragma once


#include "wx/aui/auibar.h"

namespace shoot
{
	//! ToolbarManager
	class ToolbarManager : public Singleton<ToolbarManager>
	{
		DECLARE_SINGLETON(ToolbarManager);

	public:

		//! Initializes the tool bar manager
		virtual void Init();

		//! returns the main tool bar
		wxAuiToolBar* GetMainToolbar() { return m_pMainToolbar; }

		void SetUndoEnabled(bool enable);
		void SetRedoEnabled(bool enable);

	protected:

		wxAuiToolBar* m_pMainToolbar;
	};
}



