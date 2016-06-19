/* 

Amine Rehioui
Created: February 5th 2012

*/

#pragma once


#include "BulletManager.h"

namespace shoot
{
	// forwards
	class VertexBuffer;

	//! PulseManager class
	class PulseManager : public BulletManager
	{
	public:
		
		DECLARE_OBJECT(PulseManager, BulletManager);

		//! Constructor
		PulseManager();

		//! destructor
		virtual ~PulseManager();

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		virtual void Init();

	private:

		//! Prepares a bullet for rendering
		void SetupRendering(Bullet* pBullet, int index, VertexBuffer* pVB);
	};
}




