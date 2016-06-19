/* 

Amine Rehioui
Created: April 2nd 2011

*/

#pragma once


namespace shoot
{
	// forwards
	class Entity;	

	//! Base Visitor class
	class Visitor : public Component
	{
	public:

		DECLARE_OBJECT(Visitor, Component);
		
		//! constructor
		Visitor();

		//! destructor
		virtual ~Visitor();		

		//! Reads/Writes struct properties from/to a stream
		virtual void Serialize(PropertyStream& stream);	

		//! Initializes the component
		virtual void Init();

		//! visits a particular entity
		virtual void Visit(Entity* pTarget);

		//! Leaves the visited entity
		virtual void Leave();

		//! returns true if the visitor is marked for auto start
		bool GetAutoStart() { return m_bAutoStart; }

		//! returns true if the visitor is active
		bool IsActive() const { return m_bActive; }

	protected:

		Entity* m_pTarget;
		bool m_bActive;

		// properties
		bool m_bAutoStart;
	};
}



