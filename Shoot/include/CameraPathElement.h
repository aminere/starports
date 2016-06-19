/* 

Amine Rehioui
Created: August 14th 2011

*/

#pragma once


#include "PathElement.h"

namespace shoot
{
	//! Camera Path Element entity	
	class CameraPathElement : public PathElement				    			   
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(CameraPathElement, PathElement);

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! returns the camera look at point associated with this element
		const Vector3& GetCameraLookAt() const { return m_vCameraLookAt; }

		//! sets the camera look at point associated with this element
		void SetCameraLookAt(const Vector3& vLookAt) { m_vCameraLookAt = vLookAt; }

	private:

		//! properties
		Vector3 m_vCameraLookAt;
	};
}



