/* 

Amine Rehioui
Created: April 10th 2010

*/

#pragma once


#include "wx/treectrl.h"

namespace shoot
{
	// forwards
	class Entity;

	//! provides a tree view and control over the entity hierarchy
	class TreeView : public wxTreeCtrl
	{
		typedef wxTreeCtrl super;

	public:
		
		//! Constructor
		TreeView(wxWindow *parent, 
				 const wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition, 
				 const wxSize& size = wxSize(200, 300),
                 long style = wxTR_DEFAULT_STYLE);

		//! Destructor
		virtual ~TreeView()
		{
		}

		//! Inserts an entity into the tree
		void InsertEntity(Entity* pEntity);

		//! Inserts an entity into an existing parent entity at a specific index
		void InsertEntity(Entity* pEntity, size_t index);

		//! Removes an entity from the tree
		void RemoveEntity(Entity* pEntity);

		//! refreshes an entity
		void RefreshEntity(Entity* pEntity);

		//! Selects the item corresponding to the specified entity
		void SelectItemFromEntity(Entity* pEntity);

		//! Recursive function to find the item ID associated with an entity	
		wxTreeItemId GetItemIDFromEntity(wxTreeItemId parentItemId, Entity* pEntity);

		// event handlers
		void OnSelChanged(wxTreeEvent& event); 
		void OnBeginDrag(wxTreeEvent& event);
		void OnEndDrag(wxTreeEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		void OnKeyUp(wxKeyEvent& event);
		void OnItemMenu(wxTreeEvent& event);
		void OnItemMenuClicked(wxEvent& event);
	
	private:

		wxTreeItemId m_draggedItem;
		bool m_bControlDown;
		Entity* m_pItemMenuEntity;

		//! checks if an entity is a ancestor of another one - used to make sure ancestor items are not dropped on children
		bool IsAncestorOf(Entity* pPotentialAncestor, Entity* pEntity);

		//! OnPreChangeParent
		Vector3 OnPreParentChange(Entity* pEntity);

		//! OnPostChangeParent
		void OnPostParentChange(const Vector3& vPosition, Entity* pEntity);

		//! struct to map between the tree items and the entities
		struct ItemData : public wxTreeItemData
		{
			ItemData(Entity* pEntity) : m_Entity(pEntity)
			{
			}
			Handle<Entity> m_Entity;
		};

		DECLARE_EVENT_TABLE();
	};	
}



