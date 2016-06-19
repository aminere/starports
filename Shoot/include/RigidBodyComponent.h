/* 

Amine Rehioui
Created: January 14th 2012

*/

#pragma once


#include "CollisionComponent.h"

namespace shoot
{
	//! RigidBody Component class
	class RigidBodyComponent : public CollisionComponent
	{
		DECLARE_OBJECT(RigidBodyComponent, CollisionComponent);

	public:

		//! constructor
		RigidBodyComponent();

		//! destructor
		virtual ~RigidBodyComponent();

		//! Reads/Writes struct properties from/to a stream
		virtual void Serialize(PropertyStream& stream);

		//! Initializes the component
		virtual void Init();

		//! checks collision with another component
		virtual void CheckCollisions(CollisionComponent* pOther);

		//! returns the collision shapes
		inline const Array< Reference<CollisionShape> >& GetShapes() const { return m_aShapes; }

		//! sets the velocity
		inline void SetVelocity(const Vector3& vVelocity) { m_vVelocity = vVelocity; }

		//! returns the velocity
		inline const Vector3& GetVelocity() const { return m_vVelocity; }

	private:
		
		Vector3 m_vVelocity;

		// properties
		float m_fMass;
		Array< Reference<CollisionShape> > m_aShapes;
	};
}



