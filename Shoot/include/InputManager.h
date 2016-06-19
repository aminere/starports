/* 

Amine Rehioui
Created: December 18th 2010

*/

#pragma once


#include "Vector2.h"

namespace shoot
{
	//! Interface to input devices
	class InputManager 
	{
	public:

		//! input manager types
		enum E_Type
		{
			IMT_SDL,
			IMT_Touch,
			IMT_NULL,
			IMT_Count
		};

		//! keys types
		enum E_KeyType
		{
			KT_None,
			KT_Left,
			KT_Right,
			KT_Up,
			KT_Down,
			KT_L,
			KT_R,
			KT_A,
			KT_B,
			KT_X,
			KT_Y,
			KT_W,
			KT_S,
			KT_D,
			KT_Q,
			KT_E,
			KT_Exit,
			KT_Back,
			KT_Count
		};

        // virtual keyboard result callback
        typedef bool VKeyboardCallback(std::string text, bool bFinished);
        
		//! touch state
		struct TouchState
		{
			TouchState()
				: bPressed(false)
				, vPosition(Vector2::Zero)
			{
			}

			bool bPressed;
			Vector2 vPosition;
		};

        //! constructor
        InputManager();

		//! destructor
		virtual ~InputManager()
		{
		}

		//! creates the input manager
		static void CreateInstance(E_Type eType);

		//! destroys the driver
		static void DestroyInstance();

		//! returns the instance of the graphics driver
		inline static InputManager* Instance() { return m_spInstance; }
        
		//! Update the input state
		virtual void Update();

		//! returns true only during the frame in which the key moved from unpressed to pressed state
		virtual bool IsKeyJustPressed(E_KeyType eKey) const = 0;

		//! returns true for as long as the key is pressed
		virtual bool IsKeyPressed(E_KeyType eKey) const = 0;

		//! returns true only during the frame in which the key moved from pressed to unpressed state
		virtual bool IsKeyJustReleased(E_KeyType eKey) const = 0;

		//! force key status
		virtual void SetKeyPressed(E_KeyType eKey, bool bSet) = 0;

		//! returns true only during the frame in which the touch moved from unpressed to pressed state
		virtual bool IsTouchJustPressed(Object* requester = NULL) const = 0;

		//! returns true for as long as the touch is pressed
		virtual bool IsTouchPressed(Object* requester = NULL) const = 0;

		//! returns true only during the frame in which the touch moved from pressed to unpressed state
		virtual bool IsTouchJustReleased(Object* requester = NULL) const = 0;		

		//! returns the touch state
		virtual const TouchState& GetTouchState() const = 0;

		//! returns the raw touch state - does not take 2D camera transformation into account
		virtual const TouchState& GetRawTouchState() const = 0;
	
		//! sets the touch state
		virtual void SetTouchState(const TouchState& state) = 0;

		//! sets the touch state - async
		void SetTouchStateAsync(const TouchState& state);

		//! ResetTouchState
		void ResetTouchState();

		//! force key status - async
		void SetKeyPressedAsync(E_KeyType eKey, bool bSet);
                
		//! returns true if the virtual keyboard is available
		bool IsVKeyboardAvailable() const;

        //! Requests a virtual keyboard
        void RequestVKeyboard(VKeyboardCallback* pCallback);

		//! closes the virtual keyboard
		void CloseVKeyboard();
        
        //! called when the virtual keyboard has been updated
        void OnVKeyboardUpdated(std::string text, bool bFinished);
        
        //! gets VKeyboard request status
        inline bool IsVKeyboardRequested() const { return m_bVKeyboardRequested; }

		//! gets the VKeyboard clear flag
		inline bool HasVKClearFlag() const { return m_bVKClearFlag; }

		//! resets the VKeyboard clear flag
		inline void ResetVKClearFlag() { m_bVKClearFlag = false; }

		//! RequestTouchFocus
		inline void RequestTouchFocus(Object* requester) { m_FocusHolder = requester; }

	private:

		static InputManager* m_spInstance;

		//! input state
		struct InputState
		{
			E_KeyType key;
			bool bPressed;
		};
		
		std::vector<InputState> m_aAsyncInputStates;
		std::vector<TouchState> m_aAsyncTouchStates;
        
        VKeyboardCallback* m_pVKeyboardCallback;
		std::string m_VKeyboardText;
        bool m_bVKeyboardRequested;
		bool m_bVKClearFlag;

	protected:

		Handle<Object> m_FocusHolder;
	};

	//! null InputManager interface
	class NullInputManager : public InputManager
	{
	public:
		
		//! returns true only during the frame in which the key moved from unpressed to pressed state
		bool IsKeyJustPressed(E_KeyType eKey) const { return false; }

		//! returns true for as long as the key is pressed
		bool IsKeyPressed(E_KeyType eKey) const { return false; }

		//! returns true only during the frame in which the key moved from pressed to unpressed state
		bool IsKeyJustReleased(E_KeyType eKey) const { return false; }

		//! force key status
		void SetKeyPressed(E_KeyType eKey, bool bSet) { }

		//! returns true only during the frame in which the touch moved from unpressed to pressed state
		bool IsTouchJustPressed(Object* requester = NULL) const override { return false; }

		//! returns true for as long as the touch is pressed
		bool IsTouchPressed(Object* requester = NULL) const override { return false; }

		//! returns true only during the frame in which the touch moved from pressed to unpressed state
		bool IsTouchJustReleased(Object* requester = NULL) const override { return false; }

		//! returns the touch state
		const TouchState& GetTouchState() const { return m_DummyTouchState; }

		//! returns the raw touch state - does not take 2D camera transformation into account
		const TouchState& GetRawTouchState() const { return m_DummyTouchState; }
	
		//! sets the touch state
		void SetTouchState(const TouchState& state) { }

	private:

		TouchState m_DummyTouchState;
	};
}


