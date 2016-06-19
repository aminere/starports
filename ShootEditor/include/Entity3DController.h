/* 

Amine Rehioui
Created: May 14th 2010

*/

#pragma once

namespace shoot
{
	//! Helper to control 3D entities in the editor
	class Entity3DController : public Singleton<Entity3DController>
	{
		DECLARE_SINGLETON(Entity3DController);

	public:
		
		//! Renders the controller elements
		void Render();
	
		//! Sets the 3D entity to control
		void SetEntity(Entity3D* pEntity);

		//! is called when the 3D picking ray has changed
		void OnPickingRayMoved(const Vector3& vRayStart, const Vector3& vRayEnd);

		//! called when mouse is dragged
		void OnMouseDragged(const Vector3& vRayStart, const Vector3& vRayDir, bool ctrlDown);

		//! returns true is one of the axes is being highlighted by the mouse
		bool IsAxeHighlighted() const { return m_SelectedAxes != Axe_None; }

	protected:

		//! OnEntityTranslated
		virtual void OnTranslate(const Vector3&);

		Handle<Entity3D> m_Entity;
		Matrix44 m_EntityCenterTransformation;

		Vector3 m_vIntersection;

		// settings
		Color m_AxeHighlightColor;
		float m_AxeSize;		
		float m_fTranslationStep;
		float m_fRotationStep;
		float m_fScaleStep;

		float m_fProportionalAxeSize;

		//! transformation axes
		enum E_Axes
		{			
			Axe_X,
			Axe_Y,
			Axe_Z,
			Axe_XY,
			Axe_XZ,
			Axe_ZY,
			Axe_None
		};

		E_Axes m_SelectedAxes;
	};
}

