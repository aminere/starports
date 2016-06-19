/* 

Amine Rehioui
Created: May 9th 2015

*/

#pragma once

namespace shoot
{
	//! MenuContext
	class MenuContext : public Context
	{
	public:

		//! state
		enum E_State
		{
			S_Init,
			S_Transition,
			S_Idle
		};
	
		//! Macro to provide type information
		DECLARE_OBJECT(MenuContext, Context);

		//! constructor
		MenuContext();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream) override;

		//! called during the initialization of the entity
		void Init() override;

		//! called during the update of the entity
		void Update() override;

		//! switches to a new context
		void Switch(const char* strPath) override;

		//! pops the context
		void Pop() override;

		//! OnEnter
		virtual void OnEnter() { }

		//! OnExit
		virtual void OnExit() { }

	protected:

		//! initializes the transition
		void InitTransition();

		float m_Timer;

		E_State m_eState;
		bool m_bTransitionIn;
		std::string m_ContextToSwitchTo;
		bool m_bPopRequested;
		bool m_bTransitionInitialized;
	};
}


