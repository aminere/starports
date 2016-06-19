/* 

Amine Rehioui
Created: November 18th 2011

*/

#include "Game.h"

#include "BulletManager.h"

#include "GameManager.h"

namespace shoot
{
	DEFINE_OBJECT(BulletManager);

	//! Constructor
	BulletManager::BulletManager()
		// properties
		: m_MaxBullets(128)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void BulletManager::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("MaxBullets", &m_MaxBullets);
	}

	//! called during the initialization of the entity
	void BulletManager::Init()
	{
		m_Pool.SetCapacity(m_MaxBullets);

		GraphicComponent* pGraphic = GetComponent<GraphicComponent>();
		if(!pGraphic)
		{
			pGraphic = snew GraphicComponent();
			pGraphic->SetRenderingPass(GraphicComponent::RP_3DTransparent);
			AddComponent(pGraphic, true);
		}

		super::Init();
	}

	//! called during the update of the entity	
	void BulletManager::Update()
	{
		VertexBuffer* pVB = GetComponent<GraphicComponent>()->GetVertexBuffer();
		pVB->SetNumVertices(0);

		if (m_Pool.GetElementCount())
		{
			int bulletIndex = 0;

			for (size_t i = 0; i < m_Pool.GetCapacity(); ++i)
			{
				auto bullet = m_Pool.GetElement(i);
				if (!bullet)
					continue;

				if (bullet->fLife > 0.0f)
				{
					bullet->vPosition += bullet->vDirection*bullet->fSpeed*g_fDeltaTime;
					SetupRendering(bullet, bulletIndex++, pVB);

					bullet->fLife -= g_fDeltaTime;
				}
				else
				{
					m_Pool.Free(bullet);
				}
			}

			pVB->SetDirty(true);
		}		
	}

	//! Adds a bullet
	void BulletManager::AddBullet(const Bullet::BulletParams& params)
	{
		if(Bullet* pBullet = m_Pool.Alloc())		
			pBullet->Init(params);		
	}

	//! clears the bullets
	void BulletManager::Clear()
	{
		for (size_t i = 0; i < m_Pool.GetCapacity(); ++i)
		{
			auto bullet = m_Pool.GetElement(i);
			if (!bullet)
				continue;

			bullet-> fLife = 0.0f;
		}
	}
}

