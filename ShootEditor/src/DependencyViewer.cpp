/*

Amine Rehioui
Created: January 31st 2015

*/

#include "ShootEditorCommon.h"

#include "DependencyViewer.h"

#include "IconManager.h"

#include "ShootEditor.h"

#include "MeshEntity.h"
#include "Image.h"
#include "EditorUtils.h"

#include <wx/splitter.h>

namespace shoot
{	
	// Define event table
	BEGIN_EVENT_TABLE(DependencyViewer, wxPanel)
		EVT_LIST_COL_CLICK(wxID_ANY, OnColumnClick)
		EVT_LIST_ITEM_SELECTED(wxID_ANY, OnItemSelected)
		EVT_LIST_ITEM_RIGHT_CLICK(wxID_ANY, OnItemRightClick)
		EVT_TREE_SEL_CHANGED(ID_DependencyViewer_Tree, OnSelectTreeItem)
	END_EVENT_TABLE()

	//! Constructor
	DependencyViewer::DependencyViewer(wxWindow* pParent)
		: super(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
	{
		auto splitter = new wxSplitterWindow(this);
		m_pDependencyList = new wxListView(splitter, ID_DependencyViewer_List, wxDefaultPosition);
		m_pDependencyList->SetImageList(IconManager::Instance()->GetImageList(), wxIMAGE_LIST_SMALL);
		m_pDependencyTree = new wxTreeCtrl(splitter, ID_DependencyViewer_Tree, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE);
		m_pDependencyTree->SetImageList(IconManager::Instance()->GetImageList());
		splitter->SplitHorizontally(m_pDependencyList, m_pDependencyTree);
		splitter->SetSashGravity(0.60);

		// Fill columns
		wxListItem column;
		column.SetWidth(135);
		column.SetText(_T("Dependencies"));
		m_pDependencyList->InsertColumn(0, column);

		column.SetText(_T("Template"));
		column.SetWidth(118);
		m_pDependencyList->InsertColumn(1, column);

		wxBoxSizer *pSizer = new wxBoxSizer(wxVERTICAL);
		pSizer->Add(splitter, wxSizerFlags(1).Expand());
		SetSizer(pSizer); // use the sizer for layout
		pSizer->FitInside(this);
	}	

	//! Fill
	void DependencyViewer::Fill(Object* object)
	{
		// Fill items
		m_pDependencyList->DeleteAllItems();
		m_pDependencyTree->DeleteAllItems();

		wxListItem column;
		m_pDependencyList->GetColumn(0, column);
		column.SetText("Dependencies");
		m_pDependencyList->SetColumn(0, column);

		if (!object)
			return;

		auto container = ObjectManager::Instance()->GetContainer(object);
		for (auto reference : container->lReferences)
		{
			auto owner = reference->GetOwner();
			if (!owner)
				continue;

			std::string ownerName = owner->GetName().empty() ? owner->GetClassName() : owner->GetName();

			// Type
			long item = m_pDependencyList->InsertItem(m_pDependencyList->GetItemCount(), ownerName, IconManager::Instance()->GetIconIndex(owner));			
			m_pDependencyList->SetItemPtrData(item, (size_t)owner);

			// Template
			auto path = EditorUtils::GetTemplatePath(owner);
			m_pDependencyList->SetItem(item, 1, path);
		}

		column.SetText(std::string("Dependencies") + " (" + Utils::ToString(container->lReferences.size()) + ")");
		m_pDependencyList->SetColumn(0, column);
	}

	//! ObjectSort
	int wxCALLBACK DependencyViewer::ObjectSort(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)
	{
		Object* pObject1 = (Object*)item1;
		Object* pObject2 = (Object*)item2;
		DependencyViewer* viewer = (DependencyViewer*)sortData;
		int column = viewer->m_ColumnToSort;

		int result = 0;
		switch (column)
		{
		case 0:
		{
			auto name1 = pObject1->GetName().empty() ? pObject1->GetClassName() : pObject1->GetName();
			auto name2 = pObject2->GetName().empty() ? pObject2->GetClassName() : pObject2->GetName();
			result = name1.compare(name2);
		}
		break;

		case 1:
		{
			auto path1 = EditorUtils::GetTemplatePath(pObject1);
			auto path2 = EditorUtils::GetTemplatePath(pObject2);
			result = path1.compare(path2);
		}
		break;
		}

		if (!viewer->m_ColumSortForward[column])
			result = -result;

		return result;
	}

	//! FillDependencyTree
	void DependencyViewer::FillDependencyTree(wxTreeItemId item, Object* object)
	{
		auto container = ObjectManager::Instance()->GetContainer(object);
		for (auto reference : container->lReferences)
		{
			auto owner = reference->GetOwner();
			if (!owner)
				continue;

			auto objectName = EditorUtils::GetObjectName(owner);
			auto child = m_pDependencyTree->AppendItem(item, objectName, IconManager::Instance()->GetIconIndex(owner), IconManager::Instance()->GetIconIndex(owner), new EditorUtils::ItemData(owner));
			FillDependencyTree(child, owner);
			m_pDependencyTree->Expand(child);
		}
	}

	//! event handlers
	void DependencyViewer::OnColumnClick(wxListEvent& event)
	{
		m_ColumnToSort = event.GetColumn();

		m_pDependencyList->SortItems(DependencyViewer::ObjectSort, (long)this);

		SHOOT_ASSERT(m_ColumnToSort >= 0 && m_ColumnToSort < int(NumColumns), "Invalid Column Index");
		m_ColumSortForward[m_ColumnToSort] = !m_ColumSortForward[m_ColumnToSort];
	}

	void DependencyViewer::OnItemSelected(wxListEvent& event)
	{
		long item = event.GetItem();
		auto object = reinterpret_cast<Object*>(m_pDependencyList->GetItemData(item));
		ShootEditor::Instance()->GetObjectInspector()->Fill(object);
		
		// fill dependency tree
		m_pDependencyTree->DeleteAllItems();
		auto objectName = EditorUtils::GetObjectName(object);
		auto treeRoot = m_pDependencyTree->AddRoot(objectName, IconManager::Instance()->GetIconIndex(object), IconManager::Instance()->GetIconIndex(object), new EditorUtils::ItemData(object));
		FillDependencyTree(treeRoot, object);
		m_pDependencyTree->Expand(treeRoot);
	}

	void DependencyViewer::OnItemRightClick(wxListEvent& event)
	{
		long item = event.GetItem();
		auto object = reinterpret_cast<Object*>(m_pDependencyList->GetItemData(item));

		wxMenu subMenu;
		subMenu.Append(ID_DependencyViewer_ViewDependencies, "View Dependencies");
		subMenu.Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&DependencyViewer::OnItemMenuClicked, NULL, this);
		PopupMenu(&subMenu);
	}

	void DependencyViewer::OnItemMenuClicked(wxEvent& event)
	{
		switch (event.GetId())
		{
		case ID_DependencyViewer_ViewDependencies:
		{
			auto selection = m_pDependencyList->GetFocusedItem();
			auto object = reinterpret_cast<Object*>(m_pDependencyList->GetItemData(selection));
			auto tabControl = ShootEditor::Instance()->GetMainTabControl();
			tabControl->SelectObject(object);
		}
			break;
		}
	}

	void DependencyViewer::OnSelectTreeItem(wxTreeEvent& event)
	{
		wxTreeItemId itemId = event.GetItem();
		if (!itemId.IsOk())
			return;
		
		if (auto data = static_cast<EditorUtils::ItemData*>(m_pDependencyTree->GetItemData(itemId)))
		{
			auto object = data->m_pObject;
			ShootEditor::Instance()->GetObjectInspector()->Fill(object);
		}
	}
}

