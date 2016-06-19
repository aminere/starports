/* 

Amine Rehioui
Created: September 7th 2014

*/

#pragma once


#include "AnimationVisitor.h"

namespace shoot
{
	//! TextureAnimationVisitor
	class TextureAnimationVisitor : public AnimationVisitor
	{
	public:

		DECLARE_OBJECT(TextureAnimationVisitor, AnimationVisitor);

		//! constructor
		TextureAnimationVisitor();

		//! Reads/Writes struct properties from/to a stream
		void Serialize(PropertyStream& stream);		

		//! visits a particular entity
		void Visit(Entity* pTarget);

		//! updates the visitor
		void Update();

	protected:
		
		Handle<GraphicComponent> m_TargetGC;
		float m_fDuration;
		float m_fCurrentTime;

		// KeyFrame
		struct KeyFrame : ISerializableStruct
		{
			float Time;
			Vector2 Tiling;
			Vector2 Offset;
			float Angle;

			KeyFrame()
				: Time(0.0f)
				, Tiling(Vector2::One)
				, Offset(Vector2::Zero)
				, Angle(0.0f)
			{
			}

			void Serialize(PropertyStream& stream)
			{
				stream.Serialize("Time", &Time);
				stream.Serialize("Tiling", &Tiling);
				stream.Serialize("Offset", &Offset);
				stream.Serialize("Angle", &Angle);
			}
		};

		//! returns the current interpolated key
		KeyFrame GetInterpolatedKey() const;

		//! updates the texture transform
		void UpdateTextureTransform();

		// properties
		Array<KeyFrame> m_KeyFrames;
	};
}




