/* 

Amine Rehioui
Created: Feb. 16th 2015

*/

#pragma once


namespace shoot
{
	//! RangeIndicator
	class RangeIndicator : public Entity3D
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(RangeIndicator, Entity3D);

		//! constructor
		RangeIndicator();

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream) override;

		//! called during the initialization of the entity
		virtual void Init() override;

		//! called during the update of the entity
		virtual void Update() override;

	protected:

		// properties
		int m_NumVertices;
	};
}



