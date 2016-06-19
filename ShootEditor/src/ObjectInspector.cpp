/* 

Amine Rehioui
Created: January 14th 2012

*/	

#include "ShootEditorCommon.h"

#include "PropertyLocator.h"
#include "wxArrayProperty.h"
#include "wxReferenceProperty.h"
#include "wxCommonProperties.h"
#include "Visitor.h"
#include "EditorUtils.h"
#include "Image.h"
#include "MeshEntity.h"

#include "IconManager.h"
#include "EventManager.h"
#include "EditorEvents.h"
#include "ObjectSelector.h"
#include "UndoManager.h"

#include "wx/sizer.h"
#include <wx/bmpbuttn.h>
#include <wx/artprov.h> // wxArtProvider
#include <wx/splitter.h>

#include "rapidxml/rapidxml.hpp"

namespace shoot
{
	// Define event table
	BEGIN_EVENT_TABLE(ObjectInspector, wxPanel)				
		EVT_COMMAND(ID_ObjectInspector_Load, wxEVT_COMMAND_BUTTON_CLICKED, OnLoad)
		EVT_COMMAND(ID_ObjectInspector_Save, wxEVT_COMMAND_BUTTON_CLICKED, OnSave)
		EVT_COMMAND(ID_ObjectInspector_SaveAs, wxEVT_COMMAND_BUTTON_CLICKED, OnSaveAs)
		EVT_COMMAND(ID_ObjectInspector_Reload, wxEVT_COMMAND_BUTTON_CLICKED, OnReload)
		EVT_COMMAND(ID_ObjectInspector_AddComponent, wxEVT_COMMAND_BUTTON_CLICKED, OnAddComponent)
		EVT_COMMAND(ID_ObjectInspector_RemoveComponent, wxEVT_COMMAND_BUTTON_CLICKED, OnRemoveComponent)
		EVT_COMMAND(ID_ObjectInspector_CopyComponent, wxEVT_COMMAND_BUTTON_CLICKED, OnCopyComponent)
		EVT_COMMAND(ID_ObjectInspector_PasteComponent, wxEVT_COMMAND_BUTTON_CLICKED, OnPasteComponent)		
		EVT_TREE_SEL_CHANGED(ID_ObjectInspector_Tree, OnSelectItem)
		EVT_TREE_ITEM_MENU(ID_ObjectInspector_Tree, OnTreeItemMenu)
	END_EVENT_TABLE()

	//! Constructor
	ObjectInspector::ObjectInspector(wxWindow* pParent,	
								     wxWindowID id /*= wxID_ANY*/, 
								     const wxPoint& pos /*= wxDefaultPosition*/, 
								     const wxSize& size /*= wxSize(250, 300)*/, 
								     long style /*= 0*/) 
	: super(pParent, id, pos, size, style)
	{
		IconManager* pIconMgr = IconManager::Instance();

		wxSize buttonSize = wxSize(24, 22);
		wxBoxSizer *pCommands = new wxBoxSizer( wxHORIZONTAL );
		pCommands->Add(new wxBitmapButton(this, ID_ObjectInspector_Load, wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_OTHER, buttonSize), wxDefaultPosition, buttonSize)); 		
		pCommands->Add(new wxBitmapButton(this, ID_ObjectInspector_Save, wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_OTHER, buttonSize), wxDefaultPosition, buttonSize));
		pCommands->Add(new wxBitmapButton(this, ID_ObjectInspector_SaveAs, wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS, wxART_OTHER, buttonSize), wxDefaultPosition, buttonSize));
		pCommands->Add(m_pReloadButton = new wxButton(this, ID_ObjectInspector_Reload, "R", wxDefaultPosition, buttonSize), 0, wxALIGN_CENTRE_VERTICAL);		
		pCommands->Add(new wxBitmapButton(this, ID_ObjectInspector_AddComponent, pIconMgr->GetBitmap(IconManager::Icon_Add), wxDefaultPosition, buttonSize)); 		
		pCommands->Add(new wxBitmapButton(this, ID_ObjectInspector_RemoveComponent, pIconMgr->GetBitmap(IconManager::Icon_Remove), wxDefaultPosition, buttonSize));
		pCommands->Add(m_pComponentCopyButton = new wxButton(this, ID_ObjectInspector_CopyComponent, "C", wxDefaultPosition, buttonSize), 0, wxALIGN_CENTRE_VERTICAL);
		pCommands->Add(m_pComponentPasteButton = new wxButton(this, ID_ObjectInspector_PasteComponent, "P", wxDefaultPosition, buttonSize), 0, wxALIGN_CENTRE_VERTICAL);

		auto splitter = new wxSplitterWindow(this);
		m_pTreeControl = new wxTreeCtrl(splitter, ID_ObjectInspector_Tree, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT);
		m_pTreeControl->SetImageList(pIconMgr->GetImageList());				
		m_pPropertyList = new PropertyList(splitter, this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
		splitter->SplitHorizontally(m_pTreeControl, m_pPropertyList);
		splitter->SetSashGravity(0.30);

		wxBoxSizer *pSizer = new wxBoxSizer( wxVERTICAL );
		pSizer->Add(pCommands);
		pSizer->Add(splitter, wxSizerFlags(1).Expand());
		SetSizer(pSizer);
		pSizer->FitInside(this);
	}

	//! Fills the object inspector
	void ObjectInspector::Fill(Object* pObject)
	{
		m_pTreeControl->DeleteAllItems();
		m_pPropertyList->Clear();

		if(pObject)		
		{
			auto treeRoot = m_pTreeControl->AddRoot("root");			

			// fill object
			auto objectName = EditorUtils::GetObjectName(pObject);			
			auto objectRoot = m_pTreeControl->AppendItem(treeRoot, objectName, IconManager::Instance()->GetIconIndex(pObject), IconManager::Instance()->GetIconIndex(pObject), new EditorUtils::ItemData(pObject));
			if(Entity* pEntity = DYNAMIC_CAST(pObject, Entity))
			{
				// fill components
				int numComponents = pEntity->GetComponentCount();
				for(int i=0; i<numComponents; ++i)
				{
					Component* pComponent = pEntity->GetComponent(i);
					wxString itemName = pComponent->GetClassName();
					if (!pComponent->GetTemplatePath().empty())
						itemName += " (" + EditorUtils::GetTemplatePath(pComponent) + ")";
					m_pTreeControl->AppendItem(objectRoot, itemName, IconManager::Instance()->GetIconIndex(pComponent), IconManager::Instance()->GetIconIndex(pComponent), new EditorUtils::ItemData(pComponent));
				}
			}

			m_pTreeControl->Expand(treeRoot);
			m_pTreeControl->Expand(objectRoot);
			m_pTreeControl->SelectItem(objectRoot);
		}
		else
		{
			m_pReloadButton->Enable(false);
			m_pComponentCopyButton->Enable(false);
			m_pComponentPasteButton->Enable(false);
		}
	}

	//! updates a property
	void ObjectInspector::UpdateProperty(const char* strPropertyName, wxVariant& value)
	{
		Entity* pEntity = ShootEditor::Instance()->GetSelectedEntity();

		// make property overriden if not already done
		if(pEntity
		&& !pEntity->GetTemplatePath().empty()
		&& !pEntity->GetOverridenProperties().GetProperty(strPropertyName))
		{
			PropertyStream stream(SM_Write);
			pEntity->Serialize(stream);
			pEntity->GetOverridenProperties().AddProperty(stream.GetProperty(strPropertyName)->Copy());

			if (m_pPropertyList->GetSelectedObject() == pEntity)
			{
				// make sure we're updating the selected entitie's property list
				m_pPropertyList->SetPropertyValue(strPropertyName, value);
				m_pPropertyList->EnableProperty(strPropertyName);
			}
		}
	}

	//! loads an object from a path
	void ObjectInspector::Load(Object* pObject, const std::string& path)
	{
		if(Entity* pEntity = DYNAMIC_CAST(pObject, Entity))
		{
			pEntity->Uninit();
			pEntity->LoadFromXML(path.c_str());
			pEntity->Init();

			// update name an children in tree
			ShootEditor* pEditor = ShootEditor::Instance();
			wxTreeItemId itemId = pEditor->GetTreeView()->GetItemIDFromEntity(pEditor->GetTreeView()->GetRootItem(), pEntity);
			if(itemId.IsOk())
			{
				std::string name = pEntity->GetName().empty() ? pEntity->GetClassName() : pEntity->GetName();
				pEditor->GetTreeView()->SetItemText(itemId, name);
				pEditor->GetTreeView()->RefreshEntity(pEntity);
			}

			// refresh new entity
			Fill(pEntity);
		}
		else if(Component* pComponent = DYNAMIC_CAST(pObject, Component))
		{
			rapidxml::xml_document<> document;
			try
			{
				document.parseFile<0>(path);
				auto root = document.first_node();		
				pComponent->LoadFromXML(root);
				pComponent->Init();

				// refresh new component
				m_pPropertyList->Fill(pObject);
			}
			catch(rapidxml::parse_error e)
			{
				SHOOT_ASSERT(false, "Could not load file '%s'", path.c_str());
			}
		}
	}

	//! adds a component to an entity
	void ObjectInspector::AddComponent(Entity* pEntity, Component* pComponent)
	{
		pEntity->AddComponent(pComponent, true);
		pComponent->Init();

		auto objectRoot = GetObjectRoot();
		m_pTreeControl->AppendItem(objectRoot, pComponent->GetClassName(), IconManager::Instance()->GetIconIndex(pComponent), IconManager::Instance()->GetIconIndex(pComponent), new EditorUtils::ItemData(pComponent));
		m_pTreeControl->Expand(objectRoot);
	}

	//! GetObjectRoot
	wxTreeItemId ObjectInspector::GetObjectRoot() const
	{
		wxTreeItemIdValue cookie;
		auto root = m_pTreeControl->GetRootItem();
		auto objectRoot = m_pTreeControl->GetFirstChild(root, cookie);
		return objectRoot;
	}

	// event handlers
	void ObjectInspector::OnLoad(wxCommandEvent& event)
	{
		if(Entity* pEntity = ShootEditor::Instance()->GetSelectedEntity())
		{
			std::string path;
			if(EditorUtils::OpenFileDialog(this, path, "Object files (*.xml)|*.xml", "Select Object"))
			{
				Load(pEntity, path);
			}
		}
	}

	void ObjectInspector::OnReload(wxCommandEvent& event)
	{
		if(Object* pObject = m_pPropertyList->GetSelectedObject())
		{
			std::string path = pObject->GetTemplatePath();
			SHOOT_ASSERT(!path.empty(), "Object to reload has no template path");
			Load(pObject, path);

			if(Entity* pEntity = DYNAMIC_CAST(pObject, Entity))
			{
				pEntity->Serialize(pEntity->GetOverridenProperties());

				if(Context* pContext = pEntity->GetRootContext())
				{
					QuadTree* pQuadTree = pContext->GetQuadTree();
					if(pQuadTree && pEntity->IsA(Entity3D::TypeID))
					{
						pQuadTree->ExtractEntities(pEntity);
						pQuadTree->Rebuild();
					}
				}
			}
		}
	}

	void ObjectInspector::OnSave(wxCommandEvent& event)
	{
		if(Object* pObject = m_pPropertyList->GetSelectedObject())
		{
			std::string path = pObject->GetTemplatePath();
			bool bExists = !pObject->GetTemplatePath().empty();
			if(bExists || EditorUtils::OpenFileDialog(this, path, "Object files (*.xml)|*.xml", "Save File", wxFD_SAVE))			
			{
				EditorUtils::FormatObjectPath(pObject, path);
				pObject->SaveToXML(path.c_str());
				ObjectManager::Instance()->SetTemplatePath(pObject, path);				

				if(Entity* pEntity = DYNAMIC_CAST(pObject, Entity))
					pEntity->GetOverridenProperties().Clear();
			}
		}
	}

	void ObjectInspector::OnSaveAs(wxCommandEvent& event)
	{		
		if(Object* pObject = m_pPropertyList->GetSelectedObject())
		{
			std::string path;
			if(EditorUtils::OpenFileDialog(this, path, "Object files (*.xml)|*.xml", "Save File", wxFD_SAVE))			
			{
				EditorUtils::FormatObjectPath(pObject, path);
				pObject->SaveToXML(path.c_str());
			}
		}
	}

	void ObjectInspector::OnAddComponent(wxCommandEvent& event)
	{
		if(Entity* pEntity = ShootEditor::Instance()->GetSelectedEntity())
		{
			Object* pObject = NULL;

			if(wxMessageBox("Add from a template?", "Add from a template?", wxYES_NO | wxICON_QUESTION) == wxYES)
			{
				std::string path;
				if(EditorUtils::OpenFileDialog(this, path, "Object files (*.xml)|*.xml", "Select Object"))
					pObject = ObjectManager::Instance()->Load(path.c_str());
			}
			else
			{
				std::string className;
				ObjectSelector selector(this);
				if(selector.Select(Component::GetStaticClassName(), className))
					pObject = ObjectFactory::Instance()->Create(className);
			}

			if(pObject)
			{
				Component* pComponent = DYNAMIC_CAST(pObject, Component);
				SHOOT_ASSERT(pComponent, "Cannot create an Component from Object");
				AddComponent(pEntity, pComponent);
			}
		}
	}

	void ObjectInspector::OnRemoveComponent(wxCommandEvent& event)
	{
		if (!m_pTreeControl->GetRootItem().IsOk()
		 || m_pTreeControl->GetChildrenCount(m_pTreeControl->GetRootItem()) == 0)
			return;

		Entity* pEntity = ShootEditor::Instance()->GetSelectedEntity();
		SHOOT_ASSERT(pEntity, "Cannot RemoveComponent on NULL entity");
		wxTreeItemId selectedItem = m_pTreeControl->GetSelection();
		auto pItemData = static_cast<EditorUtils::ItemData*>(m_pTreeControl->GetItemData(selectedItem));
		auto pComponent = DYNAMIC_CAST(pItemData->m_pObject, Component);
		if (selectedItem.IsOk() && pComponent)
		{
			if (wxMessageBox("Do you really want to remove the selected component?",
				"Comfirm removal",
				wxYES_NO | wxICON_QUESTION) == wxYES)
			{
				pEntity->RemoveComponent(pComponent, true);
				m_pTreeControl->Delete(selectedItem);
				m_pTreeControl->SelectItem(GetObjectRoot());
			}
		}
	}

	void ObjectInspector::OnCopyComponent(wxCommandEvent& event)
	{
		SHOOT_ASSERT(m_pPropertyList->GetSelectedObject()->IsA(Component::TypeID), "Called OnCopyComponent on a non-component");
		m_ComponentClipboard = static_cast<Component*>(m_pPropertyList->GetSelectedObject()->Copy());
	}

	void ObjectInspector::OnPasteComponent(wxCommandEvent& event)
	{
		SHOOT_ASSERT(m_pPropertyList->GetSelectedObject()->IsA(Entity::TypeID), "Called OnPasteComponent on a non-entity");
		AddComponent(static_cast<Entity*>(m_pPropertyList->GetSelectedObject()), static_cast<Component*>(m_ComponentClipboard->Copy()));
	}

	void ObjectInspector::OnSelectItem(wxTreeEvent& event)
	{
		wxTreeItemId itemId = event.GetItem();
		if(itemId.IsOk())
		{
			if (auto data = static_cast<EditorUtils::ItemData*>(m_pTreeControl->GetItemData(itemId)))
			{
				Object* pObject = data->m_pObject;
				m_pPropertyList->Fill(pObject);

				bool bIsTemplateInstance = !pObject->GetTemplatePath().empty();
				m_pReloadButton->Enable(bIsTemplateInstance);
				m_pComponentCopyButton->Enable(pObject->IsA(Component::TypeID));
				m_pComponentPasteButton->Enable(m_ComponentClipboard.IsValid() && pObject->IsA(Entity::TypeID));
			}			
		}
	}

	void ObjectInspector::OnTreeItemMenu(wxTreeEvent& event)
	{
		wxTreeItemId itemId = event.GetItem();
		if (!itemId.IsOk())
			return;

		auto data = static_cast<EditorUtils::ItemData*>(m_pTreeControl->GetItemData(itemId));
		if (!data)
			return;

		wxMenu subMenu;
		subMenu.Append(ID_ObjectInspector_ViewDependencies, "View Dependencies");
		subMenu.Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&ObjectInspector::OnTreeItemMenuClicked, NULL, this);
		PopupMenu(&subMenu);
	}

	void ObjectInspector::OnTreeItemMenuClicked(wxEvent& event)
	{
		switch (event.GetId())
		{
		case ID_ObjectInspector_ViewDependencies:
		{
			auto tabControl = ShootEditor::Instance()->GetMainTabControl();
			tabControl->SetSelection(1);
			tabControl->SelectObject(m_pPropertyList->GetSelectedObject());
		}			
		break;
		}
	}

	void ObjectInspector::OnPropertyMenuClicked(wxEvent& event)
	{
		PropertyStream stream(SM_Write);
		m_PropertyMenuData.pObject->Serialize(stream);

		auto wxProperty = m_PropertyMenuData.pWxProperty;
		auto property = PropertyLocator().GetProperty(wxProperty, stream);

		IProperty* parentProperty = NULL;
		if (!wxProperty->GetParent()->IsRoot())
			parentProperty = PropertyLocator().GetProperty(wxProperty->GetParent(), stream);

		if (event.GetId() == ID_Property_ViewDependencies)
		{
			auto tabControl = ShootEditor::Instance()->GetMainTabControl();
			tabControl->SetSelection(1);
			tabControl->SelectObject(m_PropertyMenuData.pPropertyObject);
			Fill(m_PropertyMenuData.pPropertyObject);
		}
		else if (parentProperty && parentProperty->GetType() == PT_Array)
		{
			ArrayProperty* pArrayProperty = static_cast<ArrayProperty*>(parentProperty);
			wxArrayProperty* pWxArrayProperty = static_cast<wxArrayProperty*>(wxProperty->GetParent());
			wxPGProperty* pWxParentProperty = pWxArrayProperty->GetParent();
			IProperty* pSubProperty = pArrayProperty->GetProperties()[m_PropertyMenuData.propertyIndex];
			int indexInParent = pWxArrayProperty->GetIndexInParent();
			m_pPropertyList->DeleteProperty(pWxArrayProperty);

			switch (event.GetId())
			{
			case ID_Property_Remove:
			{
				// delete the property
				pArrayProperty->GetProperties().erase(pArrayProperty->GetProperties().begin() + m_PropertyMenuData.propertyIndex);
				delete pSubProperty;

				// delete the corresponding element in the user data
				IArray* pArray = pArrayProperty->GetArray();
				pArray->Delete(m_PropertyMenuData.propertyIndex);
			}
			break;

			case ID_Property_Copy:
			{
				// update property
				auto propertyCopy = pSubProperty->Copy();
				auto newIndex = m_PropertyMenuData.propertyIndex + 1;
				pArrayProperty->GetProperties().insert(pArrayProperty->GetProperties().begin() + newIndex, propertyCopy);

				// update user data
				switch (propertyCopy->GetType())
				{
				case PT_Reference:
				{
					auto referencePropertyCopy = static_cast<ReferenceProperty*>(propertyCopy);
					if (referencePropertyCopy->GetRefInterface())
						referencePropertyCopy->SetRefInterface(referencePropertyCopy->GetRefInterface()->Copy());

					pArrayProperty->GetArray()->Insert(newIndex, referencePropertyCopy->GetRefInterface());
				}
				break;

				case PT_Struct:
				case PT_Link:
				{
					auto elemCopy = pArrayProperty->GetArray()->CopyElement(m_PropertyMenuData.propertyIndex);
					pArrayProperty->GetArray()->Insert(newIndex, elemCopy);
				}
				break;

				default:
				{
					void* elemCopy = propertyCopy->CopyData();
					SHOOT_ASSERT(elemCopy != NULL, "Property copy error");
					pArrayProperty->GetArray()->Insert(newIndex, elemCopy);
				}
				}
			}
			break;

			case ID_Property_MoveUp:
			case ID_Property_MoveDown:
			{
				// move the property
				IProperty* pSubProperty = pArrayProperty->GetProperties()[m_PropertyMenuData.propertyIndex];
				pArrayProperty->GetProperties().erase(pArrayProperty->GetProperties().begin() + m_PropertyMenuData.propertyIndex);
				int newIndex = m_PropertyMenuData.propertyIndex + ((event.GetId() == ID_Property_MoveDown) ? 1 : -1);
				pArrayProperty->GetProperties().insert(pArrayProperty->GetProperties().begin() + newIndex, pSubProperty);

				// move the corresponding element in the user data
				IArray* pArray = pArrayProperty->GetArray();
				void* pElement = pArray->Remove(m_PropertyMenuData.propertyIndex);
				pArray->Insert(newIndex, pElement);
			}
			break;

			case ID_Property_DifferentiateMaterial:
			{
				// update user data
				auto referenceProperty = static_cast<ReferenceProperty*>(pSubProperty);
				auto refInterface = referenceProperty->GetRefInterface();
				auto material = static_cast<Material*>(refInterface->Get());
				auto materialCopy = material->Copy();
				refInterface->SetObject(materialCopy);
			}
			break;
			}

			wxPGProperty* pNewProperty = PropertyList::GetWxProperty(pArrayProperty, m_pPropertyList);
			m_pPropertyList->Insert(pWxParentProperty, indexInParent, pNewProperty);
			m_pPropertyList->FillArrayProperties(pArrayProperty, pNewProperty);
		}
		else
		{			
			switch (event.GetId())
			{
			case ID_Property_Override:
			{
				auto entity = static_cast<Entity*>(m_PropertyMenuData.pObject);
				auto& overridenProperties = entity->GetOverridenProperties();
				overridenProperties.AddProperty(property->Copy());
				m_pPropertyList->EnableProperty(m_PropertyMenuData.pWxProperty);
			}
			break;

			case ID_Property_Reset:
			{
				auto entity = static_cast<Entity*>(m_PropertyMenuData.pObject);
				auto& overridenProperties = entity->GetOverridenProperties();
				overridenProperties.RemoveProperty(overridenProperties.GetProperty(property->GetName()));
				m_pPropertyList->DisableProperty(m_PropertyMenuData.pWxProperty);
			}
			break;
			}
		}		
	}

	//! call on property change
	void ObjectInspector::OnPropertyChanged(wxPropertyGridEvent &event)
	{
		Object* pObject = m_pPropertyList->GetSelectedObject();

		PropertyStream stream(shoot::SM_Write);
		pObject->Serialize(stream);

		// check if this is a sub property
		wxPGProperty* pWxProperty = event.GetProperty();
		if(!pWxProperty->GetParent()->IsRoot()) 
		{
			PropertyLocator locator;
			IProperty* pParentProperty = locator.GetProperty(pWxProperty->GetParent(), stream);

			// if this was part of an array, mark it
			if(pParentProperty->GetType() == PT_Array)
			{
				wxArrayProperty* pWxArrayParent = static_cast<wxArrayProperty*>(pWxProperty->GetParent());
				pWxArrayParent->SetEventType(wxArrayProperty::E_SubPropertyChanged);
				pWxArrayParent->SetChangedSubPropertyIndex(pWxProperty->GetIndexInParent());
			}			
			else if(pParentProperty->GetType() == PT_Reference)
			{
				wxReferenceProperty* pWxRefParent = static_cast<wxReferenceProperty*>(pWxProperty->GetParent());
				pWxRefParent->SetEventType(wxReferenceProperty::E_SubPropertyChanged);				
			}
			
			IProperty* pRootProperty = locator.GetRootProperty(pWxProperty, stream);

			// add to undo stack
			UndoManager::Instance()->RecordPropertyState(pObject, pRootProperty);

			PropertyList::FillProperty(pWxProperty->GetParent(), pParentProperty);
			stream.SetMode(shoot::SM_Read);
			stream.SetPropertyToUpdate(pRootProperty->GetName());
			pObject->Serialize(stream);
		}
		else if(pWxProperty->GetName() == "AbsolutePosition")
		{
			Entity3D* pEntity3D = DYNAMIC_CAST(pObject, Entity3D);
			SHOOT_ASSERT(pEntity3D, "AbsolutePosition property detected on a non-Entity3D");
			auto pos = Vector3RefFromVariant(pWxProperty->GetValue());
			pEntity3D->SetAbsolutePosition(pos);
		}
		else
		{
			IProperty* pProperty = PropertyLocator().GetProperty(pWxProperty, stream);
			SHOOT_ASSERT(pProperty->GetName() == pWxProperty->GetName(), "wxProperty and shoot Property mismatch");

			// add to undo stack
			bool undoable = true;
			if(pProperty->GetType() == PT_Array)
				undoable = false;
				
			if(undoable)
				UndoManager::Instance()->RecordPropertyState(pObject, pProperty);

			PropertyList::FillProperty(pWxProperty, pProperty);
			stream.SetMode(shoot::SM_Read);
			stream.SetPropertyToUpdate(pProperty->GetName());
			pObject->Serialize(stream);
			
			if(pObject->IsA(Entity::TypeID))
			{
				// if an entity's name has changed, change it in the tree control as well
				if(pProperty->GetName() == "Name")
				{
					if(pProperty->GetType() != PT_Reference)
					{
						Entity* pEntity = static_cast<Entity*>(pObject);
						ShootEditor* pEditor = ShootEditor::Instance();
						wxTreeItemId itemId = pEditor->GetTreeView()->GetItemIDFromEntity(pEditor->GetTreeView()->GetRootItem(), pEntity);			 
						pEditor->GetTreeView()->SetItemText(itemId, pProperty->GetString());
					}

					// if a camera name changed, update the camera selector				
					if(pObject->IsA(Camera::TypeID))
						ShootEditor::Instance()->UpdateCameraSelectors();
				}				
			}			
		}

		// If we changed a visitor property, reinit the visitor
		if(Visitor* pVisitor = DYNAMIC_CAST(pObject, Visitor))
		{
			if(pVisitor->GetAutoStart())
			{
				if(pVisitor->IsActive())
					pVisitor->Leave();
				
				pVisitor->Init();
			}
		}
		else if(Component* pComponent = DYNAMIC_CAST(pObject, Component))
		{			
			// if changed the component of an image, update the geometry accordingly
			if(Image* pImage = DYNAMIC_CAST(pComponent->GetOwner(), Image))			
				pImage->UpdateGeometry();			
		}
		else if(Entity* pEntity = DYNAMIC_CAST(pObject, Entity))
		{
			ShootEditor::Instance()->GetTreeView()->RefreshEntity(pEntity);			
		}
	}

	//! called while a property is changing
	void ObjectInspector::OnPropertyChanging(wxPropertyGridEvent &event)
	{
		// TODO
	}

	//! called while a property is selected
	void ObjectInspector::OnPropertySelected(wxPropertyGridEvent &event)
	{
		// TODO
	}

	//! called while a property is right clicked
	void ObjectInspector::OnPropertyRightClick(wxPropertyGridEvent &event)
	{
		auto pWxProperty = event.GetProperty();
		auto parentIsRoot = pWxProperty->GetParent()->IsRoot();
		auto pObject = m_pPropertyList->GetSelectedObject();
		wxMenu subMenu;
		
		PropertyStream stream(shoot::SM_Write);
		pObject->Serialize(stream);
		PropertyLocator locator;
		auto property = locator.GetProperty(pWxProperty, stream);
		auto parentIsArray = false;
		if (!parentIsRoot)
		{
			auto parentProperty = locator.GetProperty(pWxProperty->GetParent(), stream);
			parentIsArray = (parentProperty->GetType() == PT_Array);
		}
		
		if (parentIsArray)
		{
			subMenu.Append(ID_Property_Remove, "Remove");
			subMenu.Append(ID_Property_Copy, "Copy");

			if (pWxProperty->GetIndexInParent() > 0)
				subMenu.Append(ID_Property_MoveUp, "Move Up");

			if (pWxProperty->GetIndexInParent() < pWxProperty->GetParent()->GetChildCount() - 1)
				subMenu.Append(ID_Property_MoveDown, "Move Down");

			if (property->GetType() == PT_Reference)
			{
				auto referenceProperty = static_cast<ReferenceProperty*>(property);
				if (auto material = DYNAMIC_CAST(referenceProperty->GetRefInterface()->Get(), Material))
				{
					auto container = ObjectManager::Instance()->GetContainer(material);
					if (container->lReferences.size() > 1)
						subMenu.Append(ID_Property_DifferentiateMaterial, "Differentiate Material");
				}
			}
			
			m_PropertyMenuData.propertyIndex = pWxProperty->GetIndexInParent();
		}
		else if (parentIsRoot)
		{
			auto entity = DYNAMIC_CAST(pObject, Entity);
			auto isTemplateProperty = (entity!= NULL)
				&& !entity->GetTemplatePath().empty() 
				&& pWxProperty->GetName() != "AbsolutePosition";

			if (isTemplateProperty)
			{
				auto& overridenProperties = entity->GetOverridenProperties();
				auto propertyName = std::string(pWxProperty->GetName());
				if (overridenProperties.GetProperty(propertyName))
					subMenu.Append(ID_Property_Reset, "Reset");
				else
					subMenu.Append(ID_Property_Override, "Override");
			}
		}

		if (property->GetType() == PT_Reference)
		{
			auto referenceProperty = static_cast<ReferenceProperty*>(property);
			if (auto propertyObject = referenceProperty->GetRefInterface()->Get())
			{
				subMenu.Append(ID_Property_ViewDependencies, "View Dependencies");
				m_PropertyMenuData.pPropertyObject = propertyObject;
			}
		}
		
		m_PropertyMenuData.pWxProperty = pWxProperty;
		m_PropertyMenuData.pObject = pObject;
		subMenu.Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&ObjectInspector::OnPropertyMenuClicked, NULL, this);
		PopupMenu(&subMenu);
	}
}

