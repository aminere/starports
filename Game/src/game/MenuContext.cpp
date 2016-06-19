/*

Amine Rehioui
Created: May 9th 2015

*/

#include "Game.h"

#include "MenuContext.h"
#include "EventManager.h"

namespace shoot
{
	DEFINE_OBJECT(MenuContext);

	//! constructor
	MenuContext::MenuContext()
		: m_eState(S_Idle)
		, m_bTransitionIn(true)
		, m_bPopRequested(false)
		, m_bTransitionInitialized(false)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void MenuContext::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
	}

	//! called during the initialization of the entity
	void MenuContext::Init()
	{
		super::Init();

		InitTransition();
	}

	//! called during the update of the entity
	void MenuContext::Update()
	{
		if(!m_bTransitionInitialized)		
			InitTransition();		

		switch(m_eState)
		{
		case S_Init:
		{
			m_Timer = 0.2f;
			m_eState = S_Transition;

			if (m_bTransitionIn)
			{
				OnEnter();

				if (FadeRenderer::Instance()->GetDestColor() != Color::Zero)
					FadeRenderer::Instance()->StartFade(Color::Black, Color::Zero, m_Timer, false, false);
				else
					m_Timer = -1.0f;
			}
			else
			{
				OnExit();

				if (FadeRenderer::Instance()->GetDestColor() != Color::Black)
					FadeRenderer::Instance()->StartFade(Color::Zero, Color::Black, m_Timer, false, true);
				else
					m_Timer = -1.0f;
			}
		}			
			break;

		case S_Transition:
			if (m_Timer > 0.0f)
			{
				m_Timer -= g_fDeltaTime;
			}
			else
			{
				m_eState = S_Idle;
				if(!m_ContextToSwitchTo.empty())
				{
					auto nextContext = m_ContextToSwitchTo;
					m_ContextToSwitchTo.clear();
					m_bTransitionInitialized = false;
					super::Switch(nextContext.c_str());
					return;
				}
				else if (m_bPopRequested)
				{
					super::Pop();

					UIEvent* pEvent = snew UIEvent();
					pEvent->m_Command = "POPPED";
					EventManager::Instance()->SendEvent(pEvent);
				}
			}			
			break;

		default:
			super::Update();
		}
	}

	//! switches to a new context
	void MenuContext::Switch(const char* strPath)
	{
		m_ContextToSwitchTo = strPath;
		if (m_eState == S_Idle)
		{
			m_bTransitionIn = false;
			m_eState = S_Init;
		}
	}

	//! pops the context
	void MenuContext::Pop()
	{
		if(m_eState == S_Idle)
		{
			m_bPopRequested = true;
			m_bTransitionIn = false;
			m_eState = S_Init;
		}
	}

	//! initializes the transition
	void MenuContext::InitTransition()
	{
		m_bTransitionIn = true;
		m_eState = S_Init;
		m_bTransitionInitialized = true;
	}	
}

