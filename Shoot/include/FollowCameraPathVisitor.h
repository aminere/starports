/* 

Amine Rehioui
Created: August 14th 2011

*/

#pragma once


#include "AnimationVisitor.h"

#include "CameraPath.h"

namespace shoot
{
	// forwards 
	class Camera;

	//! FollowPathVisitor class
	class FollowCameraPathVisitor : public AnimationVisitor
	{
	public:

		DECLARE_OBJECT(FollowCameraPathVisitor, AnimationVisitor);

		//! constructor
		FollowCameraPathVisitor();

		//! Reads/Writes struct properties from/to a stream
		virtual void Serialize(PropertyStream& stream);		

		//! visits a particular entity
		virtual void Visit(Entity* pTarget);

		//! updates the visitor
		virtual void Update();

	protected:

		Camera* m_pCamera;
		float m_fTimer;
		float m_fDuration;
		int m_SrcElement;
		int m_DestElement;

		// properties
		Link<CameraPath> m_Path;
	};
}



