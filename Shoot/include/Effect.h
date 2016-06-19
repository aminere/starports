/* 

Amine Rehioui
Created: September 6th 2014

*/

#pragma once


namespace shoot
{
	//! Effect
	class Effect : public Object
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(Effect, Object);

		//! Constructor
		Effect()
		{
		}

		//! Destructor
		virtual ~Effect()
		{
		}

		//! Initializes the effect
		virtual void Init() { }

		//! Renders the effect
		virtual void Render() { }

		//! Update
		virtual void Update() { }
	};
}



