/* 

Amine Rehioui
Created: April 12th 2014

*/

#pragma once


#include "Actor.h"

namespace shoot
{
	class ProgressBar;

	//! Tower
	class Tower : public Actor
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(Tower, Actor);

		//! constructor
		Tower();

		//! destructor
		virtual ~Tower();
				
		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		virtual void Init();

		//! called during the update of the entity
		void Update();

	protected:

		//! checks for a target
		bool CheckTarget();

		//! picks a target
		void PickTarget();

		Handle<Actor> m_Target;
	};	
}




