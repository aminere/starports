/* 

Amine Rehioui
Created: October 8th 2012

*/

#include "Shoot.h"

#include "Light.h"

namespace shoot
{
	DEFINE_OBJECT(Light);
	DEFINE_OBJECT(LightLookAt);

	//! Sets the position relative to the parent's position
	void LightLookAt::SetPosition(const Vector3& vPos)
	{ 
		super::SetPosition(vPos);
		static_cast<Light*>(GetParent())->OnViewChanged();
	}	

	//! Translates the entity
	void LightLookAt::Translate(const Vector3& amount)
	{
		super::Translate(amount);
		static_cast<Light*>(GetParent())->OnViewChanged();
	}

	//! Constructor
	Light::Light()
		: m_bProjectionMatrixDirty(true)
		, m_bViewMatrixDirty(true)
		, m_bViewProjectionMatrixDirty(true)
		, m_bFrustumDirty(true)
		// properties
		, m_Color(Color::White)
		, m_FOV(45.0f)
		, m_fZNear(0.5f)
		, m_fZFar(300.0f)
		, m_vUpVector(Vector3::Create(0.0f, 0.0f, 1.0f))
		, _projection(Projection::Perspective)
		, _orthoSize(Vector2::Create(2.0f, 2.0f))
		, m_AttenuationFactor(0.001f)
	{		
	}

	//! serializes the entity to/from a PropertyStream
	void Light::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		
		stream.Serialize("Color", &m_Color);
		stream.Serialize("FOV", &m_FOV);
		stream.Serialize("ZNear", &m_fZNear);
		stream.Serialize("ZFar", &m_fZFar);
		stream.Serialize("UpVector", &m_vUpVector);
		stream.Serialize("Projection", &_projection, ENUM_PARAMS1(Projection::Literals));
		stream.Serialize("OrthoSize", &_orthoSize);
		stream.Serialize("AttenuationFactor", &m_AttenuationFactor);		

		m_bProjectionMatrixDirty = true;
		m_bViewMatrixDirty = true;
		m_bViewProjectionMatrixDirty = true;
		m_bFrustumDirty = true;
	}

	//! called during the initialization of the entity
	void Light::Init()
	{
		super::Init();

		m_LookAt = static_cast<LightLookAt*>(GetChildByType("LightLookAt"));
		if(!m_LookAt.IsValid())
		{
			LightLookAt* pLookAt = snew LightLookAt();
			pLookAt->SetSavableToXML(true);
			AddChild(pLookAt, true);
			m_LookAt = pLookAt;
		}
	}

	//! Sets the position relative to the parent's position
	void Light::SetPosition(const Vector3& vPos)
	{
		super::SetPosition(vPos);
		OnViewChanged();
	}

	//! Translates the entity
	void Light::Translate(const Vector3& amount)
	{
		super::Translate(amount);
		OnViewChanged();
	}

	//! returns the view matrix
	const Matrix44& Light::GetViewProjectionMatrix()
	{
		if(m_bViewProjectionMatrixDirty)
		{
			m_ViewProjectionMatrix = GetViewMatrix()*GetProjectionMatrix();
			m_bViewProjectionMatrixDirty = false;
		}
		return m_ViewProjectionMatrix;
	}

	//! update the projection matrix
	const Matrix44& Light::UpdateProjectionMatrix()
	{
		switch(_projection)
		{
		case Projection::Ortho3D:
			m_ProjectionMatrix = Matrix44::Make3DOrthographicProjection(_orthoSize.X, _orthoSize.Y, m_fZNear, m_fZFar);
			break;

		case Projection::Perspective:
		default:
			{
				auto vScreenSize = GraphicsDriver::Instance()->GetViewPort().Size();
				m_ProjectionMatrix = Matrix44::MakePerspectiveProjection(m_FOV, vScreenSize.X/vScreenSize.Y, m_fZNear, m_fZFar);
			}
		}
		m_bProjectionMatrixDirty = false;
		return m_ProjectionMatrix;
	}

	//! update the view matrix
	const Matrix44& Light::UpdateViewMatrix()
	{
		const auto& dir = (m_LookAt->GetAbsolutePosition() - GetAbsolutePosition()).Normalized();
		m_ViewMatrix = Matrix44::MakeLookAtLH(GetAbsolutePosition(), dir, m_vUpVector);
		m_bViewMatrixDirty = false;
		return m_ViewMatrix;
	}

	//! updates the frustum
	const Frustum& Light::UpdateFrustum()
	{
		switch (_projection)
		{
		case Projection::Ortho3D:
			m_Frustum = Frustum(_orthoSize, m_fZNear, m_fZFar, m_vPosition, m_LookAt->GetAbsolutePosition(), m_vUpVector);
			break;

		case Projection::Perspective:
		default:
		{
			auto vScreenSize = GraphicsDriver::Instance()->GetViewPort().Size();
			m_Frustum = Frustum(m_FOV, vScreenSize.X / vScreenSize.Y, m_fZNear, m_fZFar, GetAbsolutePosition(), m_LookAt->GetAbsolutePosition(), m_vUpVector);
		}
		}

		m_bFrustumDirty = false;
		return m_Frustum;
	}

	//! called when the view has changed
	void Light::OnViewChanged()
	{
		m_bViewMatrixDirty = true;
		m_bViewProjectionMatrixDirty = true;
		m_bFrustumDirty = true;
	}
}

