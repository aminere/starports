/* 

Amine Rehioui
Created: July 28th 2011

*/

#pragma once


#include "PathElement.h"

namespace shoot
{
	//! Path entity
	class Path : public Entity3D				 
	{
	public:

		//! interpolation type
		enum E_Interpolation
		{
			I_Hermite,
			I_CatmullRom,
			I_Cubic
		};

		//! Macro to provide type information
		DECLARE_OBJECT(Path, Entity3D);

		//! Constructor
		Path();

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		virtual void Init();

		//! adds a child to the entity
		virtual void AddChild(Entity* pChild, bool bImmediate = false);

		//! returns a position along the path
		/** \param fRatio: factor from 0.0f to 1.0f */
		Vector3 GetPathPosition(float fRatio, int* pSrcElementIndex = NULL);

		//! returns a position along the path
		Vector3 GetPathPosition(int srcIndex, int destIndex, float fLocalRatio);

		//! returns a path element
		inline PathElement* GetElement(int index) const { return static_cast<PathElement*>(GetChild(index)); }

		//! returns wrap around status
		bool IsWrapAround() const { return m_bWrapAround; }
			
	protected:

		typedef Vector3 (Interpolator)(Vector3, Vector3, Vector3, Vector3, float, float);		
		Interpolator* m_pInterpolator;

		// properties
		bool m_bWrapAround;
		E_Interpolation m_eInterpolation;
		float m_fTightness;
	};
}



