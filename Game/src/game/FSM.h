/* 

Amine Rehioui
Created: February 5th 2010

*/

#pragma once


#include "FSMState.h"

#include <string.h> // memset

namespace shoot
{
	//! FSM base
	class FSMBase
	{
	public:

		//! Destructor
		virtual ~FSMBase()
		{
		}

		//! Initializes the state table
		virtual void Init() = 0;

		//! Update the FSM
		virtual void Update() = 0;

		//! Switches the current state, call the appropriate state begin / end functions
		/** \param iNextState index of the next state */
		virtual void SwitchState(int iNextState) = 0;

		//! returns the current state index
		virtual int GetState() const = 0;
	};

	//! Base Finite State Machine class
	template <class Actor> 
	class FSM : public FSMBase
	{
	public:

		//! Constructor
		FSM(Actor* pActor) : m_iCurrentState(-1), m_pActor(pActor)
		{
			for(int i=0; i<MaxStates; ++i)
			{
				m_aStates[i] = NULL;
			}
		}

		//! Destructor
		virtual ~FSM()
		{
			for(int i=0; i<MaxStates; ++i)
			{
				if(m_aStates[i] != NULL)
				{
					sdelete(m_aStates[i]);
				}
			}
		}

		//! Initializes the state table
		virtual void Init() { }

		//! Update the FSM
		void Update()
		{
			if(m_iCurrentState >= 0)
			{
				m_aStates[m_iCurrentState]->Update();
			}
		}

		//! Switches the current state, call the appropriate state begin / end functions
		/** \param iNextState index of the next state */
		void SwitchState(int iNextState)
		{
			if(iNextState != m_iCurrentState)
			{
				if(m_iCurrentState >= 0)
				{
					m_aStates[m_iCurrentState]->End(iNextState);
				}

				m_aStates[iNextState]->Begin(m_iCurrentState);

				m_iCurrentState = iNextState;
			}
		}

		//! returns the current state index
		int GetState() const { return m_iCurrentState; }

		//! Add a state to the state table
		/** \param pState pointer to the state
		\param iIndex target index of the state in the table */
		void AddState(FSMState<Actor>* pState, int index)
		{
			SHOOT_ASSERT(index < MaxStates, "MaxStates reached please increase");
			SHOOT_ASSERT(m_aStates[index] == NULL, "Adding multiple state to the same location");
			m_aStates[index] = pState;
			m_aStates[index]->Init(this, m_pActor);
		}

		//! Max states constant
		static const int MaxStates = 64;

	protected:

		FSMState<Actor>* m_aStates[MaxStates];
		int m_iCurrentState;

		Actor* m_pActor;
	};
}



