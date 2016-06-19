/* 

Amine Rehioui
Created: September 1st 2014

*/

#pragma once


#include "GraphicComponent.h"

namespace shoot
{
	//! LaserComponent
	class LaserComponent : public GraphicComponent
	{
		DECLARE_OBJECT(LaserComponent, GraphicComponent);

	public:

		//! constructor
		LaserComponent();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! Initializes the component
		void Init();

		//! Updates the component
		void Update();

		//! activates the laser
		void Activate(const Vector3& vTarget);

		//! Deactivate
		void Deactivate();

		//! returns the length
		inline float GetLength() const { return m_fLength; }

		//! SetMaxLength
		inline void SetMaxLength(float maxLength) { m_fMaxLength = maxLength; }

	private:

		Handle<Entity3D> m_StartPoint;
		bool m_bActive;
		Vector3 m_vTarget;		
		float m_fLength;
		float m_fMaxLength;
		float m_fOcillationTimer;
		float m_fThickness;

		// properties
		float m_fDeploySpeed;
		float m_fFrequency;
		Range m_ThicknessRange;
	};
}



