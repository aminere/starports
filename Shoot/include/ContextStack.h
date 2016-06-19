/* 

Amine Rehioui
Created: April 11th 2010

*/

#pragma once


namespace shoot
{
	//! The context stack is the root of all contexts currently managed by the engine
	class ContextStack : public Entity
	{
	public:

		DECLARE_OBJECT(ContextStack, Entity);

		//! Constructor
		ContextStack();

		//! Destructor
		virtual ~ContextStack();

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		virtual void Init();

		//! called during the update of the entity
		virtual void Update();

		//! registers the entity and its children for rendering
		virtual void RegisterForRendering();

		//! adds a child to the entity
		virtual void AddChild(Entity* pChild, bool bImmediate = false);
	};
}



