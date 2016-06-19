/* 

Amine Rehioui
Created: March 31st 2011

*/

#include "ShootEditorCommon.h"

#include "ObjectViewer.h"

#include "IconManager.h"

#include "ShootEditor.h"
#include "DependencyViewer.h"
#include "EditorUtils.h"

#include <wx/sizer.h>

namespace shoot
{	
	// Define event table
	BEGIN_EVENT_TABLE(ObjectViewer, wxPanel)
		EVT_LIST_COL_CLICK(wxID_ANY, OnColumnClick)
		EVT_LIST_ITEM_SELECTED(wxID_ANY, OnItemSelected)
	END_EVENT_TABLE()

	//! Constructor
	ObjectViewer::ObjectViewer(wxWindow* pParent)
		: super(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
	{
		m_pObjectList = new ListView(this);
		m_pObjectList->SetImageList(IconManager::Instance()->GetImageList(), wxIMAGE_LIST_SMALL);

		// Fill columns
		wxListItem column;
		column.SetWidth(150);
		column.SetText(_T("Type"));
		m_pObjectList->InsertColumn(0, column);		

		column.SetText(_T("ID"));
		column.SetWidth(74);
		m_pObjectList->InsertColumn(1, column);

		column.SetText(_T("Template"));
		column.SetWidth(120);
		m_pObjectList->InsertColumn(2, column);

		column.SetText(_T("Ref"));
		column.SetWidth(40);
		m_pObjectList->InsertColumn(3, column);		

		wxBoxSizer *pSizer = new wxBoxSizer(wxVERTICAL);
		pSizer->Add(m_pObjectList, wxSizerFlags(1).Expand());

		SetSizer(pSizer); // use the sizer for layout

		pSizer->FitInside(this);

		for (int i = 0; i < NumColumns; ++i)
			m_ColumSortForward[i] = true;
	}	

	//! Select
	void ObjectViewer::SelectObject(Object* object)
	{
		auto it = std::find(m_Objects.begin(), m_Objects.end(), object);
		auto item = std::distance(m_Objects.begin(), it);
		// m_pObjectList->Select(item);
		// TODO doesn't actually work
	}

	//! event handlers
	void ObjectViewer::OnColumnClick(wxListEvent& event)
	{
		auto column = event.GetColumn();
		SHOOT_ASSERT(column >= 0 && column < int(NumColumns), "Invalid Column Index");
		
		auto forward = m_ColumSortForward[column];
		switch (column)
		{
		case 0: // Type
			std::sort(m_Objects.begin(), m_Objects.end(), [&](Object* a, Object* b) 
				{					
					auto result = std::string(a->GetClassName()).compare(b->GetClassName()) < 0;
					return forward ? result : !result;
				});
			break;

		case 1: // ID
			std::sort(m_Objects.begin(), m_Objects.end(), [&](Object* a, Object* b)
			{
				auto result = a->GetID() < b->GetID();
				return forward ? result : !result;
			});
			break;		

		case 2: // Template
			std::sort(m_Objects.begin(), m_Objects.end(), [&](Object* a, Object* b)
			{
				auto result = std::string(a->GetTemplatePath()).compare(b->GetTemplatePath()) < 0;
				return forward ? result : !result;
			});
			break;

		case 3: // RefCount
			std::sort(m_Objects.begin(), m_Objects.end(),
				[&](Object* a, Object* b)
			{
				auto container1 = ObjectManager::Instance()->GetContainer(a);
				auto container2 = ObjectManager::Instance()->GetContainer(b);				
				auto result = container1->lReferences.size() >= container2->lReferences.size();
				return forward ? result : !result;
			});
			break;
		}

		m_ColumSortForward[column] = !m_ColumSortForward[column];
		m_pObjectList->RefreshItems(0, m_pObjectList->GetItemCount());
	}

	void ObjectViewer::OnItemSelected(wxListEvent& event)
	{
		long item = event.GetItem();
		auto object = m_Objects[item];
		ShootEditor::Instance()->GetObjectInspector()->Fill(object);
		ShootEditor::Instance()->GetMainTabControl()->GetDependencyViewer()->Fill(object);
		SetFocus();
	}

	//! OnSelected
	void ObjectViewer::OnSelected()
	{
		m_Objects.clear();
		const auto& objectMap = ObjectManager::Instance()->GetObjectMap();
		for (auto it = objectMap.begin(); it != objectMap.end(); ++it)		
			m_Objects.push_back((*it).first);		
		
		m_pObjectList->SetObjects(&m_Objects);
		m_pObjectList->SetItemCount(m_Objects.size());

		// update tab title
		wxAuiNotebook* notebook = ShootEditor::Instance()->GetMainTabControl();
		auto title = std::string("Objects (") + Utils::ToString(m_Objects.size()) + ")";
		notebook->SetPageText(notebook->GetPageCount() - 1, title);
	}

	//! constructor
	ObjectViewer::ListView::ListView(wxWindow* parent)
		: super(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_VIRTUAL)
		, m_Objects(NULL)
	{
	}

	// return the text for the given column of the given item
	wxString ObjectViewer::ListView::OnGetItemText(long item, long column) const
	{		
		auto pObject = (*m_Objects)[item];

		wxString text;
		switch (column)
		{
			case 0: // Type
			text = pObject->GetClassName();
			break;

			case 1: // ID
			{
				std::stringstream ss;
				ss << std::hex << pObject->GetID();
				text = ss.str();
			}
			break;			

			case 2: // Template
			text = EditorUtils::GetTemplatePath(pObject);
			break;

			case 3: // RefCount
			{
				auto container = ObjectManager::Instance()->GetContainer(pObject);
				text = Utils::ToString(container->lReferences.size());				
			}
			break;
		}
		return text;
	}

	//! OnGetItemImage
	int ObjectViewer::ListView::OnGetItemImage(long item) const
	{
		auto pObject = (*m_Objects)[item];
		return (int)IconManager::Instance()->GetIconIndex(pObject);
	}
}

