/* 

Amine Rehioui
Created: August 7th 2011

*/

#pragma once


#include "AnimationVisitor.h"

#include "Path.h"

namespace shoot
{
	// forwards 
	class Entity3D;	

	//! FollowPathVisitor class
	class FollowPathVisitor : public AnimationVisitor
	{
	public:

		DECLARE_OBJECT(FollowPathVisitor, AnimationVisitor);

		//! constructor
		FollowPathVisitor();

		//! Reads/Writes struct properties from/to a stream
		virtual void Serialize(PropertyStream& stream);		

		//! visits a particular entity
		virtual void Visit(Entity* pTarget);

		//! updates the visitor
		virtual void Update();

		//! overrides the path to follow
		inline void SetPath(Path* pPath) { m_Path.SetObject(pPath); }

	protected:

		Entity3D* m_p3DEntity;
		float m_fTimer;
		float m_fDuration;
		int m_SrcElement;
		int m_DestElement;

		// properties
		Link<Path> m_Path;
		bool m_bOrientEntity;
	};
}



