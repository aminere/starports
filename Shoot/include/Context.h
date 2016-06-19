/* 

Amine Rehioui
Created: February 23th 2010

*/

#pragma once


#include "QuadTree.h"
#include "CollisionManager.h"
#include "ContextStack.h"

namespace shoot
{
	// forwards
	class Thread;

	//! A context is a representation of a game area
	/** Contexts allow smooth transitions between game areas and can be superposed by the engine 
		into a stack where multiple contexts are managed at the same time */
	class Context : public Entity
	{
	public:		
	
		//! Macro to provide type information
		DECLARE_OBJECT(Context, Entity);

		//! constructor
		Context();

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		virtual void Init();

		//! called during the update of the entity
		virtual void Update();

		//! registers the entity and its children for rendering
		virtual void RegisterForRendering();

		//! returns the QuadTree
		inline QuadTree* GetQuadTree() const { return m_QuadTree; }

		//! returns the collision manager
		inline CollisionManager* GetCollisionManager() const { return m_CollisionManager; }

		//! recursively update children
		void Update(Entity* pEntity);

		//! recursivly register entities for rendering
		void RegisterForRendering(Entity* pEntity);

		//! switches to a new context
		virtual void Switch(const char* strPath);

		//! pops the context
		virtual void Pop();

		//! returns true if the context is on top of the context stack
		bool IsTopContext() const;

		//! returns blocking status
		inline bool CanBlockUpdate() const { return m_bBlockUpdate; }

	protected:

		Reference<CollisionManager> m_CollisionManager;

		// properties
		Reference<QuadTree> m_QuadTree;
		bool m_bBlockUpdate;
		bool m_ReleaseOnSwitch;
	};
}




