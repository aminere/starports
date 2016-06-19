/* 

Amine Rehioui
Created: January 31st 2015

*/

#pragma once


#include <wx/listctrl.h>

namespace shoot
{
	//! DependencyViewer
	class DependencyViewer : public wxPanel
	{
		typedef wxPanel super;

	public:		

		//! Constructor
		DependencyViewer(wxWindow* pParent);

		//! Fill
		void Fill(Object* object);

	private:

		wxTreeCtrl* m_pDependencyTree;
		wxListView* m_pDependencyList;
		
		//! ObjectSort
		static int wxCALLBACK ObjectSort(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData);

		//! FillDependencyTree
		void FillDependencyTree(wxTreeItemId item, Object* object);

		// column sorting
		static const int NumColumns = 2;
		bool m_ColumSortForward[NumColumns];
		int m_ColumnToSort;

		//! event handlers
		void OnColumnClick(wxListEvent& event);
		void OnItemSelected(wxListEvent& event);
		void OnItemRightClick(wxListEvent& event);
		void OnItemMenuClicked(wxEvent& event);
		void OnSelectTreeItem(wxTreeEvent& event);

		DECLARE_EVENT_TABLE();
	};
}




