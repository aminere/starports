/* 

Amine Rehioui
Created: September 3rd 2013

*/

#pragma once


namespace shoot
{
	// forwards
	class Sprite;

	//! Button
	class Button : public Entity2D				 
	{	
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(Button, Entity2D);

		//! Constructor
		Button();

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		virtual void Init();

		//! called during the update of the entity
		virtual void Update();

		//! sets checked status
		void SetChecked(bool bChecked);

		//! sets the command
		inline void SetCommand(const std::string& command) { m_Command = command; }

		//! SetUIEnabled
		inline void SetUIEnabled(bool enabled) { m_UIEnabled = enabled; }

	protected:

		//! returns true if the button is touched
		virtual bool IsTouched() const;

		//! sends the UI event
		virtual void SendUIEvent();

		//! changes pressed status
		virtual void SetPressed(bool bPressed);

		bool m_bPressed;
		bool m_bChecked;
		Handle<Sprite> m_Icon;
		Vector2 m_vOriginalPosition;

		// properties
		std::string m_Command;
		bool m_bCheckBox;
		bool m_bCustomBBox;
		bool m_UIEnabled;
	};
}



