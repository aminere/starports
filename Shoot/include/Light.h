/* 

Amine Rehioui
Created: October 8th 2012

*/

#pragma once


#include "Projection.h"

namespace shoot
{
	//! LightLookAt
	class LightLookAt : public Entity3D
	{
		//! Macro to provide type information
		DECLARE_OBJECT(LightLookAt, Entity3D);

	public:

		//! Sets the position relative to the parent's position
		void SetPosition(const Vector3& vPos);

		//! Translates the entity
		void Translate(const Vector3& amount);
	};

	//! Light entity
	class Light : public Entity3D				 
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(Light, Entity3D);

		//! Constructor
		Light();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		void Init();

		//! Sets the position relative to the parent's position
		void SetPosition(const Vector3& vPos);

		//! Translates the entity
		void Translate(const Vector3& amount);

		//! returns the view matrix
		const Matrix44& GetViewProjectionMatrix();

		//! returns the frustum
		inline const Frustum& GetFrustum() { return m_bFrustumDirty ? UpdateFrustum() : m_Frustum; }

		//! returns the color
		inline const Color& GetColor() const { return m_Color; }

		//! sets the color
		inline void SetColor(const Color& color) { m_Color = color; }

		//! GetAttenuationFactor
		inline float GetAttenuationFactor() const { return m_AttenuationFactor; }

		//! called when the view has changed
		void OnViewChanged();

	private:

		Handle<LightLookAt> m_LookAt;
		Frustum m_Frustum;
		Matrix44 m_ProjectionMatrix;
		Matrix44 m_ViewMatrix;
		Matrix44 m_ViewProjectionMatrix;
		bool m_bFrustumDirty;
		bool m_bProjectionMatrixDirty;
		bool m_bViewMatrixDirty;
		bool m_bViewProjectionMatrixDirty;

		//! returns the projection matrix
		inline const Matrix44& GetProjectionMatrix() { return m_bProjectionMatrixDirty ? UpdateProjectionMatrix() : m_ProjectionMatrix; }

		//! returns the view matrix
		inline const Matrix44& GetViewMatrix() { return m_bViewMatrixDirty ? UpdateViewMatrix() : m_ViewMatrix; }

		//! update the projection matrix
		const Matrix44& UpdateProjectionMatrix();

		//! update the view matrix
		const Matrix44& UpdateViewMatrix();

		//! updates the frustum
		const Frustum& UpdateFrustum();

		// properties
		Color m_Color;
		Vector3 m_vUpVector;
		float m_FOV;
		float m_fZNear;
		float m_fZFar;
		Projection::Type _projection;
		Vector2 _orthoSize;
		float m_AttenuationFactor;
	};
}



