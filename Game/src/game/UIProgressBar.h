/* 

Amine Rehioui
Created: December 16th 2014

*/

#pragma once


#include "Image.h"

namespace shoot
{
	//! UIProgressBar
	class UIProgressBar : public Image				 
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(UIProgressBar, Image);

		//! Constructor
		UIProgressBar();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		void Init();

		//! called during the update of the entity		
		void Update();

		//! sets the progress in the range [0.0 - 1.0]
		inline void SetProgress(float progress) 
		{ 
			m_Progress = Math::Clamp(progress, 0.0f, 1.0f);
			m_Dirty = true;
		}

	private:
				
		float m_Progress;
		bool m_Dirty;
	};
}



