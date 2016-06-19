/* 

Amine Rehioui
Created: April 26th 2015

*/

#pragma once

#include "Camera.h"

namespace shoot
{
	class LevelSelectContext;

	//! LevelSelectCamera
	class LevelSelectCamera : public Camera
	{
		//! Macro to provide type information
		DECLARE_OBJECT(LevelSelectCamera, Camera);

	public:

		//! constructor
		LevelSelectCamera();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! Init
		void Init();

		//! Update
		void Update();

		//! SelectLevel
		void SelectLevel(int level);

	private:

		Vector2 m_vDragStart;
		Vector2 m_vStartingPos;
		LevelSelectContext* m_LevelContext;

		//! state
		enum E_State
		{
			S_Idle,
			S_Sliding,
			S_Homing
		};

		Vector2 m_vPreviousTouchPos;
		Vector2 m_vSpeed;
		bool m_Dragging;
		int m_SelectedLevel;
		E_State m_State;
		Vector2 m_SrcPos;
		Vector2 m_DestPos;
		float m_Interpolator;

		Handle<Entity2D> m_Root;
		Handle<Entity2D> m_LevelPanel;
		Handle<Entity2D> m_Selector;

		// properties
		float m_DragTreshold;
		AABBox3D m_Bounds;
		float m_AnimDuration;
		float m_LevelSwitchOffset;
	};
}

