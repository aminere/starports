/* 

Amine Rehioui
Created: May 9th 2010

*/

#include "Shoot.h"

#include "Camera.h"

namespace shoot
{
	DEFINE_OBJECT(Camera);

	//! constructor
	Camera::Camera()
	: m_bSet(false)
	// properties
	, m_FOV(45.0f)
	, m_ZNear(0.5f)
	, m_ZFar(300.0f)
	, m_eType(Projection::Perspective)
	, m_bProjectionMatrixDirty(true)
	, m_bViewMatrixDirty(true)
	, m_bFrustumDirty(true)
	, m_TargetResolution(960, 640)
	, _orthoSize(Vector2::Create(2.0f, 2.0f))
	{
		m_vPosition = Vector3::Zero;
		m_vLookAt = m_vLookAtDirection = Vector3::Zero;
	}

	//! serializes the entity to/from a PropertyStream
	void Camera::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("FOV", &m_FOV);
		stream.Serialize("ZNear", &m_ZNear);
		stream.Serialize("ZFar", &m_ZFar);
		stream.Serialize("Projection", &m_eType, ENUM_PARAMS1(Projection::Literals));
		stream.Serialize("LookAt", &m_vLookAt);
		stream.Serialize("TargetResolution", &m_TargetResolution);
		stream.Serialize("OrthoSize", &_orthoSize);

		UpdateProjectionMatrix();
		UpdateViewMatrix();
		UpdateFrustum();
	}

	//! registers the entity and its children for rendering
	void Camera::RegisterForRendering()
	{
		if(m_bSet)
			return;
		
		switch(m_eType)
		{
		case Projection::Perspective:
		case Projection::Ortho3D:
			EntityRenderer::Instance()->Set3DCamera(this);
			break;

		case Projection::Ortho2D:
			EntityRenderer::Instance()->Set2DCamera(this);
			break;
		}
		m_bSet = true;
	}

	//! update the projection matrix
	const Matrix44& Camera::UpdateProjectionMatrix()
	{
		const Vector2& vScreenSize = GraphicsDriver::Instance()->GetViewPort().Size();
		switch(m_eType)
		{
		case Projection::Perspective:
			m_ProjectionMatrix = Matrix44::MakePerspectiveProjection(m_FOV, vScreenSize.X/vScreenSize.Y, m_ZNear, m_ZFar);
			break;

		case Projection::Ortho3D:
			m_ProjectionMatrix = Matrix44::Make3DOrthographicProjection(_orthoSize.X, _orthoSize.Y, m_ZNear, m_ZFar);
			break;

		case Projection::Ortho2D:
			m_ProjectionMatrix = Matrix44::Make2DOrthographicProjection(vScreenSize.X, vScreenSize.Y);
			break;
		}
		m_bProjectionMatrixDirty = false;
		return m_ProjectionMatrix;
	}

	//! update the view matrix
	const Matrix44& Camera::UpdateViewMatrix(bool bReturnInverse /*= false*/)
	{
		m_vLookAtDirection = (m_vLookAt - m_vPosition).Normalized();

		switch(m_eType)
		{
		case Projection::Perspective:
		case Projection::Ortho3D:
			m_ViewMatrix = Matrix44::MakeLookAtLH(m_vPosition, m_vLookAtDirection, Vector3::Up);
			break;

		case Projection::Ortho2D:
			// stretch to preserve m_TargetResolution Ratio
			Vector2 vTargetSize;
			Get2DRatioInfo(vTargetSize, m_v2DOffset);
			m_ViewMatrix = Matrix44::Identity;
			m_ViewMatrix.Scale(Vector3::Create(vTargetSize.X/m_TargetResolution.Width*m_vScale.X, vTargetSize.Y/m_TargetResolution.Height*m_vScale.Y, 1.0f));
			m_ViewMatrix.Translate(Vector3::Create(m_v2DOffset.X+m_vPosition.X, m_v2DOffset.Y+m_vPosition.Y, 0.0f));
			break;
		}
		m_ViewMatrix.GetInverse(m_ViewMatrixInverse);
		m_bViewMatrixDirty = false;
		return bReturnInverse ? m_ViewMatrixInverse : m_ViewMatrix;
	}

	//! updates the frustum
	const Frustum& Camera::UpdateFrustum()
	{
		switch(m_eType)
		{
		case Projection::Ortho3D:
			m_Frustum = Frustum(_orthoSize, m_ZNear, m_ZFar, m_vPosition, m_vLookAt, Vector3::Up);
			break;

		case Projection::Perspective:
		default:
			{
				auto vScreenSize = GraphicsDriver::Instance()->GetViewPort().Size();
				m_Frustum = Frustum(m_FOV, vScreenSize.X/vScreenSize.Y, m_ZNear, m_ZFar, m_vPosition, m_vLookAt, Vector3::Up);
			}
		}

		m_bFrustumDirty = false;
		return m_Frustum;
	}

	//! calculates a 3D ray from screen coordinates
	/** the ray is from the camera position to a far plane position pointed by the screen coordinates */
	void Camera::Get3DRayFromScreenPos(Vector3& rayStart, Vector3& rayDir, const Vector2& vScreenPos)
	{
		// uses ToyMaker's method - http://www.toymaker.info/Games/html/picking.html
		const Matrix44& projMatrix = GetProjectionMatrix();
		const Vector2& vScreenSize = GraphicsDriver::Instance()->GetViewPort().Size();

		Vector3 v;
		v.X =  ( ( ( 2.0f * vScreenPos.X ) / vScreenSize.X ) - 1.0f ) / projMatrix(0, 0);
		v.Y = -( ( ( 2.0f * vScreenPos.Y ) / vScreenSize.Y ) - 1.0f ) / projMatrix(1, 1);
		v.Z = -1.0f;

		const Matrix44& m = GetViewMatrixInverse();
		// Transform the screen space pick ray into 3D space
		rayDir.X  = v.X*m(0, 0) + v.Y*m(1, 0) + v.Z*m(2, 0);
		rayDir.Y  = v.X*m(0, 1) + v.Y*m(1, 1) + v.Z*m(2, 1);
		rayDir.Z  = v.X*m(0, 2) + v.Y*m(1, 2) + v.Z*m(2, 2);
		rayDir.Normalize();
		rayStart  = m_vPosition;
	}

	//! converts from world space to screen space
	bool Camera::GetScreenPos(const Vector3& vAbsWorldPos, Vector3& vScreenPos)
	{
		Vector3 vToTarget = (vAbsWorldPos-m_vPosition).Normalize();
		bool bInFront = (vToTarget.DotProduct(m_vLookAtDirection) > 0.0f);

		// TODO DX11 might need to inverse the matrix multiplication order because the row majority is different
		Vector4 vClipPos = (GetViewMatrix() * GetProjectionMatrix()) * Vector4::Create(vAbsWorldPos, 1.0f);
		Vector3 vClipPosNDC = vClipPos.XYZ/vClipPos.W;
		const Vector2& vScreenSize = GraphicsDriver::Instance()->GetViewPort().Size();
		// convert clipPosNDC.X[-1, 1] to screenPos.X[0, screenSize.X]
		// and clipPosNDC.Y[1, -1] to screenPos.Y[0, screenSize.Y ]
		vScreenPos = Vector3::Create((vClipPosNDC.X*0.5f + 0.5f) * vScreenSize.X, (-vClipPosNDC.Y*0.5f + 0.5f) * vScreenSize.Y, vClipPosNDC.Z);

		// Convert to 2D camera space
		if (Camera* p2DCamera = EntityRenderer::Instance()->Get2DCamera())
		{
			Vector2 vTargetSize, v2DOffset;
			p2DCamera->Get2DRatioInfo(vTargetSize, v2DOffset);
			Matrix44 _2DRatioTransform = Matrix44::Identity;
			_2DRatioTransform.Translate(-Vector3::Create(v2DOffset.X + p2DCamera->GetPosition().X, v2DOffset.Y + p2DCamera->GetPosition().Y, 0.0f));
			_2DRatioTransform.Scale(Vector3::One / Vector3::Create(vTargetSize.X / p2DCamera->GetTargetResolution().Width*p2DCamera->GetScale().X, vTargetSize.Y / p2DCamera->GetTargetResolution().Height*p2DCamera->GetScale().Y, 1.0f));
			vScreenPos = _2DRatioTransform.TransformVect(vScreenPos);
		}
		return bInFront;
	}

	//! copies another camera's configuration
	void Camera::CopyFrom(Camera* pOther)
	{
		PropertyStream stream(SM_Write, this);
		pOther->Serialize(stream);
		stream.SetMode(SM_Read);
		Serialize(stream);
	}

	//! calculates a 2D size and offset preserving the ratio of the target resolution
	void Camera::Get2DRatioInfo(Vector2& vTargetSize, Vector2& vOffset)
	{
		Vector2 vScreenSize = GraphicsDriver::Instance()->GetViewPort().Size();
		float screenRatio = vScreenSize.X/vScreenSize.Y;
		float targetRatio = float(m_TargetResolution.Width)/m_TargetResolution.Height;
		vTargetSize = vScreenSize;
		vOffset = Vector2::Zero;
		if(screenRatio > targetRatio)
		{
			vTargetSize.X = vTargetSize.Y * targetRatio;
			vOffset.X = (vScreenSize.X-vTargetSize.X)/2.0f;
		}
		else
		{
			vTargetSize.Y = vTargetSize.X / targetRatio;
			vOffset.Y = (vScreenSize.Y-vTargetSize.Y)/2.0f;
		}
	}
}

