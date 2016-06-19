/* 

Amine Rehioui
Created: September 7th 2013

*/

#include "Game.h"

#include "UIContext.h"

#include "GameManager.h"
#include "EventManager.h"

#include "Thread.h"

namespace shoot
{
	DEFINE_OBJECT(UIContext);

	//! constructor
	UIContext::UIContext()
		: m_eState(S_Idle)
		, m_bTransitionIn(true)
		, m_bPopRequested(false)
		, m_bSkipBGAnim(false)
		, m_bTransitionInitialized(false)
		// properties
		, m_fTransitionLinePos(400.0f)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void UIContext::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("TransitionLinePos", &m_fTransitionLinePos);
		stream.Serialize("OnTouchEvent", &m_OnTouchEvent);
		stream.Serialize("TouchArea", &m_TouchArea);
	}

	//! called during the initialization of the entity
	void UIContext::Init()
	{
		super::Init();

#ifndef SHOOT_EDITOR
		InitTransition();
#endif

		GAMEMGR->InitUIContext(this);
	}

	//! called during the update of the entity
	void UIContext::Update()
	{
		super::Update();

#ifdef SHOOT_EDITOR
		if(!m_bTransitionInitialized)
		{
			InitTransition();
		}
#endif

		switch(m_eState)
		{
		case S_Init:
			if(Camera* p2DCamera = EntityRenderer::Instance()->Get2DCamera())
			{
				const Size& resolution = p2DCamera->GetTargetResolution();		
				m_fTopSrcPos = GAMEMGR->GetUITransitionOriginTop();
				m_fTopDestPos = 0.0f;
				m_fBottomSrcPos = resolution.Height + GAMEMGR->GetUITransitionOriginBottom();
				m_fBottomDestPos = 0.0f;

				if(m_BG.IsValid())
				{
					m_fBGSrcAlpha = 0.0f;
					m_fBGDestAlpha = m_BG->GetComponent<GraphicComponent>()->GetMaterial()->GetColor().A;
				}

				if(!m_bTransitionIn)
				{
					Math::Swap(m_fTopSrcPos, m_fTopDestPos);
					Math::Swap(m_fBottomSrcPos, m_fBottomDestPos);
					Math::Swap(m_fBGSrcAlpha, m_fBGDestAlpha);
				}

				if(m_BG.IsValid())
				{
					m_BG->SetVisible(true);
				}
				m_TopElements->SetVisible(true);
				m_BottomElements->SetVisible(true);

				m_fInterpolator = 0.0f;
				UpdateTransition();
				m_eState = S_Transition;
			}
			break;

		case S_Transition:
			if(m_fInterpolator < 1.0f)
			{
				UpdateTransition();
				m_fInterpolator += g_fDeltaTime/GAMEMGR->GetUITransitionDuration();
			}
			else
			{
				m_fInterpolator = 1.0f;
				UpdateTransition();
				m_eState = S_Idle;

				if(!m_ContextToSwitchTo.empty())
				{
					auto nextContext = m_ContextToSwitchTo;
					m_ContextToSwitchTo.clear();
					m_bTransitionInitialized = false;
					super::Switch(nextContext.c_str());
					return;
				}
				else if(m_bPopRequested)
				{
					super::Pop();

					UIEvent* pEvent = snew UIEvent();
					pEvent->m_Command = "POPPED";
					EventManager::Instance()->SendEvent(pEvent);
				}
				else
				{
					m_bTransitionIn = !m_bTransitionIn;
				}
			}			
			break;

		case S_Idle:
			if(!m_OnTouchEvent.empty() && InputManager::Instance()->IsTouchJustReleased())
			{
				if(!m_TouchArea.Contains(InputManager::Instance()->GetTouchState().vPosition))
				{
					UIEvent* pEvent = snew UIEvent();
					pEvent->m_Sender = this;
					pEvent->m_Command = m_OnTouchEvent;
					EventManager::Instance()->SendEvent(pEvent);
				}
			}
			break;
		}
	}

	//! switches to a new context
	void UIContext::Switch(const char* strPath)
	{
		m_ContextToSwitchTo = strPath;
		if(m_eState == S_Idle)
		{
			m_eState = S_Init;
		}		
		m_bSkipBGAnim = m_BG.IsValid();
	}

	//! pops the context
	void UIContext::Pop()
	{
		m_bPopRequested = true;
		m_bSkipBGAnim = false;

		if (m_eState != S_Transition)
			m_eState = S_Init;		
	}

	//! initializes the transition
	void UIContext::InitTransition()
	{
		Entity* pPanel = GetChildByName("Panel");
		if(!pPanel)
		{
			return;
		}

		m_BG = pPanel->GetChildByName("BG", false);

		std::vector<Entity*> aTopElements;
		std::vector<Entity*> aBottomElements;
		for (size_t i = 0; i<pPanel->GetChildCount(); ++i)
		{
			if(pPanel->GetChild(i)->IsA(Entity2D::TypeID) 
				&& pPanel->GetChild(i) != m_BG.Get())
			{
				Entity2D* p2DEntity = static_cast<Entity2D*>(pPanel->GetChild(i));
				AABBox2D bbox = p2DEntity->GetBoundingBox() + p2DEntity->GetPosition();
				Vector2 vCenter = (bbox.Min() + bbox.Max())/2.0f;
				if(vCenter.Y < m_fTransitionLinePos)
				{
					aTopElements.push_back(p2DEntity);
				}
				else
				{
					aBottomElements.push_back(p2DEntity);
				}
			}
		}

		m_TopElements = snew Entity2D();
		m_TopElements->Init();
		pPanel->AddChild(m_TopElements, true);
		for (size_t i=0; i<aTopElements.size(); ++i)
		{
			m_TopElements->AddChild(aTopElements[i], true);
		}

		m_BottomElements = snew Entity2D();
		m_BottomElements->Init();
		pPanel->AddChild(m_BottomElements, true);
		for (size_t i=0; i<aBottomElements.size(); ++i)
		{
			m_BottomElements->AddChild(aBottomElements[i], true);
		}

		if(m_BG.IsValid() && !m_bSkipBGAnim)
		{
			m_BG->SetVisible(false);
		}

		m_TopElements->SetVisible(false);
		m_BottomElements->SetVisible(false);

		m_eState = S_Init;
		m_bTransitionIn = true;
		m_bTransitionInitialized = true;
	}

	//! updates the transition
	void UIContext::UpdateTransition()
	{
		float fTopPos = Math::Lerp(m_fTopSrcPos, m_fTopDestPos, m_fInterpolator);
		float fBottomPos = Math::Lerp(m_fBottomSrcPos, m_fBottomDestPos, m_fInterpolator);
		float fBGAlpha = Math::Lerp(m_fBGSrcAlpha, m_fBGDestAlpha, m_fInterpolator);

		if(m_TopElements.IsValid())
		{
			m_TopElements->SetPosition(Vector2::Create(0.0f, fTopPos));
		}

		if(m_BottomElements.IsValid())
		{
			m_BottomElements->SetPosition(Vector2::Create(0.0f, fBottomPos));
		}

		if(m_BG.IsValid() && !m_bSkipBGAnim)
		{
			Material* pMaterial = m_BG->GetComponent<GraphicComponent>()->GetMaterial();
			Color color = pMaterial->GetColor();
			pMaterial->SetColor(Color::Create(color.R, color.G, color.B, fBGAlpha));
		}
	}	
}

