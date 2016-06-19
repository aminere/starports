/* 

Amine Rehioui
Created: January 14th 2012

*/

#pragma once


namespace shoot
{
	// forwards
	class Entity;
	class Context;

	//! Base Component class
	class Component : public Object
	{
		DECLARE_OBJECT(Component, Object);

	public:		

		//! constructor
		Component();

		//! destructor
		virtual ~Component();

		//! Initializes the component
		virtual void Init() { }

		//! Updates the component
		virtual void Update() { }

		//! sets the owner entity
		inline void SetOwner(Entity* pOwner) { m_pOwner = pOwner; }

		//! returns the owner entity
		inline Entity* GetOwner() const { return m_pOwner; }

		//! returns the root context
		Context* GetRootContext() const;

	protected:

		Entity* m_pOwner;
	};
}



