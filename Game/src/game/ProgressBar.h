/* 

Amine Rehioui
Created: January 11th 2014

*/

#pragma once


namespace shoot
{
	//! ProgressBar
	class ProgressBar : public Entity3D				 
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(ProgressBar, Entity3D);

		//! Constructor
		ProgressBar();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		void Init();

		//! called during the update of the entity		
		void Update();

		//! returns the affine transformation matrix
		Matrix44 GetWorldTransform();

		//! sets the progress in the range [0.0 - 1.0]
		inline void SetProgress(float progress)
		{
			m_Progress = Math::Clamp(progress, 0.0f, 1.0f);
			m_Dirty = true;
		}

	private:

		float m_Progress;
		bool m_Dirty;

		// properties
		Color m_ColorFull;
		Color m_ColorEmpty;
	};
}



