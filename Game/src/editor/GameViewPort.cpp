/* 

Amine Rehioui
Created: January 9th 2014

*/

#include "GameEditorCommon.h"

#include "GameViewPort.h"
#include "GameEditor.h"
#include "GameEditorRenderer.h"

#include "Playfield.h"

namespace shoot
{
	// Define event table
	BEGIN_EVENT_TABLE(GameViewPort, ViewPort)
		EVT_MOTION(GameViewPort::OnMouseMove)
		EVT_LEFT_DOWN(GameViewPort::OnMouseLeftDown)
		EVT_LEFT_UP(GameViewPort::OnMouseLeftUp)		
	END_EVENT_TABLE()

	//! Constructor
	GameViewPort::GameViewPort(wxWindow *parent)
		: super(parent)
	{
	}

	//! event handlers
	void GameViewPort::OnMouseMove(wxMouseEvent& event)
	{
		super::OnMouseMove(event);
	}

	void GameViewPort::OnMouseLeftDown(wxMouseEvent& event)
	{
		if (!GameEditor::Instance()->GetHexMode())
		{
			super::OnMouseLeftDown(event);
			return;
		}
		
		if (!event.AltDown() && ShootEditor::Instance()->IsEngineRunning() && ShootEditor::Instance()->HasGameMouseControl())
		{
			OnMouseMiddleDown(event);
			return;
		}

		SetFocus();

		Camera* pCamera = EntityRenderer::Instance()->Get3DCamera();
		if (Engine::Instance()->IsLoading() || !pCamera)
			return;

		Vector2 clickPos = Vector2::Create(event.GetX(), event.GetY());
		Vector3 rayStart = Vector3::Zero, rayDir = Vector3::One;
		pCamera->Get3DRayFromScreenPos(rayStart, rayDir, clickPos);
		Vector3 vIntersection;
		auto gameContext = GameEngine::Instance()->GetGameContext();
		if (Plane(Vector3::Up, 0).IntersectWithRay(rayStart, rayDir, &vIntersection))
		{
			int y = -1, x = -1;
			Tile* pTile = GetTile(vIntersection, y, x);
			if (pTile)
			{
				if (event.ControlDown())
				{
					pTile->m_bBuildable = !pTile->m_bBuildable;
					static_cast<GameEditorRenderer*>(EditorRenderer::Instance())->SetPlayfieldDirty(true);
				}
				static char buf[256];
				sprintf(buf, "Selected Tile[%d][%d]", y, x);
				static_cast<wxFrame*>(wxTheApp->GetTopWindow())->SetStatusText(buf, 3);
			}
			gameContext->m_Playfield->SetSelectedTile(pTile);
		}
	}

	void GameViewPort::OnMouseLeftUp(wxMouseEvent& event)
	{
		super::OnMouseLeftUp(event);
	}

	//! returns the closest playfield tile
	Tile* GameViewPort::GetTile(const Vector3& vIntersection, int& y, int& x) const
	{
		auto gameContext = GameEngine::Instance()->GetGameContext();
		auto pPlayfield = gameContext->m_Playfield.Get();		
		for(int i=0; i<pPlayfield->GetDimension().Height; ++i)
		{
			int curRowSize = (i%2 == 0) ? pPlayfield->GetDimension().Width : pPlayfield->GetDimension().Width-1;
			for(int j=0; j<curRowSize; ++j)
			{
				auto tilePos = pPlayfield->GetAllTiles()[i][j].m_vPosition + Vector3::Create(pPlayfield->GetPosition().XY, 0.0f);
				if ((tilePos - vIntersection).GetLengthSquared() < pPlayfield->GetTileCollisionRadiusSq())
				{
					y = i;
					x = j;
					return &pPlayfield->GetAllTiles()[i][j];
				}
			}

			curRowSize = (curRowSize%2) ? curRowSize-1 : curRowSize+1;
		}

		return NULL;
	}
}

