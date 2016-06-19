/* 

Amine Rehioui
Created: December 15th 2014

*/

#pragma once


namespace shoot
{
	//! LayoutComponent
	class LayoutComponent : public Component
	{
	public:

		//! HorizontalAlignement
		enum E_HorizontalAlignement
		{
			HA_Left,
			HA_Center,
			HA_Right
		};

		//! VerticalAlignement
		enum E_VerticalAlignement
		{
			VA_Top,
			VA_Center,
			VA_Bottom
		};

		DECLARE_OBJECT(LayoutComponent, Component);

		//! constructor
		LayoutComponent();

		//! Reads/Writes struct properties from/to a stream
		void Serialize(PropertyStream& stream);

		//! Align
		Matrix44 Align(Entity2D* target, const Vector2& position) const;

	private:

		// properties
		E_HorizontalAlignement m_HorizontalAlignment;
		E_VerticalAlignement m_VerticalAlignment;
	};
}




