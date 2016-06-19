/* 

Amine Rehioui
Created: August 14th 2011

*/

#pragma once


#include "Path.h"
#include "CameraPathElement.h"

namespace shoot
{
	//! Camera Path entity
	class CameraPath : public Path				 
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(CameraPath, Path);

		//! adds a child to the entity
		void AddChild(Entity* pChild, bool bImmediate = false);

		//! returns a camera look at position along the path
		/** \param fRatio: factor from 0.0f to 1.0f */
		Vector3 GetLookAt(float fRatio);

		//! returns a camera look at position along the path
		Vector3 GetLookAt(int srcIndex, int destIndex, float fLocalRatio);

		//! returns a path element
		inline CameraPathElement* GetElement(int index) const { return static_cast<CameraPathElement*>(GetChild(index)); }
	};
}



