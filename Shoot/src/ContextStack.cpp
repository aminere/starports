/* 

Amine Rehioui
Created: April 11th 2010

*/

#include "Shoot.h"

#include "ContextStack.h"

namespace shoot
{
	DEFINE_OBJECT(ContextStack);

	//! Constructor
	ContextStack::ContextStack()
	{
	}

	//! Destructor
	ContextStack::~ContextStack()
	{
	}

	//! serializes the entity to/from a PropertyStream
	void ContextStack::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
	}

	//! called during the initialization of the entity
	void ContextStack::Init()
	{
		super::Init();
	}

	//! called during the update of the entity
	void ContextStack::Update()
	{
		for(int i=int(GetChildCount())-1; i >= 0; --i)
		{
			auto context = static_cast<Context*>(GetChild(i));
			if(!context->IsEnabled())
				continue;

			context->Update();

			if(context->CanBlockUpdate())
				break;
		}
	}

	//! registers the entity and its children for rendering
	void ContextStack::RegisterForRendering()
	{
		for(auto& child : GetChildren())
			child->RegisterForRendering();
	}

	//! adds a child to the entity
	void ContextStack::AddChild(Entity* pChild, bool bImmediate /*= false*/)
	{
		SHOOT_ASSERT(pChild->IsA(Context::TypeID), "ContextStack::AddChild: child is not of type Context");
		super::AddChild(pChild, bImmediate);
	}
}

