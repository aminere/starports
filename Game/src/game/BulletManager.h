/* 

Amine Rehioui
Created: February 5th 2012

*/

#pragma once


#include "RenderableEntity.h"

#include "MemoryPool.h"

namespace shoot
{
	// forwards
	class VertexBuffer;

	//! Bullet class
	class Bullet
	{
	public:
		
		//! bullet type
		enum E_Type
		{
			T_Pulse,
			T_Pellet
		};

		//! bullet params
		struct BulletParams
		{
			BulletParams()
				: vPosition(Vector3::Zero)
				, vDirection(Vector3::Zero)
				, fSpeed(50.0f)
				, fLife(5.0f)
				, fRadius(.5f)
				, damage(30.0f)
				, type(T_Pulse)
			{
			}
			
			Vector3 vPosition;
			Vector3 vDirection;
			float fSpeed;
			float fLife;
			float fRadius;
			float damage;
			E_Type type;
		};

		//! Initialize
		void Init(const BulletParams& params)
		{
			vPosition = params.vPosition;
			vDirection = params.vDirection;
			fSpeed = params.fSpeed;
			fLife = params.fLife;
			fRadius = params.fRadius;
			damage = params.damage;
			type = params.type;
		}

		Vector3 vPosition;
		Vector3 vDirection;
		float fSpeed;
		float fLife;
		float fRadius;
		float damage;
		E_Type type;
	};

	//! BulletManager class
	class BulletManager : public RenderableEntity
	{
	public:
		
		DECLARE_OBJECT(BulletManager, RenderableEntity);

		//! Constructor
		BulletManager();

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		virtual void Init();

		//! called during the update of the entity		
		virtual void Update();

		//! Adds a bullet
		virtual void AddBullet(const Bullet::BulletParams& params);

		//! returns the bullet list
		inline MemoryPool<Bullet>& GetBullets() { return m_Pool; }

		//! clears the bullets
		void Clear();

	protected:

		//! Prepares a bullet for rendering
		virtual void SetupRendering(Bullet* pBullet, int index, VertexBuffer* pVB) { }

		MemoryPool<Bullet> m_Pool;

		// properties
		int m_MaxBullets;
	};
}




