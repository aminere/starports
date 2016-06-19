/* 

Amine Rehioui
Created: November 11th 2013

*/

#pragma once


namespace shoot
{
	//! VKeyboard
	class VKeyboard : public Entity2D				 
	{	
		DECLARE_OBJECT(VKeyboard, Entity2D);

	public:

		//! Constructor
		VKeyboard();

		//! Destructor
		virtual ~VKeyboard();

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		virtual void Init();

		//! called during the update of the entity
		virtual void Update();

		//! toggles the mode
		void ToggleMode();

	private:
		
		bool m_bShowLetters;
		Handle<Entity> m_Row1, m_Row1Numbers;
		Handle<Entity> m_Row2, m_Row2Numbers;
		Handle<Entity> m_Row3, m_Row3Numbers;
		
		//! fills a row
		Entity* FillRow(char* letters, int letterCount, float rowY);
	};
}



