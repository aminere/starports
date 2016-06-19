/* 

Amine Rehioui
Created: October 15th 2011

*/

#include "GameEditorCommon.h"

#include "GameEditor.h"

#include "GameEditorRenderer.h"
#include "GameViewPort.h"
#include "GameEntity3DController.h"

namespace shoot
{
	//! constructor
	GameEditor::GameEditor()
		: m_bHexMode(false)
	{
		EditorRenderer::OverrideInstance(new GameEditorRenderer());
		Entity3DController::OverrideInstance(new GameEntity3DController());
	}

	//! destructor
	GameEditor::~GameEditor()
	{
	}

	//! returns a menu category for a class type
	std::string GameEditor::GetMenuCategory(const char* classType)
	{
		std::string category = ShootEditor::GetMenuCategory(classType);

		if(!category.empty())
		{
			return category;
		}

		std::map<const char*, const char*> aCategoryMap;

		// Game
		const char* strGame = "Game";
		aCategoryMap["Actor"] = strGame;
		aCategoryMap["GameManager"] = strGame;
		aCategoryMap["AudioManager"] = strGame;
		aCategoryMap["SFXManager"] = strGame;
		aCategoryMap["GameCamera"] = strGame;
		aCategoryMap["Playfield"] = strGame;
		aCategoryMap["InputController"] = strGame;

		// UI
		const char* strUI = "UI";
		aCategoryMap["UIContext"] = strUI;
		aCategoryMap["VKButton"] = strUI;
		aCategoryMap["VKeyboard"] = strUI;
		aCategoryMap["WheelMenu"] = strUI;
		aCategoryMap["WheelMenuButton"] = strUI;
		aCategoryMap["Minimap"] = strUI;
		aCategoryMap["UIMarker"] = strUI;
		aCategoryMap["ProgressBar"] = strUI;
		aCategoryMap["MenuVerse"] = strUI;		

		// Weapon
		const char* strWeapon = "Weapon";
		aCategoryMap["BulletManager"] = strWeapon;
		aCategoryMap["PulseManager"] = strWeapon;
		aCategoryMap["PelletManager"] = strWeapon;			
		aCategoryMap["LaserManager"] = strWeapon;			

		// Entities
		const char* strEntities = "Entities";
		aCategoryMap["Pathway"] = strEntities;
		aCategoryMap["PathwayElement"] = strEntities;
		aCategoryMap["SpawnPoint"] = strEntities;
		aCategoryMap["Defender"] = strEntities;
		aCategoryMap["ShootingAttacker"] = strEntities;
		aCategoryMap["KamikazeAttacker"] = strEntities;
		aCategoryMap["TankAttacker"] = strEntities;
		aCategoryMap["Tower"] = strEntities;
		aCategoryMap["GunTower"] = strEntities;
		aCategoryMap["FlameTower"] = strEntities;
		aCategoryMap["ElectricTower"] = strEntities;
		aCategoryMap["NuclearTower"] = strEntities;
		aCategoryMap["MenuStar"] = strEntities;
		aCategoryMap["BlackHole"] = strEntities;

		for(std::map<const char*, const char*>::iterator it = aCategoryMap.begin(); it != aCategoryMap.end(); ++it)
		{
			if(std::string((*it).first) == classType)
			{
				return (*it).second;
			}
		}

		return "";
	}

	//! Selects an entity
	Entity* GameEditor::SelectEntity(Entity* pEntity, const Vector3& vRayStart, const Vector3& vRayDir)
	{
		// setup Pin
		if(!pEntity && m_b3DControl)
		{
			Plane plane(Vector3::Create(0.0f, 0.0f, 1.0f), 0.0f);
			if(plane.IntersectWithRay(vRayStart, vRayDir, &m_vPinPosition))
			{
				SetHasPin(true);
			}
		}

		return super::SelectEntity(pEntity);
	}

	//! Creates a view port
	ViewPort* GameEditor::CreateViewPort(wxWindow* pParent)
	{
		return new GameViewPort(pParent);
	}
}

