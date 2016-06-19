/* 

Amine Rehioui
Created: February 5th 2010

*/

#pragma once


namespace shoot
{
	// forwards
	template <class Actor> class FSM;

	//! Base Finite State Machine State class
	template <class Actor> 
	class FSMState
	{
	public:

		//! Constructor	
		FSMState() : m_pFSM(0), m_pActor(0)
		{
		}

		//! Destructor
		virtual ~FSMState() 
		{
		}

		//! State Begin function
		virtual void Begin(int iPreviousState) { }

		//! State Update function
		virtual void Update() { }

		//! State End function
		virtual void End(int iNextState) { } 

		//! Initializes the state with core pointers - internal FSM use do not use/modify
		void Init(FSM<Actor>* pFSM, Actor* pActor)
		{
			m_pFSM = pFSM; 
			m_pActor = pActor;
		}

	protected:

		FSM<Actor>* m_pFSM;
		Actor* m_pActor;
	};
}



