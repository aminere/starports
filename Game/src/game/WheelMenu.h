/* 

Amine Rehioui
Created: April 11th 2014

*/

#pragma once


#include "WheelMenuButton.h"

namespace shoot
{
	class Tile;

	//! WheelMenu
	class WheelMenu : public Entity2D
	{
		DECLARE_OBJECT(WheelMenu, Entity2D);

	public:
		
		//! state
		enum E_State
		{
			S_Opening,
			S_Closing,
			S_Open,
			S_Closed
		};

		//! constructor
		WheelMenu();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);
		
		//! Initializes the menu
		void Open(Tile* pTile);

		//! called during the update of the entity
		void Update();

		//! closes the menu
		void Close();

		//! returns the screen bounds
		inline const AABBox2D& GetScreenBounds() const { return m_ScreenBounds; }

		//! returns the tile
		inline Tile* GetTile() const { return m_pTile; }

		//! returns the state
		inline E_State GetState() const { return m_eState; }
		
	private:

		//! AnimInfo
		struct AnimInfo
		{
			AnimInfo(WheelMenuButton* button, float angle)
				: Button(button), Angle(angle)
			{
			}

			Handle<WheelMenuButton> Button;
			float Angle;
		};

		//! ButtonInfo
		struct ButtonInfo
		{
			ButtonInfo(const std::string& action, Texture* texture, int price)
				: Action(action)
				, m_Texture(texture)
				, Price(price)
			{
			}

			std::string Action;
			Texture* m_Texture;
			int Price;
			Handle<Material> m_Material;
			Handle<Material> m_Foreground;
		};

		std::vector<AnimInfo> m_aAnimInfos;
		E_State m_eState;

		float m_fSrcRadius;
		float m_fDestRadius;
		float m_fSrcAngleOffset;
		float m_fDestAngleOffset;
		float m_fSrcAlpha;
		float m_fDestAlpha;
		float m_fInterpolator;
		Tile* m_pTile;
		std::vector<ButtonInfo> m_ButtonInfos;

		//! updates the position
		void UpdatePosition();

		// properties
		float m_fRadius;
		float m_fAnimDuration;
		float m_fAnimAngleOffset;
		AABBox2D m_ScreenBounds;
		Reference<Texture> SellIcon;
		Reference<Texture> RepairIcon;
		Reference<Texture> NuclearLaunchIcon;
		Reference<WheelMenuButton> m_ButtonRef;
	};
}



