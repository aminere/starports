/* 

Amine Rehioui
Created: April 7th 2011

*/

#include "Shoot.h"	

#include "Rotation3DVisitor.h"

namespace shoot
{
	DEFINE_OBJECT(Rotation3DVisitor);

	//! constructor
	Rotation3DVisitor::Rotation3DVisitor()	
		: m_p3DEntity(NULL)
		// properties		
		, m_fDuration(1.0f)
		, m_bRandomize(false)
	{
	}

	//! destructor
	Rotation3DVisitor::~Rotation3DVisitor()
	{
	}

	//! Reads/Writes struct properties from/to a stream
	void Rotation3DVisitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);	

		stream.Serialize("Src", &m_Src);
		stream.Serialize("Dest", &m_Dest);
		stream.Serialize("Duration", &m_fDuration);
		stream.Serialize("Randomize", &m_bRandomize);
	}

	//! visits a particular entity
	void Rotation3DVisitor::Visit(Entity* pTarget)
	{
		SHOOT_ASSERT(pTarget->IsA(Entity3D::TypeID), "Rotation3DVisitor target is not of type Entity3D");
		m_p3DEntity = static_cast<Entity3D*>(pTarget);
		m_fCurrentTime = 0.0f;
		ESP<Vector3>::Compute<Entity3D>(m_Src, m_Dest, m_vSrcRotation, m_vDestRotation, m_p3DEntity, &Entity3D::GetRotation);

		if(m_bRandomize)
		{
			m_vDestRotation = Vector3::Create(Random::GetFloat(-m_vDestRotation.X, m_vDestRotation.X),
											  Random::GetFloat(-m_vDestRotation.Y, m_vDestRotation.Y),
											  Random::GetFloat(-m_vDestRotation.Z, m_vDestRotation.Z));
		}

		m_fCurrentDuration = m_bRandomize ? (Random::GetFloat(0.0f, m_fDuration)) : (m_fDuration);

		super::Visit(pTarget);
	}

	//! updates the visitor	
	void Rotation3DVisitor::Update()
	{
		if(!m_bActive)
			return;		

		if(m_fCurrentTime < m_fCurrentDuration)
		{
			Vector3 rotation = Math::Lerp(m_vSrcRotation, m_vDestRotation, m_fCurrentTime/m_fCurrentDuration);			
			m_p3DEntity->SetRotation(rotation);

			m_fCurrentTime += g_fDeltaTime;
		}
		else
		{
			m_p3DEntity->SetRotation(m_vDestRotation);
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
					(m_ePlaybackType == PT_Toggle) ? Math::Swap(m_vSrcRotation, m_vDestRotation) : void();
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

