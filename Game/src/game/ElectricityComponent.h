/* 

Amine Rehioui
Created: April 16th 2014

*/

#pragma once


#include "GraphicComponent.h"

namespace shoot
{
	//! ElectricityComponent class
	class ElectricityComponent : public GraphicComponent
	{
		DECLARE_OBJECT(ElectricityComponent, GraphicComponent);

	public:	

		//! constructor
		ElectricityComponent();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! Initializes the component
		void Init();

		//! Updates the component
		void Update();

		//! Activates the electricity
		void Activate(bool bActive);

	private:
				
		std::vector<float> m_ChargeTimers;
		bool m_bActive;

		//! Initializes the coils
		void InitCoils();

		//! Initializes the beam
		void InitBeam();

		//! generates a coil
		void GenerateCoil(Vertex3D* pVertices, int& currentVertex, int maxVertices);

		// properties
		int m_NumSegments;
		int m_CoilCount;
		Range m_rAmplitude;
		Range m_ChargeDuration;
		float m_fBeamThickness;
	};
}



