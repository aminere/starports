/*

Amine Rehioui
Created: April 26th 2015

*/

#include "Game.h"

#include "LevelSelectCamera.h"
#include "LevelSelectContext.h"

#include "SequenceVisitor.h"

namespace shoot
{
	DEFINE_OBJECT(LevelSelectCamera);

	//! constructor
	LevelSelectCamera::LevelSelectCamera()
		: m_vSpeed(Vector2::Zero)
		, m_Dragging(false)
		, m_SelectedLevel(0)
		, m_State(S_Idle)
		, m_LevelContext(NULL)
		// properties
		, m_DragTreshold(4.0f)
		, m_Bounds(Vector3::Create(-1800.0f, -1000.0f, -999.0f), Vector3::Create(300.0f, 0.0f, 999.0f))
		, m_AnimDuration(0.3f)
		, m_LevelSwitchOffset(-400.0f)
	{
		m_eType = Projection::Ortho2D;
	}

	//! serializes the entity to/from a PropertyStream
	void LevelSelectCamera::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("DragTreshold", &m_DragTreshold);
		stream.Serialize("AnimDuration", &m_AnimDuration);
		stream.Serialize("Bounds", &m_Bounds);
		stream.Serialize("LevelSwitchOffset", &m_LevelSwitchOffset);
	}

	//! Init
	void LevelSelectCamera::Init()
	{
		super::Init();

		m_Root = static_cast<Entity2D*>(GetParent()->GetChildByName("Root"));
		m_LevelPanel = static_cast<Entity2D*>(GetParent()->GetChildByName("LevelPanel"));
		m_Selector = static_cast<Entity2D*>(GetParent()->GetChildByName("Selector"));

		m_LevelContext = static_cast<LevelSelectContext*>(GetParent());
	}

	//! Update
	void LevelSelectCamera::Update()
	{
		// input
		if (InputManager::Instance()->IsTouchJustPressed())
		{
			m_vPreviousTouchPos = m_vDragStart = InputManager::Instance()->GetTouchState().vPosition;
			m_vStartingPos = m_Root->GetPosition();
		}
		else if (InputManager::Instance()->IsTouchPressed())
		{
			auto touchPos = InputManager::Instance()->GetTouchState().vPosition;
			auto deltaPos = touchPos - m_vDragStart;

			if (!m_Dragging)
			{
				m_Dragging = deltaPos.GetLength() > m_DragTreshold;
			}
			else
			{
				auto newPos = m_vStartingPos + deltaPos;
				newPos.Clamp(m_Bounds.Min().XY, m_Bounds.Max().XY);
				m_Root->SetPosition(newPos);
			}

			m_vPreviousTouchPos = touchPos;
		}
		else if (InputManager::Instance()->IsTouchJustReleased())
		{
			if (m_Dragging)
			{
				auto touchPos = InputManager::Instance()->GetTouchState().vPosition;
				auto swipe = touchPos - m_vPreviousTouchPos;
				m_vSpeed = swipe;
				m_State = S_Sliding;
				m_Dragging = false;
			}
			else
			{
				auto& buttons = m_LevelPanel->GetChildren();
				int selected = -1;
				int previouslySelected = -1;
				for (int i = 0; i < buttons.size(); ++i)
				{
					auto _button = static_cast<Entity2D*>(buttons[i].Get());
					Matrix44 transformation = Matrix44::Identity;
					transformation.Translate(-Vector3::Create(_button->GetCenter().X, _button->GetCenter().Y, 0.0f));
					transformation.Scale(Vector3::Create(_button->GetScale().X, _button->GetScale().Y, 1.0f));
					transformation.Translate(Vector3::Create(_button->GetPosition().X, _button->GetPosition().Y, 0.0f));
					if (Entity2D* p2DEntity = _button->GetAncestor<Entity2D>())
						transformation *= p2DEntity->GetWorldTransform();

					Matrix44 inverse;
					if (transformation.GetInverse(inverse))
					{
						Vector2 vTouchPos = InputManager::Instance()->GetTouchState().vPosition;
						Vector3 invTouchPos3D = inverse.TransformVect(Vector3::Create(vTouchPos.X, vTouchPos.Y, 0.0f));
						Vector2 vInvTouchPos = Vector2::Create(invTouchPos3D.X, invTouchPos3D.Y);
						if (_button->GetBoundingBox().Contains(vInvTouchPos))
						{
							selected = i;
							previouslySelected = m_SelectedLevel;
							if (selected != m_SelectedLevel)
								SelectLevel(selected);
							break;
						}
					}
				}

				if (selected >= 0)
				{
					auto unlocked = selected <= GAMEMGR->GetGameData()->GetLastUnlockedLevel();
					if (selected == previouslySelected)
					{
						if (unlocked)
							m_LevelContext->PlayLevel(selected);
					}
					else
					{
						// update content
						auto previous = buttons[previouslySelected].Get();
						auto next = buttons[selected].Get();
						auto previousUnlocked = previouslySelected <= GAMEMGR->GetGameData()->GetLastUnlockedLevel();
						previous->GetChildByName("Lock")->SetVisible(!previousUnlocked);
						previous->GetChildByName("Play")->SetVisible(false);
						previous->GetChildByName("Level")->SetVisible(previousUnlocked);
						next->GetChildByName("Lock")->SetVisible(!unlocked);
						next->GetChildByName("Play")->SetVisible(unlocked);
						next->GetChildByName("Level")->SetVisible(false);
					}
				}
			}
		}
		else
		{
			switch (m_State)
			{
			case S_Sliding:
			{
				auto newPos = m_Root->GetPosition() + m_vSpeed*g_fDeltaTime;
				newPos = newPos.Clamp(m_Bounds.Min().XY, m_Bounds.Max().XY);
				if (newPos == m_Root->GetPosition())
					m_State = S_Idle;
				else
					m_Root->SetPosition(newPos);
			}
			break;

			case S_Homing:
			{
				if (m_Interpolator < 1.0f)
				{
					auto pos = Math::Lerp(m_SrcPos, m_DestPos, m_Interpolator*m_Interpolator);
					m_Root->SetPosition(pos);
					m_Interpolator += (g_fDeltaTime / m_AnimDuration);
				}
				else
				{
					m_Root->SetPosition(m_DestPos);
					m_State = S_Idle;
				}
			}
			break;
			}

			// update current level
			uint level;
			if (m_Root->GetPosition().Y < m_LevelSwitchOffset)
				level = 1;
			else
				level = 0;

			if (level != m_LevelContext->GetCurrentLevel())			
				m_LevelContext->UpdateCurrentLevel(level);
		}
	}

	//! SelectLevel
	void LevelSelectCamera::SelectLevel(int level)
	{
		auto& buttons = m_LevelPanel->GetChildren();
		auto _button = static_cast<Entity2D*>(buttons[level].Get());
		m_Selector->SetPosition(_button->GetPosition() + m_Selector->GetCenter());
		auto anim = m_Selector->GetComponent<SequenceVisitor>();
		anim->Visit(m_Selector);

		auto res = GetTargetResolution();
		auto resv = Vector2::Create((float)res.Width, (float)res.Height);
		m_SrcPos = m_Root->GetPosition();
		m_DestPos = -_button->GetPosition() + (resv / 2.0f - m_Selector->GetCenter());
		m_DestPos = m_DestPos.Clamp(m_Bounds.Min().XY, m_Bounds.Max().XY);
		m_State = S_Homing;
		m_Interpolator = 0.0f;

		m_SelectedLevel = level;
	}
}

