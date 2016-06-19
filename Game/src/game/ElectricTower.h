/* 

Amine Rehioui
Created: April 26th 2014

*/

#pragma once


#include "Tower.h"

namespace shoot
{
	//! ElectricTower
	class ElectricTower : public Tower
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(ElectricTower, Tower);

		//! constructor
		ElectricTower();
				
		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		void Init();

		//! called during the update of the entity
		void Update();

	private:

		//! picks a target
		Actor* PickElectricTarget();

		//! IsAlreadyTargeted
		bool IsAlreadyTargeted(Actor*) const;

		Handle<Entity3D> m_Coils;
		Handle<Entity> m_GroundFlash;
		Handle<Entity> m_BeamHalo;
		std::vector< Handle<Entity> > m_TeslaOverlays;

		//! TargetInfo
		struct TargetInfo
		{
			Handle<Entity> m_Coil;
			Handle<Actor> m_Target;
		};

		std::vector<TargetInfo> m_TargetInfos;

		// properties
		Range m_DampingRange;
	};	
}




