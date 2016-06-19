/*

Amine Rehioui
Created: May 8th 2015

*/

#pragma once

namespace shoot
{
	class Defender;

	//! FPSContext
	class FPSContext : public Context
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(FPSContext, Context);

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream) override;

		//! called during the initialization of the entity
		void Init() override;

		//! called during the update of the entity
		void Update() override;

	private:

		Handle<Defender> m_Target;
		Vector3 m_PreviousTargetPos;
	};
}



