/* 

Amine Rehioui
Created: September 7th 2013

*/

#pragma once


namespace shoot
{
	//! UIContext
	class UIContext : public Context
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
		DECLARE_OBJECT(UIContext, Context);

		//! constructor
		UIContext();

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		virtual void Init();

		//! called during the update of the entity
		virtual void Update();

		//! switches to a new context
		virtual void Switch(const char* strPath);

		//! pops the context
		virtual void Pop();

		//! returns the transition line position
		inline float GetTransitionLinePos() const { return m_fTransitionLinePos; }

		//! returns the state
		inline E_State GetState() const { return m_eState; }

		//! returns the touch area
		inline const AABBox2D& GetTouchArea() const { return m_TouchArea; }

		//! SetOnTouchEvent
		inline void SetOnTouchEvent(const std::string& event) { m_OnTouchEvent = event; }

	protected:

		//! initializes the transition
		void InitTransition();

		//! updates the transition
		void UpdateTransition();

		//! switch context thread
		static void* SwitchContextThread(void* pParam);

		Handle<Entity2D> m_TopElements;
		Handle<Entity2D> m_BottomElements;
		Handle<Entity> m_BG;

		float m_fInterpolator;
		float m_fTopSrcPos;
		float m_fTopDestPos;
		float m_fBottomSrcPos;
		float m_fBottomDestPos;
		float m_fBGSrcAlpha;
		float m_fBGDestAlpha;		

		E_State m_eState;
		bool m_bTransitionIn;
		std::string m_ContextToSwitchTo;
		bool m_bPopRequested;
		bool m_bSkipBGAnim;
		bool m_bTransitionInitialized;

		// properties
		float m_fTransitionLinePos;
		std::string m_OnTouchEvent;
		AABBox2D m_TouchArea;
	};
}




