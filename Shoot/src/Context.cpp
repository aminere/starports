/* 

Amine Rehioui
Created: February 28th 2010

*/

#include "Shoot.h"

#include "Context.h"

#include "Thread.h"
#include "MenuRootContext.h"

namespace shoot
{
	DEFINE_OBJECT(Context);

	//! constructor
	Context::Context()
		// properties
		: m_bBlockUpdate(false)
		, m_ReleaseOnSwitch(true)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void Context::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("QuadTree", &m_QuadTree);
		stream.Serialize("BlockUpdate", &m_bBlockUpdate);
		stream.Serialize("ReleaseOnSwitch", &m_ReleaseOnSwitch);
	}

	//! called during the initialization of the entity
	void Context::Init()
	{
		m_CollisionManager = snew CollisionManager();
		m_CollisionManager->SetContext(this);

		super::Init();

		if(m_QuadTree.IsValid())
			m_QuadTree->Build(this);
	}

	//! called during the update of the entity
	void Context::Update()
	{
		m_CollisionManager->Update();

		if(m_QuadTree.IsValid())
			m_QuadTree->Update();
		else
		{
			for(auto& child : GetChildren())
				Update(child);
		}
	}

	//! registers the entity and its children for rendering
	void Context::RegisterForRendering()
	{
		if(m_QuadTree.IsValid())
			m_QuadTree->RegisterForRendering();
		else
		{
			for(auto& child : GetChildren())
				RegisterForRendering(child.Get());
		}
	}
	
	//! recursively update children
	void Context::Update(Entity* pEntity)
	{
		if(!pEntity->IsEnabled())
			return;

		pEntity->Update();
		for(auto& component : pEntity->GetComponents())
			component->Update();

		for(auto& child : pEntity->GetChildren())
			Update(child.Get());
	}

	//! recursivly register entities for rendering
	void Context::RegisterForRendering(Entity* pEntity)
	{
		if(!pEntity->IsVisible() || !pEntity->IsInitialized())
			return;

		pEntity->RegisterForRendering();

		for(auto& child : pEntity->GetChildren())
			RegisterForRendering(child.Get());
	}

	//! switches to a new context
	void Context::Switch(const char* strPath)
	{
		auto newContext = static_cast<Context*>(ObjectManager::Instance()->FindOrLoad(strPath));
		int indexInParent = -1;
		auto& siblings = GetParent()->GetChildren();
		for (int i = 0; i < siblings.size(); ++i)
			if (siblings[i].Get() == this)
			{
				indexInParent = i;
				break;
			}
		SHOOT_ASSERT(indexInParent >= 0, "Context switch error");
		GetParent()->InsertChild(newContext, indexInParent, true);

		if (!m_ReleaseOnSwitch)
		{
			if (auto menuRoot = Engine::Instance()->GetContextStack()->GetChildByType<MenuRootContext>())
				menuRoot->AddReference(this);
		}

		GetParent()->RemoveChild(this, true);
		if (!newContext->IsInitialized())
			newContext->Init();
	}

	//! pops the context
	void Context::Pop()
	{
		GetParent()->RemoveChild(this, true);
	}

	//! returns true if the context is on top of the context stack
	bool Context::IsTopContext() const
	{
		auto topContextIndex = Engine::Instance()->GetNumContexts()-1;
		return this == Engine::Instance()->GetContext(topContextIndex);
	}	
}

