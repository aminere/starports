/* 

Amine Rehioui
Created: October 11th 2011

*/

#pragma once


#include "GameData.h"

namespace shoot
{
	// forwards
	class Text;
	class Event;

	//! GameManager
	class GameManager : public Entity
	{
		DECLARE_OBJECT(GameManager, Entity);
		friend class UIHandler;

	public:

		//! game state
		enum E_State
		{
			S_Menu,
			S_InGame,
			S_InGamePause,
			S_AskExitLevel,
			S_AskExitGame,
			S_MissionFailed,
			S_MissionSuceeded,
			S_Replay,
			S_IAPInit,
			S_IAP,
			S_IAPBuy,
			S_IAPRestore,
			S_MenuOptions,
			S_ThankYou
		};

		//! Context
		enum E_Context
		{
			C_TouchToStart,
			C_MainMenu,
			C_Tutorial,
			C_Level1
		};

		//! ContextInfo
		struct ContextInfo
		{
			E_Context m_Type;
			std::string m_ContextStack;
			E_State m_StartingState;
			std::vector<std::string> m_Partitions;
		};

		//! constructor
		GameManager();

		//! destructor
		virtual ~GameManager();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		void Init();

		//! called during the update of the entity
		void Update();

		//! initializes a UIContext
		void InitUIContext(Entity* pContext);

		//! pauses the game
		void Pause();

		//! returns the game data
		inline GameData* GetGameData() const { return m_GameData; }

		//! returns the game state
		inline E_State GetState() const { return m_eState; }

		//! returns the UI transition duration
		inline float GetUITransitionDuration() const { return m_fUITransitionDuration; }

		//! returns the UI transition origin from top
		inline float GetUITransitionOriginTop() const { return m_fUITransitionOriginTop; }

		//! returns the UI transition origin from bottom
		inline float GetUITransitionOriginBottom() const { return m_fUITransitionOriginBottom; }

		//! GetContext
		inline float GetContextTransitionDuration() const { return m_ContextTransitionDuration; }

		//! restarts the level
		void Restart();

		//! OnFailed
		void OnFailed();

		//! OnSucceeded
		void OnSucceeded();

		//! SwitchContext
		void SwitchContext(E_Context type);

		//! SwitchContext
		void SwitchContext(const std::string& contextStack);

		//! SwitchLevel
		void SwitchLevel(const std::string& contextStack, int level);

		//! ResetMenuTimer
		inline void ResetMenuTimer() 
		{ 
			m_MenuTimer = 0.0f; 
			m_MenuTimeOut = m_MenuTimeOutBig;
		}
		
		//! IsThankYouPending
		inline bool IsThankYouPending() const { return m_ThankYouPending; }

		//! ResetThankYouPending
		inline void ResetThankYouPending() { m_ThankYouPending = false; }

		//! ResetGameData
		void ResetGameData();

		//! sets the current state
		void SetState(E_State eNewState);

		//! IAP
		void OnProductsRetrieved(bool success);
		void OnTransactionCompleted(bool success);
		void OnSignInCancelled();
		void OnNothingToRestore();
		void OnIAPInit();
		bool m_HasBuyFPSModeRequest;

		//! event handlers
		void HandleUIEvent(Event* pEvent);
		void HandleUITriggerEvent(Event* pEvent);

	private:

		E_State m_eState;
		Reference<GameData> m_GameData;
		std::string m_OnPopUIEvent;
		std::vector<ContextInfo> m_ContextInfos;
		ContextInfo* m_CurrentContext;
		float m_MenuTimer;
		float m_MenuTimeOut;
		float m_MenuSuccessTimer;
		bool m_MenuSuccessTouched;
		
		bool m_ThankYouPending;

		//! Navigation type
		enum E_NavigationType
		{
			NT_Switch,
			NT_Push
		};

		//! Navigation info
		struct NavigationInfo
		{
			E_NavigationType m_eType;
			std::string m_Context;
		};

		std::list<NavigationInfo> m_NavigationList;

		//! resumes the game
		void Resume();

		//! exits the current level
		void ExitLevel();

		//! SetupContexts
		void SetupContexts();

		//! DiscardPartitions
		void DiscardPartitions(E_Context nextContext);

		// IAP
		bool m_ProductsRetrieved;

		// properties		
		float m_fUITransitionDuration;
		float m_fUITransitionOriginTop;
		float m_fUITransitionOriginBottom;
		float m_ContextTransitionDuration;
		float m_MenuTimeOutBig;
		float m_MenuTimeOutSmall;
	};
}



