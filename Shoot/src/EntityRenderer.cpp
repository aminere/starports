/* 

Amine Rehioui
Created: May 8th 2010

*/

#include "Shoot.h"

#include "EntityRenderer.h"
#include "EntityRendererFull.h"

#include "MaterialProvider.h"
#include "GeometryProvider.h"

namespace shoot
{
	EntityRenderer* EntityRenderer::m_spInstance = NULL;

	//! creates an instance
	void EntityRenderer::CreateInstance()
	{
		SHOOT_ASSERT(!m_spInstance, "Cannot create multiple EntityRenderer instances");
		m_spInstance = snew EntityRendererFull();
	}

	//! destroys the instance
	void EntityRenderer::DestroyInstance()
	{
		SHOOT_ASSERT(m_spInstance, "Cannot destroy NULL EntityRenderer instance");
		delete m_spInstance;
	}

	//! Constructor
	EntityRenderer::EntityRenderer()
		: m_bRenderViewField(true)
	{
	}

	//! Destructor
	EntityRenderer::~EntityRenderer()
	{
	}

	//! Initializes the renderer
	void EntityRenderer::Init()
	{
		if(!m_BlackBarsVB.IsValid())		
			m_BlackBarsVB = GeometryProvider::Instance()->Get2DQuad(Vector2::Create(1.0f, 1.0f), Vector2::One);		

		if(!m_ColorMaterial.IsValid())
		{
			Material::CreationInfo info;
			info.AddTexture("common/tex/WhiteBG.xml");
			m_ColorMaterial = MaterialProvider::Instance()->GetMaterial(info);
		}
	}

	//! called when the screen has been resized
	void EntityRenderer::OnResize()
	{
		if(Camera* p3DCamera = Get3DCamera())
			p3DCamera->SetProjectionMatrixDirty();

		if(Camera* p2DCamera = Get2DCamera())
		{
			p2DCamera->SetProjectionMatrixDirty();
			p2DCamera->SetViewMatrixDirty();
		}
	}

	//! changes the current 2D camera
	void EntityRenderer::Set2DCamera(Camera* pCamera)
	{ 
		m_2DCamera = pCamera;
		if (pCamera)
		{
			pCamera->SetProjectionMatrixDirty();
			pCamera->SetViewMatrixDirty();
		}
	}

	//! increment from a vertex buffer
	void EntityRenderer::Stats::Increment(VertexBuffer* pVB, int numInstances /*= 1*/)
	{
#ifndef _RETAIL_
		if(Enabled)
		{
			NumVertices += pVB->GetNumVertices();
			if(pVB->GetPrimitiveType() == GraphicsDriver::PT_Triangle)			
				NumTriangles += pVB->GetNumVertices()/3;
			++NumDrawCalls;
		}
#endif
	}

	//! increment from a material
	void EntityRenderer::Stats::Increment(Material* pMaterial)
	{
#ifndef _RETAIL_
		if(Enabled)		
			++NumMaterials;		
#endif
	}

	//! renders the black bars
	void EntityRenderer::RenderBlackBars()
	{
		if (!m_bRenderViewField)
			return;

		// render black bars
		float fDeltaOffset = m_2DCamera->Get2DOffset().X - m_2DCamera->Get2DOffset().Y;
		if(Math::FIsZero(fDeltaOffset))
			return;

		Vector2 vScreenSize = GraphicsDriver::Instance()->GetViewPort().Size();
		GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_View, Matrix44::Identity);
		m_ColorMaterial->Begin();
		m_ColorMaterial->GetShader()->GetConstant("uAmbientColor")->Set(&Color::Black, 1);
		m_ColorMaterial->GetShader()->Begin();
		m_BlackBarsVB->Begin(m_ColorMaterial->GetShader());
		if(fDeltaOffset > 0.0f)
		{
			float fBarWidth = m_2DCamera->Get2DOffset().X;
			Matrix44 transform = Matrix44::Identity;
			transform.SetScale(Vector3::Create(fBarWidth, vScreenSize.Y, 1.0f));
			GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, transform);
			m_BlackBarsVB->Draw();
			transform.SetTranslation(Vector3::Create(vScreenSize.X-fBarWidth, 0.0f, 0.0f));
			GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, transform);
			m_BlackBarsVB->Draw();
		}
		else
		{
			float fBarHeight = m_2DCamera->Get2DOffset().Y;
			Matrix44 transform = Matrix44::Identity;
			transform.SetScale(Vector3::Create(vScreenSize.X, fBarHeight, 1.0f));
			GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, transform);
			m_BlackBarsVB->Draw();
			transform.SetTranslation(Vector3::Create(0.0f, vScreenSize.Y-fBarHeight, 0.0f));
			GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, transform);
			m_BlackBarsVB->Draw();
		}
	}
}

