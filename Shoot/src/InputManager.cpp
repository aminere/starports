/* 

Amine Rehioui
Created: December 18th 2010

*/

#include "Shoot.h"

#include "InputManager.h"

#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32
#include "InputManagerSDL.h"
#else
#include "InputManagerTouch.h"
#endif

namespace shoot
{
	// static variable initialization
	InputManager* InputManager::m_spInstance = NULL;

	 //! constructor
	InputManager::InputManager()
		: m_pVKeyboardCallback(NULL)
		, m_bVKeyboardRequested(false)
		, m_bVKClearFlag(false)
	{
	}

	//! creates the input manager
	void InputManager::CreateInstance(E_Type eType)
	{
		SHOOT_ASSERT(!m_spInstance, "Cannot create multiple InputManager instances");

		switch(eType)
		{
#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32
		case IMT_SDL: 
			m_spInstance = snew InputManagerSDL();
			break;
#else
		case IMT_Touch:
			m_spInstance = snew InputManagerTouch();
			break;		
#endif

		case IMT_NULL:
			m_spInstance = snew NullInputManager();
			break;

		default:
			SHOOT_ASSERT(false, "InputManager not supported on this platform");	
		}		
	}

	//! destroys the driver
	void InputManager::DestroyInstance()
	{
		SHOOT_ASSERT(m_spInstance, "Cannot destroy NULL InputManager instance");
		delete m_spInstance;
	}

	//! Update the input state
	void InputManager::Update()
	{
		if(!m_aAsyncInputStates.empty())
		{
			for (size_t i=0; i<m_aAsyncInputStates.size(); ++i)
			{
				SetKeyPressed(m_aAsyncInputStates[i].key, m_aAsyncInputStates[i].bPressed);
			}
			m_aAsyncInputStates.clear();
		}

		if(!m_aAsyncTouchStates.empty())
		{
			SetTouchState(m_aAsyncTouchStates[0]);
			m_aAsyncTouchStates.clear();
		}

		// update focus holder
		if (m_FocusHolder.IsValid())
		{
			auto holder = m_FocusHolder.Get();
			m_FocusHolder.Clear();
			if (IsTouchPressed() || IsTouchJustReleased())
				m_FocusHolder = holder;
		}	
	}

	//! force key status - async
	void InputManager::SetKeyPressedAsync(E_KeyType eKey, bool bSet)
	{
		if(eKey != KT_None)
		{
			InputState state = { eKey, bSet };
			m_aAsyncInputStates.push_back(state);
		}	
	}

	//! sets the touch state - async
	void InputManager::SetTouchStateAsync(const TouchState& state)
	{
		if(m_aAsyncTouchStates.empty())
		{
			m_aAsyncTouchStates.push_back(state);
		}
		else
		{
			m_aAsyncTouchStates[0] = state;
		}		
	}

	//! ResetTouchState
	void InputManager::ResetTouchState()
	{
		TouchState state;
		state.bPressed = false;
		SetTouchState(state);
		m_aAsyncTouchStates.clear();
		m_FocusHolder.Clear();
	}

	//! returns true if the virtual keyboard is available
	bool InputManager::IsVKeyboardAvailable() const
	{
#if SHOOT_PLATFORM == SHOOT_PLATFORM_IOS
		return true;
#else
		return false;
#endif
	}
    
    //! Requests a virtual keyboard
    void InputManager::RequestVKeyboard(VKeyboardCallback* pCallback)
    {
        m_bVKeyboardRequested = true;
        m_pVKeyboardCallback = pCallback;
		m_VKeyboardText.clear();
    }
    
	//! closes the virtual keyboard
	void InputManager::CloseVKeyboard()
	{
		m_bVKeyboardRequested = false;
		m_pVKeyboardCallback = NULL;
	}

    //! called when the virtual keyboard has been updated
    void InputManager::OnVKeyboardUpdated(std::string text, bool bFinished)
    {
		if(!bFinished)
		{
			m_VKeyboardText = text;
		}
		else
		{
			m_bVKClearFlag = true;
		}

        bool bCloseKeyboard = m_pVKeyboardCallback(bFinished ? m_VKeyboardText : text, bFinished);

        if(bCloseKeyboard)
        {
           CloseVKeyboard();
        }
    }
}

