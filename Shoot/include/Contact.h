/* 

Amine Rehioui
Created: January 2nd 2011

*/

#pragma once


namespace shoot
{
	// forwards
	class RigidBody;

	//! Used to resolve contacts between Collidables
	class Contact
	{
	public:

		//! constructor
		Contact(RigidBody* pRigidBody1, RigidBody* pRigidBody2, const Vector3& vNormal);

		//! resolves the collision
		void Resolve();

	private:

		RigidBody* m_pRigidBody1;
		RigidBody* m_pRigidBody2;
		Vector3 m_vContactNormal;
	};
}



