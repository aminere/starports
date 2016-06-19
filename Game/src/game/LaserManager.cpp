/* 

Amine Rehioui
Created: March 5th 2013

*/

#include "Game.h"

#include "LaserManager.h"

#include "MaterialProvider.h"

#include "Quaternion.h"
#include "Basis3.h"

namespace shoot
{
	DEFINE_OBJECT(LaserManager);

	//! Constructor
	LaserManager::LaserManager()
		// properties
		: m_MaxLasers(64)
	{
	}

	//! destructor
	LaserManager::~LaserManager()
	{
	}

	//! serializes the entity to/from a PropertyStream
	void LaserManager::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("MaxLasers", &m_MaxLasers);
	}

	//! called during the initialization of the entity
	void LaserManager::Init()
	{
		m_Pool.SetCapacity(m_MaxLasers);

		GraphicComponent* pGraphic = GetComponent<GraphicComponent>();
		if(!pGraphic)
		{
			pGraphic = snew GraphicComponent();
			pGraphic->SetRenderingPass(GraphicComponent::RP_3DTransparent);
			AddComponent(pGraphic, true);
		}

		if(!pGraphic->GetMaterial())
		{
			pGraphic->SetMaterial(snew Material());
			pGraphic->GetMaterial()->SetCullMode(GraphicsDriver::CM_None);
			pGraphic->GetMaterial()->SetShader(Material::m_strDefaultShader);
		}
				
		int maxVertices = m_MaxLasers * 24;
		Vertex3D* pVertices = snew Vertex3D[maxVertices];
		VertexBuffer* pVB = GraphicsDriver::Instance()->CreateVertexBuffer();
		pVB->SetVertices(pVertices, maxVertices, Vertex3D::VF_Pos|Vertex3D::VF_UV);
		pVB->SetNumVertices(0);
		pVB->SetPrimitiveType(GraphicsDriver::PT_Triangle);
		pVB->SetName(GetClassName());
		pGraphic->SetVertexBuffer(pVB);

		pVB->SetDynamic(true);

		super::Init();
	}	

	//! called during the update of the entity	
	void LaserManager::Update()
	{
		VertexBuffer* pVB = GetComponent<GraphicComponent>()->GetVertexBuffer();
		pVB->SetNumVertices(0);

		if (m_Pool.GetElementCount())
		{
			int index = 0;
			Vertex3D* pVertices = reinterpret_cast<Vertex3D*>(pVB->GetVertices());

			for (size_t i = 0; i < m_Pool.GetCapacity(); ++i)			
			{
				auto laser = m_Pool.GetElement(i);
				if (!laser)
					continue;

				Basis3 localBasis(Vector3::Zero, laser->vDir);
				localBasis.Lateral = localBasis.Lateral*laser->fThickness;
				localBasis.Vertical = localBasis.Vertical*laser->fThickness;
				Vector3 vDiagonalAxe = Quaternion(laser->vDir, Math::PIBy4).Rotate(localBasis.Lateral);
				Vector3 vDiagonalAxe2 = Vector3::Create(vDiagonalAxe.X, vDiagonalAxe.Y, -vDiagonalAxe.Z);
				Vector3 vEnd = laser->vStart + laser->vDir * laser->fLength;

				pVertices[index+0].UV = Vector2::Create(0.0f, 0.0f); pVertices[index+0].Pos = vEnd+localBasis.Lateral;
				pVertices[index+1].UV = Vector2::Create(1.0f, 0.0f); pVertices[index+1].Pos = laser->vStart+localBasis.Lateral;
				pVertices[index+2].UV = Vector2::Create(0.0f, 1.0f); pVertices[index+2].Pos = vEnd-localBasis.Lateral;
				pVertices[index+3].UV = Vector2::Create(0.0f, 1.0f); pVertices[index+3].Pos = vEnd-localBasis.Lateral;
				pVertices[index+4].UV = Vector2::Create(1.0f, 0.0f); pVertices[index+4].Pos = laser->vStart+localBasis.Lateral;
				pVertices[index+5].UV = Vector2::Create(1.0f, 1.0f); pVertices[index+5].Pos = laser->vStart-localBasis.Lateral;

				pVertices[index+6].UV = Vector2::Create(0.0f, 0.0f); pVertices[index+6].Pos = vEnd+localBasis.Vertical;
				pVertices[index+7].UV = Vector2::Create(1.0f, 0.0f); pVertices[index+7].Pos = laser->vStart+localBasis.Vertical;
				pVertices[index+8].UV = Vector2::Create(0.0f, 1.0f); pVertices[index+8].Pos = vEnd-localBasis.Vertical;
				pVertices[index+9].UV = Vector2::Create(0.0f, 1.0f); pVertices[index+9].Pos = vEnd-localBasis.Vertical;
				pVertices[index+10].UV = Vector2::Create(1.0f, 0.0f); pVertices[index+10].Pos = laser->vStart+localBasis.Vertical;
				pVertices[index+11].UV = Vector2::Create(1.0f, 1.0f); pVertices[index+11].Pos = laser->vStart-localBasis.Vertical;

				pVertices[index+12].UV = Vector2::Create(0.0f, 0.0f); pVertices[index+12].Pos = vEnd+vDiagonalAxe;
				pVertices[index+13].UV = Vector2::Create(1.0f, 0.0f); pVertices[index+13].Pos = laser->vStart+vDiagonalAxe;
				pVertices[index+14].UV = Vector2::Create(0.0f, 1.0f); pVertices[index+14].Pos = vEnd-vDiagonalAxe;
				pVertices[index+15].UV = Vector2::Create(0.0f, 1.0f); pVertices[index+15].Pos = vEnd-vDiagonalAxe;
				pVertices[index+16].UV = Vector2::Create(1.0f, 0.0f); pVertices[index+16].Pos = laser->vStart+vDiagonalAxe;
				pVertices[index+17].UV = Vector2::Create(1.0f, 1.0f); pVertices[index+17].Pos = laser->vStart-vDiagonalAxe;

				pVertices[index+18].UV = Vector2::Create(0.0f, 0.0f); pVertices[index+18].Pos = vEnd+vDiagonalAxe2;
				pVertices[index+19].UV = Vector2::Create(1.0f, 0.0f); pVertices[index+19].Pos = laser->vStart+vDiagonalAxe2;
				pVertices[index+20].UV = Vector2::Create(0.0f, 1.0f); pVertices[index+20].Pos = vEnd-vDiagonalAxe2;
				pVertices[index+21].UV = Vector2::Create(0.0f, 1.0f); pVertices[index+21].Pos = vEnd-vDiagonalAxe2;
				pVertices[index+22].UV = Vector2::Create(1.0f, 0.0f); pVertices[index+22].Pos = laser->vStart+vDiagonalAxe2;
				pVertices[index+23].UV = Vector2::Create(1.0f, 1.0f); pVertices[index+23].Pos = laser->vStart-vDiagonalAxe2;

				index += 24;
				pVB->SetNumVertices(index);

				laser->fLife -= g_fDeltaTime;
				float fDelta = laser->fSpeed * g_fDeltaTime;
				if(laser->fLife > 0.0f)
				{
					laser->fLength += fDelta;
					if(laser->fLength > laser->fMaxLength)
					{
						laser->fLength = laser->fMaxLength;
						if(!laser->bStatic)
						{
							laser->vStart += laser->vDir * fDelta;
						}
					}
				}
				else
				{
					laser->fLength -= fDelta;
					if(laser->fLength > 0.0f)
					{
						laser->vStart += laser->vDir * fDelta;						
					}
					else
					{
						m_Pool.Free(laser);
					}
				}
			}

			pVB->SetDirty(true);
		}
	}
	
	//! Adds a laser
	Laser* LaserManager::AddLaser(const Laser::Params& params)
	{
		if(Laser* pLaser = m_Pool.Alloc())
		{
			pLaser->Init(params);
			return pLaser;
		}
		return NULL;
	}

	//! clears all lasers
	void LaserManager::Clear()
	{
		for (size_t i = 0; i < m_Pool.GetCapacity(); ++i)
		{
			auto laser = m_Pool.GetElement(i);
			if (!laser)
				continue;

			laser->fLife = 0.0f;
		}
	}
}

