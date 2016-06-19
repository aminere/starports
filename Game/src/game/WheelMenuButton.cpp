/* 

Amine Rehioui
Created: April 12th 2014

*/

#include "Game.h"

#include "WheelMenuButton.h"
#include "Playfield.h"

namespace shoot
{
	DEFINE_OBJECT(WheelMenuButton);	

	//! sets the alpha
	void WheelMenuButton::SetAlpha(float fAlpha)
	{
		auto bgMaterial = GetChildByName("BG")->GetComponent<GraphicComponent>()->GetMaterial();
		auto imageMaterial = GetChildByName("Image")->GetComponent<GraphicComponent>()->GetMaterial();
		bgMaterial->SetColor(Color::Create(bgMaterial->GetColor().RGB, fAlpha));
		imageMaterial->SetColor(Color::Create(imageMaterial->GetColor().RGB, fAlpha));
	}
}

