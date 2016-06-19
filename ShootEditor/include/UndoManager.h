/* 

Amine Rehioui
Created: July 7th 2012

*/

#pragma once


#include "Camera.h"

namespace shoot
{
	//! Undo manager class
	class UndoManager : public Singleton<UndoManager>
	{
		DECLARE_SINGLETON(UndoManager);

	public:	

		//! records a property state
		void RecordPropertyState(Object* pObject, IProperty* pProperty);

		//! records transformation state
		void RecordTransformState(Entity2D* pEntity2D);

		//! records transformation state
		void RecordTransformState(Entity3D* pEntity3D);

		//! records transformation state
		void RecordTransformState(Camera* pCamera);

		//! performs an Undo
		void Undo();

		//! performs a Redo
		void Redo();

		//! clears entire history
		void ClearHistory();

		//! clears an object history
		void ClearHistory(Object* pObject);		

	private:

		struct PropertyChangeInfo
		{
			PropertyChangeInfo(Object* pObject)
				: m_pObject(pObject)
			{
			}

			Object* m_pObject;
			std::vector<IProperty*> m_Properties;

			//! clears the properties
			void Clear()
			{
				for (size_t i=0; i<m_Properties.size(); ++i)				
					sdelete(m_Properties[i]);
				
				m_Properties.clear();
			}
		};

		std::vector<PropertyChangeInfo> m_UndoStack;
		std::vector<PropertyChangeInfo> m_RedoStack;

		//! record property change
		void RecordPropertyChange(const PropertyChangeInfo& change);

		//! performs a property change
		IProperty* DoPropertyChange(Object* pObject, IProperty* pProperty);		

		//! clears entire history in a stack
		void ClearHistory(std::vector<PropertyChangeInfo>& stack);

		//! clears object history from a stack
		void ClearHistory(Object* pObject, std::vector<PropertyChangeInfo>& stack);
	};
}




