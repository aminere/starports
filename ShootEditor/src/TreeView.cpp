/* 

Amine Rehioui
Created: April 10th 2010

*/		

#include "ShootEditorCommon.h"

#include "TreeView.h"

#include "IconManager.h"
#include "LayoutComponent.h"
#include "EditorUtils.h"

namespace shoot
{
	// Define event table
	BEGIN_EVENT_TABLE(TreeView, wxTreeCtrl)
		EVT_TREE_SEL_CHANGED(ID_TreeView, TreeView::OnSelChanged)
		EVT_TREE_BEGIN_DRAG(wxID_ANY, TreeView::OnBeginDrag)
		EVT_TREE_END_DRAG(wxID_ANY, TreeView::OnEndDrag)
		EVT_KEY_DOWN(TreeView::OnKeyDown)
		EVT_KEY_UP(TreeView::OnKeyUp)
		EVT_TREE_ITEM_MENU(wxID_ANY, TreeView::OnItemMenu)
	END_EVENT_TABLE()

	//! Constructor
	TreeView::TreeView(wxWindow *parent, 
					   const wxWindowID id /*= wxID_ANY*/,
					   const wxPoint& pos /*= wxDefaultPosition*/, 
					   const wxSize& size /*= wxSize(200, 300)*/,
					   long style /*= wxTR_DEFAULT_STYLE*/)
	: super(parent, ID_TreeView, pos, size, style)
	, m_bControlDown(false)
	, m_pItemMenuEntity(NULL)
	{
		SetImageList(IconManager::Instance()->GetImageList());
	}

	//! Inserts an entity into the tree
	void TreeView::InsertEntity(Entity* pEntity)
	{
		if(!GetRootItem().IsOk())
		{
			// if no root item, then recursively insert from the root
			Entity* pRoot = Engine::Instance()->GetContextStack();
			AddRoot("ContextStack", IconManager::Instance()->GetIconIndex(pRoot), IconManager::Instance()->GetIconIndex(pRoot), new ItemData(pRoot));
			for (size_t i = 0; i<pRoot->GetChildCount(); ++i)
			{
				InsertEntity(pRoot->GetChild(i));
			}
		}
		else
		{
			Entity* pParent = pEntity->GetParent();
			wxTreeItemId parentItemId = GetItemIDFromEntity(GetRootItem(), pParent);
			SHOOT_ASSERT(parentItemId.IsOk(), "Trying to insert entity into an unexisting tree item");

			// insert the entity
			std::string name = pEntity->GetName().empty() ? pEntity->GetClassName() : pEntity->GetName();
			AppendItem(parentItemId, name, IconManager::Instance()->GetIconIndex(pEntity), IconManager::Instance()->GetIconIndex(pEntity), new ItemData(pEntity));

			// recursively insert children, if any
			int numChildren = pEntity->GetChildCount();
			for(int i=0; i<numChildren; ++i)
			{
				InsertEntity(pEntity->GetChild(i));
			}

			if(pParent->IsA(ContextStack::TypeID) || pParent->IsA(Context::TypeID))
			{
				Expand(parentItemId);
			}
		}		
	}

	//! Inserts an entity into an existing parent entity at a specific index
	void TreeView::InsertEntity(Entity* pEntity, size_t index)
	{
		Entity* pParent = pEntity->GetParent();
		wxTreeItemId parentItemId = GetItemIDFromEntity(GetRootItem(), pParent);
		SHOOT_ASSERT(parentItemId.IsOk(), "Trying to insert entity into an unexisting tree item");
		
		std::string name = pEntity->GetName().empty() ? pEntity->GetClassName() : pEntity->GetName();
		if (index > 0)
		{
			Entity* pEntityBefore = pParent->GetChild(index-1);
			wxTreeItemId itemIdBefore = this->GetItemIDFromEntity(parentItemId, pEntityBefore);
			SHOOT_ASSERT(itemIdBefore.IsOk(), "Trying to drop an entity on an unexisting tree item");
			InsertItem(parentItemId, itemIdBefore, name, IconManager::Instance()->GetIconIndex(pEntity), IconManager::Instance()->GetIconIndex(pEntity), new ItemData(pEntity));
		}
		else
		{
			PrependItem(parentItemId, name, IconManager::Instance()->GetIconIndex(pEntity), IconManager::Instance()->GetIconIndex(pEntity), new ItemData(pEntity));
		}

		// recursively insert children, if any
		int numChildren = pEntity->GetChildCount();
		for(int i=0; i<numChildren; ++i)
		{
			Entity* pChild = pEntity->GetChild(i);
			InsertEntity(pChild);
		}

		if(pParent->IsA(ContextStack::TypeID) || pParent->IsA(Context::TypeID))		
			Expand(parentItemId);		
	}

	//! Removes an entity from the tree
	void TreeView::RemoveEntity(Entity* pEntity)
	{
		wxTreeItemId itemId = GetItemIDFromEntity(GetRootItem(), pEntity);
		if(itemId.IsOk())
		{
			Delete(itemId);
		}
	}

	//! refreshes an entity
	void TreeView::RefreshEntity(Entity* pEntity)
	{
		wxTreeItemId itemId = GetRootItem().IsOk() ? GetItemIDFromEntity(GetRootItem(), pEntity) : wxTreeItemId();
		if(itemId.IsOk())
		{
			DeleteChildren(itemId);

			// recursively insert children, if any
			int numChildren = pEntity->GetChildCount();
			for(int i=0; i<numChildren; ++i)
			{
				InsertEntity(pEntity->GetChild(i));
			}
		}
	}

	//! Selects the item corresponding to the specified entity
	void TreeView::SelectItemFromEntity(Entity* pEntity)
	{
		// make sure this does not cause an unwanted OnSelChanged() event
		SetEvtHandlerEnabled(false);

		if(pEntity)
		{
			wxTreeItemId itemId = GetItemIDFromEntity(GetRootItem(), pEntity);
			if(itemId.IsOk())
			{
				SelectItem(itemId);
			}
			else
			{
				if(!ShootEditor::Instance()->IsEngineRunning())
				{
					SHOOT_ASSERT(false, "Trying to select an entity that does not have a tree item");			
				}			
			}
		}
		else
		{
			UnselectAll();
		}

		// restore event handling
		SetEvtHandlerEnabled(true);
	}
		
	//! Recursive function to find the item ID associated with an entity
	wxTreeItemId TreeView::GetItemIDFromEntity(wxTreeItemId parentItemId, Entity* pEntiy)
	{
		if(pEntiy == Engine::Instance()->GetContextStack())
		{
			return GetRootItem();
		}

		wxTreeItemIdValue cookie;
		wxTreeItemId childId = this->GetFirstChild(parentItemId, cookie);
		while(childId.IsOk())
		{
			ItemData* pData = static_cast<ItemData*>(GetItemData(childId));
			if(pData->m_Entity.Get() == pEntiy)
			{
				return pData->GetId();
			}
			childId = this->GetNextChild(parentItemId, cookie);
		}

		// not found among children, recurse to reach grand children
		childId = this->GetFirstChild(parentItemId, cookie);
		while(childId.IsOk())
		{
			wxTreeItemId itemId = GetItemIDFromEntity(childId, pEntiy);
			if(itemId.IsOk())
			{
				return itemId;
			}
			childId = this->GetNextChild(parentItemId, cookie);
		}

		// not found, returning an invalid item ID
		return wxTreeItemId();
	}

	//! checks if an entity is an ancestor of another one - used to make sure ancestor items are not dropped on children
	bool TreeView::IsAncestorOf(Entity* pPotentialAncestor, Entity* pEntity)
	{
		if(pEntity->GetParent() == NULL)
		{
			return false;
		}
		else if(pPotentialAncestor == pEntity->GetParent())
		{
			return true;
		}
		else
		{
			return IsAncestorOf(pPotentialAncestor, pEntity->GetParent());
		}
	}

	//! OnPreChangeParent
	Vector3 TreeView::OnPreParentChange(Entity* pEntity)
	{
		Vector3 vAbsPos = Vector3::Zero;
		if(Entity3D* pEntity3D = DYNAMIC_CAST(pEntity, Entity3D))
		{
			vAbsPos = pEntity3D->GetWorldTransform().GetTranslation();
		}
		else if(Entity2D* pEntity2D = DYNAMIC_CAST(pEntity, Entity2D))
		{
			vAbsPos = pEntity2D->GetWorldTransform().GetTranslation();
		}
		return vAbsPos;
	}

	//! OnPostChangeParent
	void TreeView::OnPostParentChange(const Vector3& vPosition, Entity* pEntity)
	{
		if(Entity3D* pEntity3D = DYNAMIC_CAST(pEntity, Entity3D))
		{
			pEntity3D->SetAbsolutePosition(vPosition);
		}
		else if(Entity2D* pEntity2D = DYNAMIC_CAST(pEntity, Entity2D))
		{
			if(!pEntity2D->GetComponent<LayoutComponent>())
				pEntity2D->SetAbsolutePosition(Vector2::Create(vPosition.X, vPosition.Y));
		}
	}

	// event handlers
	void TreeView::OnSelChanged(wxTreeEvent& event)
	{
		Entity* pSelectedEntity = NULL;
		wxTreeItemId itemId = event.GetItem();
		ItemData* pData = itemId.IsOk() ? static_cast<ItemData*>(GetItemData(itemId)) : NULL;
		pSelectedEntity = pData ? pData->m_Entity.Get() : NULL;

		if(pSelectedEntity)
			ShootEditor::Instance()->SelectEntity(pSelectedEntity);		
		else
			Delete(itemId);
	}

	void TreeView::OnBeginDrag(wxTreeEvent& event)
	{
		wxTreeItemId itemId = event.GetItem();
		if(itemId != GetRootItem()) // don't allow dragging of the root
		{
			m_draggedItem = itemId;
			event.Allow();
		}    
	}

	void TreeView::OnEndDrag(wxTreeEvent& event)
	{
		wxTreeItemId srcItem = m_draggedItem;
        wxTreeItemId destItem = event.GetItem();

		if(destItem.IsOk())
		{
			Entity* pSrcEntity = static_cast<ItemData*>(GetItemData(srcItem))->m_Entity.Get();
			Entity* pDestEntity = static_cast<ItemData*>(GetItemData(destItem))->m_Entity.Get();

			if(!IsAncestorOf(pSrcEntity, pDestEntity)) // don't allow dropping a parent into a child
			{	
				if(m_bControlDown)
				{
					Vector3 vAbsPos = OnPreParentChange(pSrcEntity);

					RemoveEntity(pSrcEntity);					
					pDestEntity->AddChild(pSrcEntity, true);
					InsertEntity(pSrcEntity);					
					
					OnPostParentChange(vAbsPos, pSrcEntity);
				}
				else
				{
					Entity* pDestParent = pDestEntity->GetParent();
					for (size_t destIndex = 0; destIndex < pDestParent->GetChildCount(); ++destIndex)
					{
						if(pDestParent->GetChild(destIndex) == pDestEntity)
						{
							Vector3 vAbsPos = OnPreParentChange(pSrcEntity);

							Reference<Entity> srcEntity(pSrcEntity);
							pSrcEntity->GetParent()->RemoveChild(pSrcEntity, true);
							RemoveEntity(pSrcEntity);

							pDestParent->InsertChild(pSrcEntity, destIndex, true);
							InsertEntity(pSrcEntity, destIndex);

							OnPostParentChange(vAbsPos, pSrcEntity);
							break;
						}
					}					
				}
			}
			else
			{
				Log << "Cannot drop a parent item into a child\n";
			}
		}
		m_draggedItem = wxTreeItemId(); // invalidate the dragged item
	}

	void TreeView::OnKeyDown(wxKeyEvent& event)
	{
		m_bControlDown = event.ControlDown();
	}

	void TreeView::OnKeyUp(wxKeyEvent& event)
	{
		m_bControlDown = false;
	}

	void TreeView::OnItemMenu(wxTreeEvent& event)
	{
		ItemData* pItemData = static_cast<ItemData*>(GetItemData(event.GetItem()));
		Entity* pEntity = pItemData ? pItemData->m_Entity.Get() : NULL;
		if(!pEntity)
			return;

		wxMenu subMenu;
		if(ShootEditor::Instance()->GetLockedEntity() != pEntity)
		{
			subMenu.Append(ID_Edit_LockSelection, "Lock Selection");
		}
		else
		{
			subMenu.Append(ID_Edit_UnlockSelection, "Unlock Selection");
		}

		if(!pEntity->GetTemplatePath().empty())
			subMenu.Append(ID_Edit_UnlinkTemplate, "Unlink from template");
		else
			subMenu.Append(ID_Edit_UseTemplate, "Use template");			

		m_pItemMenuEntity = pEntity;
		subMenu.Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&TreeView::OnItemMenuClicked, NULL, this);
		PopupMenu(&subMenu);
	}

	void TreeView::OnItemMenuClicked(wxEvent& event)
	{
		switch(event.GetId())
		{
		case ID_Edit_UnlinkTemplate:
			ObjectManager::Instance()->GetContainer(m_pItemMenuEntity)->strPath.clear();
			ShootEditor::Instance()->GetObjectInspector()->Fill(m_pItemMenuEntity);
			break;

		case ID_Edit_UseTemplate:
		{
			std::string path;
			if (EditorUtils::OpenFileDialog(this, path, "Object files (*.xml)|*.xml", "Select Object"))
			{
				ShootEditor::Instance()->GetObjectInspector()->Load(m_pItemMenuEntity, path);
				ObjectManager::Instance()->GetContainer(m_pItemMenuEntity)->strPath = path;
				ShootEditor::Instance()->GetObjectInspector()->Fill(m_pItemMenuEntity);
			}
		}			
			break;

		case ID_Edit_LockSelection:
			ShootEditor::Instance()->SetLockedEntity(m_pItemMenuEntity);
			break;

		case ID_Edit_UnlockSelection:
			ShootEditor::Instance()->SetLockedEntity(NULL);
			break;
		}
	}
}

