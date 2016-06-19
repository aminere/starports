/* 

Amine Rehioui
Created: April 7th 2011

*/

#include "Shoot.h"	

#include "Scale3DVisitor.h"

namespace shoot
{
	DEFINE_OBJECT(Scale3DVisitor);

	//! constructor
	Scale3DVisitor::Scale3DVisitor()	
		: m_p3DEntity(NULL)
		, m_fCurrentTime(0.0f)
		, m_vSrcScale(Vector3::One)
		, m_vDestScale(Vector3::One)
		// properties
		, m_fDuration(1.0f)
		, m_bRandomize(false)
	{
	}

	//! destructor
	Scale3DVisitor::~Scale3DVisitor()
	{
	}

	//! Reads/Writes struct properties from/to a stream
	void Scale3DVisitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("Src", &m_Src);
		stream.Serialize("Dest", &m_Dest);
		stream.Serialize("Duration", &m_fDuration);
		stream.Serialize("Randomize", &m_bRandomize);
	}

	//! visits a particular entity
	void Scale3DVisitor::Visit(Entity* pTarget)
	{
		SHOOT_ASSERT(pTarget->IsA(Entity3D::TypeID), "Scale3DVisitor target is not of type Entity3D");
		m_p3DEntity = static_cast<Entity3D*>(pTarget);
		m_fCurrentTime = 0.0f;
		ESP<Vector3>::Compute<Entity3D>(m_Src, m_Dest, m_vSrcScale, m_vDestScale, m_p3DEntity, &Entity3D::GetScale);

		if(m_bRandomize)
		{
			m_vDestScale = Vector3::Create(Random::GetFloat(0.0f, m_vDestScale.X),
											  Random::GetFloat(0.0f, m_vDestScale.Y),
											  Random::GetFloat(0.0f, m_vDestScale.Z));
		}

		m_fCurrentDuration = m_bRandomize ? (Random::GetFloat(0.0f, m_fDuration)) : (m_fDuration);

		super::Visit(pTarget);
	}

	//! updates the visitor	
	void Scale3DVisitor::Update()
	{
		if(!m_bActive)
			return;
		
		if(m_fCurrentTime < m_fCurrentDuration)
		{
			Vector3 scale = Math::Lerp(m_vSrcScale, m_vDestScale, m_fCurrentTime/m_fCurrentDuration);
			m_p3DEntity->SetScale(scale);
			m_fCurrentTime += g_fDeltaTime;
		}
		else
		{
			m_p3DEntity->SetScale(m_vDestScale);
			m_iPlayCount++;

			switch(m_ePlaybackType)
			{
			case PT_Once:
				Leave();
				break;

			case PT_Loop:
			case PT_Toggle:
				if((m_iMaxPlayCount < 0) || (m_iPlayCount < m_iMaxPlayCount))
				{
					(m_ePlaybackType == PT_Toggle) ? Math::Swap(m_vSrcScale, m_vDestScale) : void();
					m_fCurrentTime = 0.0f;
				}
				else
					Leave();
				break;
				
			default:
				Leave();								
			}			
		}
	}
}

