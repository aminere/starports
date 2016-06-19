/* 

Amine Rehioui
Created: May 31st 2014

*/

#pragma once


namespace shoot
{
	//! UIMarker
	class UIMarker : public Entity2D
	{
		DECLARE_OBJECT(UIMarker, Entity2D);

	public:		

		//! constructor
		UIMarker();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		void Init();

		//! called during the update of the entity
		void Update();

		//! sets the target
		inline void SetTarget(Entity3D* pTarget) { m_Target = pTarget; }

		//! returns the target
		inline Entity3D* GetTarget() const { return m_Target; }

	private:

		Handle<Entity3D> m_Target;

		// properties
		Vector2 m_vRadiusFactors;
	};
}



