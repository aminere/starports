/* 

Amine Rehioui
Created: March 31st 2011

*/

#pragma once


#include <wx/listctrl.h>

namespace shoot
{
	//! Object Viewer
	class ObjectViewer : public wxPanel
	{
		typedef wxPanel super;

	public:
		
		//! ListView
		class ListView : public wxListView
		{
			typedef wxListView super;
		public:

			//! constructor
			ListView(wxWindow*);

			//! return the text for the given column of the given item
			virtual wxString OnGetItemText(long item, long column) const;

			//! OnGetItemImage
			virtual int OnGetItemImage(long item) const;

			//! SetObjects
			inline void SetObjects(std::vector<Object*>* objects) { m_Objects = objects; }

		private:

			std::vector<Object*>* m_Objects;
		};

		//! Constructor
		ObjectViewer(wxWindow* pParent);

		//! OnSelected
		void OnSelected();

		//! Select
		void SelectObject(Object* object);

	private:

		ListView* m_pObjectList;
		std::vector<Object*> m_Objects;

		// column sorting
		static const int NumColumns = 4;
		bool m_ColumSortForward[NumColumns];

		//! event handlers
		void OnColumnClick(wxListEvent& event);
		void OnItemSelected(wxListEvent& event);

		DECLARE_EVENT_TABLE();
	};
}




