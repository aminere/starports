/* 

Amine Rehioui
Created: September 7th 2014

*/

#include "Shoot.h"	

#include "TextureAnimationVisitor.h"
#include "cpplinq.hpp"

namespace shoot
{
	DEFINE_OBJECT(TextureAnimationVisitor);

	//! constructor
	TextureAnimationVisitor::TextureAnimationVisitor()
		: m_fDuration(0.0f)
		, m_fCurrentTime(0.0f)
	{
	}

	//! Reads/Writes struct properties from/to a stream
	void TextureAnimationVisitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		
		stream.Serialize("KeyFrames", &m_KeyFrames);
	}	

	//! visits a particular entity
	void TextureAnimationVisitor::Visit(Entity* pTarget)
	{
		m_TargetGC = pTarget->GetComponent<GraphicComponent>();
		if(!m_TargetGC.IsValid())
		{
			SHOOT_WARNING(false, "TextureAnimationVisitor target has no GraphicComponent");
			return;
		}
		
		std::sort(m_KeyFrames.begin(), m_KeyFrames.end(), [](KeyFrame* a, KeyFrame* b) { return a->Time < b->Time; });
		m_fCurrentTime = 0.0f;

		if(m_KeyFrames.GetSize() > 1)
		{
			m_fDuration = m_KeyFrames.last()->Time;
			UpdateTextureTransform();
			super::Visit(pTarget);
		}
	}

	//! updates the visitor	
	void TextureAnimationVisitor::Update()
	{
		if(!m_bActive)
			return;
		
		if(m_fCurrentTime < m_fDuration)
		{
			UpdateTextureTransform();
			m_fCurrentTime += g_fDeltaTime;
		}		
		else
		{
			m_fCurrentTime = m_fDuration;
			UpdateTextureTransform();
			m_iPlayCount++;

			switch(m_ePlaybackType)
			{
			case PT_Once:
				Leave();
				break;

			case PT_Loop:
				if((m_iMaxPlayCount < 0) || (m_iPlayCount < m_iMaxPlayCount))				
					m_fCurrentTime = 0.0f;
				else
					Leave();
				break;
				
			default:
				Leave();								
			}			
		}
	}

	//! returns the current interpolated key
	TextureAnimationVisitor::KeyFrame TextureAnimationVisitor::GetInterpolatedKey() const
	{
		auto it = std::find_if(m_KeyFrames.begin(), m_KeyFrames.end(), [this](const KeyFrame* k) { return Math::FEqual(k->Time, m_fCurrentTime); });
		if(it != m_KeyFrames.end())
			return *(*it);

		using namespace cpplinq;
		auto afterKeys = from(m_KeyFrames.Elements()) >> where ([this](const KeyFrame* k) { return k->Time > m_fCurrentTime; }) >> to_vector();
		if(afterKeys.size() == m_KeyFrames.GetSize())
			return *afterKeys.front();

		auto beforeKeys = from(m_KeyFrames.Elements()) >> where ([this](const KeyFrame* k) { return k->Time < m_fCurrentTime; }) >> to_vector();

		KeyFrame* srcKey = beforeKeys.back();
		KeyFrame* destKey = afterKeys.front();
		float fFactor = (m_fCurrentTime-srcKey->Time)/(destKey->Time-srcKey->Time);
		KeyFrame interpolated;
		interpolated.Tiling = Math::Lerp(srcKey->Tiling, destKey->Tiling, fFactor);
		interpolated.Offset = Math::Lerp(srcKey->Offset, destKey->Offset, fFactor);
		interpolated.Angle = Math::Lerp(srcKey->Angle, destKey->Angle, fFactor);
		return interpolated;
	}

	//! updates the texture transform
	void TextureAnimationVisitor::UpdateTextureTransform()
	{
		KeyFrame currentKey = GetInterpolatedKey();
		Matrix44 textureTransform = Matrix44::Identity;
		textureTransform.Scale(Vector3::Create(currentKey.Tiling.X, currentKey.Tiling.Y, 1.0f));
		textureTransform.Rotate(Vector3::Create(0.0f, 0.0f, currentKey.Angle*Math::DegToRadFactor));
		textureTransform.Translate(Vector3::Create(currentKey.Offset.X, currentKey.Offset.Y, 0.0f));
		m_TargetGC->SetTextureTransform(textureTransform);
	}
}

