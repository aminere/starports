/* 

Amine Rehioui
Created: July 7th 2012

*/

#include "ShootEditorCommon.h"

#include "UndoManager.h"
#include "ToolbarManager.h"

namespace shoot
{
	//! constructor
	UndoManager::UndoManager()
	{
	}

	//! destructor
	UndoManager::~UndoManager()
	{
		ClearHistory();		
	}

	//! records a property state
	void UndoManager::RecordPropertyState(Object* pObject, IProperty* pProperty)
	{
		PropertyChangeInfo change(pObject);
		change.m_Properties.push_back(pProperty->Copy());
		RecordPropertyChange(change);		
	}

	//! records transformation state
	void UndoManager::RecordTransformState(Entity2D* pEntity2D)
	{
		PropertyStream stream(shoot::SM_Write);
		pEntity2D->Serialize(stream);

		PropertyChangeInfo change(pEntity2D);
		std::string aProperties[3] = { "Position", "Rotation", "Scale" };
		for(int i=0; i<3; ++i)
		{
			if(IProperty* pProperty = stream.GetProperty(aProperties[i]))			
				change.m_Properties.push_back(pProperty->Copy());
		}

		RecordPropertyChange(change);
	}

	//! records transformation state
	void UndoManager::RecordTransformState(Entity3D* pEntity3D)
	{
		PropertyStream stream(shoot::SM_Write);
		pEntity3D->Serialize(stream);

		PropertyChangeInfo change(pEntity3D);
		std::string aProperties[3] = { "Position", "Rotation", "Scale" };
		for(int i=0; i<3; ++i)
		{
			if(IProperty* pProperty = stream.GetProperty(aProperties[i]))			
				change.m_Properties.push_back(pProperty->Copy());
		}

		RecordPropertyChange(change);
	}

	//! records transformation state
	void UndoManager::RecordTransformState(Camera* pCamera)
	{
		PropertyStream stream(shoot::SM_Write);
		pCamera->Serialize(stream);

		PropertyChangeInfo change(pCamera);
		change.m_Properties.push_back(stream.GetProperty("Position")->Copy());
		change.m_Properties.push_back(stream.GetProperty("LookAt")->Copy());		

		RecordPropertyChange(change);
	}

	//! performs an Undo
	void UndoManager::Undo()
	{
		if(m_UndoStack.empty())
			return;

		PropertyChangeInfo change = m_UndoStack.back();

		std::vector<IProperty*> oldProperties;
		for(size_t i=0; i<change.m_Properties.size(); ++i)
		{
			IProperty* pOldProperty = DoPropertyChange(change.m_pObject, change.m_Properties[i]);
			oldProperties.push_back(pOldProperty);
		}			

		change.Clear();
		change.m_Properties = oldProperties;
				
		m_RedoStack.push_back(change);
		m_UndoStack.pop_back();

		ToolbarManager::Instance()->SetUndoEnabled(!m_UndoStack.empty());
		ToolbarManager::Instance()->SetRedoEnabled(true);
	}

	//! performs a Redo
	void UndoManager::Redo()
	{
		if(m_RedoStack.empty())
			return;

		PropertyChangeInfo change = m_RedoStack.back();

		std::vector<IProperty*> oldProperties;
		for (size_t i = 0; i<change.m_Properties.size(); ++i)
		{
			IProperty* pOldProperty = DoPropertyChange(change.m_pObject, change.m_Properties[i]);
			oldProperties.push_back(pOldProperty);
		}			

		change.Clear();
		change.m_Properties = oldProperties;

		m_UndoStack.push_back(change);
		m_RedoStack.pop_back();

		ToolbarManager::Instance()->SetUndoEnabled(true);
		ToolbarManager::Instance()->SetRedoEnabled(!m_RedoStack.empty());
	}

	//! clears entire history
	void UndoManager::ClearHistory()
	{
		ClearHistory(m_UndoStack);
		ClearHistory(m_RedoStack);
	}

	//! clears an object history
	void UndoManager::ClearHistory(Object* pObject)
	{
		ClearHistory(pObject, m_UndoStack);
		ClearHistory(pObject, m_RedoStack);

		ToolbarManager::Instance()->SetUndoEnabled(!m_UndoStack.empty());
		ToolbarManager::Instance()->SetRedoEnabled(!m_RedoStack.empty());
	}

	//! record property change
	void UndoManager::RecordPropertyChange(const PropertyChangeInfo& change)
	{
		m_UndoStack.push_back(change);
		ClearHistory(m_RedoStack);

		ToolbarManager::Instance()->SetUndoEnabled(true);
		ToolbarManager::Instance()->SetRedoEnabled(false);
	}

	//! performs a property change
	IProperty* UndoManager::DoPropertyChange(Object* pObject, IProperty* pProperty)
	{
		PropertyStream stream(shoot::SM_Write);
		pObject->Serialize(stream);		

		for (size_t i = 0; i<stream.GetNumProperties(); ++i)
		{
			if(stream.GetProperty(i)->GetName() == pProperty->GetName())
			{
				IProperty* pOldProperty = stream.GetProperty(i);
				stream.GetProperties()[i] = pProperty;

				stream.SetMode(shoot::SM_Read);
				stream.SetPropertyToUpdate(pProperty->GetName());
				pObject->Serialize(stream);

				stream.GetProperties()[i] = pOldProperty;

				// update property list
				PropertyList* pProperties = ShootEditor::Instance()->GetObjectInspector()->GetPropertyList();
				if(pProperties->GetSelectedObject() == pObject)
				{
					pProperties->Fill(pObject);
				}

				IProperty* pOldCopy = pOldProperty->Copy();
				return pOldCopy;
			}
		}
		return NULL;
	}
	
	//! clears entire history in a stack
	void UndoManager::ClearHistory(std::vector<PropertyChangeInfo>& stack)
	{
		for (size_t i=0; i<stack.size(); ++i)
			stack[i].Clear();			
		
		stack.clear();
	}

	//! clears entire history in a stack
	void UndoManager::ClearHistory(Object* pObject, std::vector<PropertyChangeInfo>& stack)
	{
		for(std::vector<PropertyChangeInfo>::iterator it = stack.begin(); it != stack.end(); )
		{
			if((*it).m_pObject == pObject)
			{
				(*it).Clear();
				it = stack.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}

