/* 

Amine Rehioui
Created: October 9th 2013

*/

#pragma once


namespace shoot
{
	// forwards
	class Image;

	//! UISlider
	class UISlider : public Entity2D				 
	{	
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(UISlider, Entity2D);

		//! Constructor
		UISlider();

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		virtual void Init();

		//! called during the update of the entity
		virtual void Update();

		//! sets the slider ratio
		void SetRatio(float fRatio);

	private:

		//! returns true if the slider is touched
		bool IsTouched() const;

		Handle<Image> m_Selector;
		Handle<Image> m_BG;
		float m_fRatio;
		Vector2 m_vPreviousTouchPos;
		bool m_bStartDragging;

		// properties
		Range m_PositionRange;
	};
}



