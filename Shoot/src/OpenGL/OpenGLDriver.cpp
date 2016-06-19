/* 

Amine Rehioui
Created: July 10th 2010

*/

#include "Shoot.h"

#include "OpenGLDriver.h"
#include "OpenGLTexture.h"
#include "OpenGLShader.h"
#include "SkyBoxVertexBufferNoCubemap.h"
#include "FakeCubeMapTexture.h"
#include "OpenGLRenderTarget.h"
#include "OpenGLVertexBuffer.h"
#include "OpenGLVertexBufferNoVAO.h"
#include "SkyBoxVertexBufferNoCubemap_NoVAO.h"
#include "OpenGLVertexBufferInstanced.h"
#include "OpenGLMemoryTexture.h"

#include "OpenGLExtensionHandler.h"

#include "VertexBuffer.h"

namespace shoot
{
	static int glCompFuncs[] = { GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, GL_ALWAYS };
	static int glStecilOps[] = { GL_KEEP, GL_ZERO, GL_REPLACE, GL_INCR, GL_INCR_WRAP, GL_DECR, GL_DECR_WRAP, GL_INVERT };

	//! constructor
	OpenGLDriver::OpenGLDriver()
	{
	}

	//! destructor
	OpenGLDriver::~OpenGLDriver()
	{
	}

	//! driver initialization
	void OpenGLDriver::Init()
	{
		super::Init();

		SetCullMode(CM_CounterClockWise);
		SetRenderState(GraphicsDriver::RS_DepthFunc, GraphicsDriver::CF_Less);
		GL_CHECK(glEnable(GL_TEXTURE_2D));
		glClearDepth(1.0f);
		glClearStencil(0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}

	//! create a render target texture
	RenderTarget* OpenGLDriver::CreateRenderTarget(const Vector2& vSize, int typeMask)
	{
		return snew OpenGLRenderTarget(vSize, typeMask);
	}

	//! sets a render target
	/** \param pTarget: render target texture. Pass NULL to select the regular back buffer */
	void OpenGLDriver::SetRenderTarget(RenderTarget* pTarget)
	{
		if(pTarget)
			pTarget->Bind();
		else
		{
			OpenGLExtensionHandler::Instance()->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
			GraphicsDriver::Instance()->SetViewPort(GraphicsDriver::Instance()->GetViewPort());
			GraphicsDriver::Instance()->ClearBuffers(GraphicsDriver::BF_ColorBuffer | GraphicsDriver::BF_DepthBuffer);
		}
	}

	//! sets the viewport area
	void OpenGLDriver::SetViewPort(const AABBox2D& area)
	{
		GL_CHECK(glViewport(int(area.Min().X), int(area.Min().Y), int(area.Max().X), int(area.Max().Y)));
		m_ViewPortArea = area;
	}

	//! Sets transformation matrices
	/** override with driver specific code */
	void OpenGLDriver::SetTransform(E_TransformState eState, const Matrix44& matrix)
	{
		super::SetTransform(eState, matrix); 

		switch(eState)
		{
		case TS_Projection:
		case TS_View:
			m_StateMatrices[TS_ViewProjection] = m_StateMatrices[TS_View] * m_StateMatrices[TS_Projection];
			break;
		}
	}

	//! Sets a render state
	void OpenGLDriver::SetRenderState(E_RenderState eState, bool bEnable)
	{
		switch(eState)
		{
		case RS_DepthTesting: 
			bEnable ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
			SHOOT_LOG_WARNING(glGetError() == GL_NO_ERROR, "GL_DEPTH_TEST change failed");
			break;

		case RS_DepthWriting:
			GL_CHECK(glDepthMask(bEnable));
			break;

		case RS_AlphaBlending:
			bEnable ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
			SHOOT_LOG_WARNING(glGetError() == GL_NO_ERROR, "GL_BLEND change failed");
			break;

		case RS_StencilTesting:
			bEnable ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
			SHOOT_LOG_WARNING(glGetError() == GL_NO_ERROR, "GL_STENCIL_TEST change failed");
			break;

		case RS_ColorWriting:
			GL_CHECK(glColorMask(bEnable, bEnable, bEnable, bEnable));
			break;

		default:
			SHOOT_WARNING(0, "SetRenderState could not handle state");
		}
	}

	//! Sets a render state
	void OpenGLDriver::SetRenderState(E_RenderState eState, int iValue)
	{
		static int glFillModes[] = { GL_POINT, GL_LINE, GL_FILL };

		switch(eState)
		{
		case RS_DepthFunc:
			{
				SHOOT_ASSERT(iValue < CF_Count, "Invalid E_ComparisonFunction");
				int func = glCompFuncs[iValue];
				GL_CHECK(glDepthFunc(func));
			}
			break;

		case RS_FillMode:
			{
				SHOOT_ASSERT(iValue < FM_Count, "Invalid E_FillMode");
				GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, glFillModes[iValue]));
			}
			break;

		case RS_StencilWriting:
			GL_CHECK(glStencilMask(iValue));
			break;

		default:
			SHOOT_WARNING(0, "SetRenderState could not handle state");
		}
	}
	
	//! Sets a render state
	void OpenGLDriver::SetRenderState(E_RenderState eState, float fValue)
	{
		switch(eState)
		{
		case RS_LineWidth:
			glLineWidth(fValue);
			break;

		default:
			SHOOT_WARNING(0, "SetRenderState could not handle state");
		}
	}

	//! Sets the blend function
	void OpenGLDriver::SetBlendFunc(E_BlendFactor eSrc, E_BlendFactor eDest)
	{
		static int glBlendFactors[] = { GL_ZERO, 
										GL_ONE, 
										GL_SRC_COLOR, 
										GL_ONE_MINUS_SRC_COLOR, 
										GL_SRC_ALPHA, 
										GL_ONE_MINUS_SRC_ALPHA,
										GL_DST_ALPHA, 
										GL_ONE_MINUS_DST_ALPHA, 
										GL_DST_COLOR, 
										GL_ONE_MINUS_DST_COLOR, 
										GL_SRC_ALPHA_SATURATE };

		GL_CHECK(glBlendFunc(glBlendFactors[eSrc], glBlendFactors[eDest]));
	}

	//! Sets the stencil function
	void OpenGLDriver::SetStencilFunc(E_ComparisonFunction func, int ref, int mask)
	{
		GL_CHECK(glStencilFunc(glCompFuncs[func], ref, mask));
	}
	
	//! Sets the stencil operation
	void OpenGLDriver::SetStencilOps(E_StencilOp sFail, E_StencilOp sOkDepthFail, E_StencilOp sOkDepthOk)
	{
		GL_CHECK(glStencilOp(glStecilOps[sFail], glStecilOps[sOkDepthFail], glStecilOps[sOkDepthOk]));
	}

	//! creates a texture from a path
	TextureImpl* OpenGLDriver::CreateTexture()
	{
		return snew OpenGLTexture();
	}

	//! create a memory texture
	TextureImpl* OpenGLDriver::CreateMemoryTexture()
	{
		return snew OpenGLMemoryTexture();
	}

	//! create a shader
	ShaderImpl* OpenGLDriver::CreateShader()
	{
		return snew OpenGLShader();
	}

	//! creates a cubemap texture
	Texture* OpenGLDriver::CreateCubeMapTexture(const char* strPath[6])
	{
		return snew FakeCubeMapTexture(strPath);
	}

	//! create a vertex buffer
	VertexBuffer* OpenGLDriver::CreateVertexBuffer(bool bInstancedRendering /*= false*/)
	{
		if (OpenGLExtensionHandler::Instance()->HasExtension(OpenGLExtensionHandler::E_VAO))
		{
			if (bInstancedRendering)
				return snew OpenGLVertexBufferInstanced();
			else
				return snew OpenGLVertexBuffer();
		}
		else
		{
			if (bInstancedRendering)
			{
				SHOOT_ASSERT(false, "TODO unsupported vertex buffer");
				return NULL;
				//return snew OpenGLVertexBufferInstancedNoVAO();
			}
			else
				return snew OpenGLVertexBufferNoVAO();
		}
	}

	//! create a skybox vertex buffer
	VertexBuffer* OpenGLDriver::CreateSkyBoxVertexBufferNoCubemap(Texture* pTexture)
	{
		if (OpenGLExtensionHandler::Instance()->HasExtension(OpenGLExtensionHandler::E_VAO))
		{
			return snew SkyBoxVertexBufferNoCubemap(static_cast<FakeCubeMapTexture*>(pTexture));
		}
		else
		{
			return snew SkyBoxVertexBufferNoCubemap_NoVAO(static_cast<FakeCubeMapTexture*>(pTexture));
		}
	}

	//! clears graphic buffers
	/** \param mask: a combination of E_BufferFlag flags */
	void OpenGLDriver::ClearBuffers(int mask, Color clearColor /*= Color()*/)
	{
		GLbitfield glMask = 0;

		if((mask & BF_ColorBuffer) != 0)
		{
			glMask |= GL_COLOR_BUFFER_BIT;
		}

		if((mask & BF_DepthBuffer) != 0)
		{
			glMask |= GL_DEPTH_BUFFER_BIT;
		}

		if((mask & BF_StencilBuffer) != 0)
		{
			glMask |= GL_STENCIL_BUFFER_BIT;
		}
		
		GL_CHECK(glClear(glMask));
	}

	//! sets the culling mode
	void OpenGLDriver::SetCullMode(E_CullMode eMode)
	{
		switch(eMode)
		{
		case CM_ClockWise:
			GL_CHECK(glFrontFace(GL_CCW));
			GL_CHECK(glCullFace(GL_BACK));
			glEnable(GL_CULL_FACE);
			break;

		case CM_CounterClockWise:
			GL_CHECK(glFrontFace(GL_CW));
			GL_CHECK(glCullFace(GL_BACK));
			glEnable(GL_CULL_FACE);
			break;

		case CM_None:
			glDisable(GL_CULL_FACE);
			break;

		default:
			SHOOT_WARNING(0, "SetCullMode could not handle mode");
		}
	}
}

