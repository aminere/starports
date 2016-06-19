/* 

Amine Rehioui
Created: May 1st 2013

*/

#pragma once


#include "BulletManager.h"

namespace shoot
{
	// forwards
	class VertexBuffer;

	//! PelletManager class
	class PelletManager : public BulletManager
	{
	public:
		
		DECLARE_OBJECT(PelletManager, BulletManager);

		//! Constructor
		PelletManager();

		//! destructor
		virtual ~PelletManager();

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		virtual void Init();

		//! called during the update of the entity		
		virtual void Update();

		//! sets the max homing angle
		inline void SetMaxHomingAngle(float fAngle) 
		{
			m_fMaxHomingAngle = fAngle;
			m_fMinCos = Math::Cos(m_fMaxHomingAngle*Math::DegToRadFactor);
		}

	private:

		//! Prepares a bullet for rendering
		void SetupRendering(Bullet* pBullet, int index, VertexBuffer* pVB);

		float m_fMinCos;

		//! properties
		float m_fBulletSize;
		float m_fMinDistToActor;
		float m_fHomingFactor;
		float m_fMaxHomingAngle;
	};
}




