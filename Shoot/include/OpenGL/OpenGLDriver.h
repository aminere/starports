/* 

Amine Rehioui
Created: June 13th 2010

*/

#pragma once


namespace shoot
{
	//! OpenGL 3D driver
	class OpenGLDriver : public GraphicsDriver
	{
		typedef GraphicsDriver super;

	public:

		//! constructor
		OpenGLDriver();

		//! destructor
		virtual ~OpenGLDriver();

		//! driver initialization
		void Init();

		//! create a render target texture
		RenderTarget* CreateRenderTarget(const Vector2& vSize, int typeMask);

		//! sets a render target
		/** \param pTarget: render target texture. Pass NULL to select the regular back buffer */
		void SetRenderTarget(RenderTarget* pTarget);

		//! sets the viewport area
		void SetViewPort(const AABBox2D& area);

		//! returns the viewport area
		const AABBox2D& GetViewPort() const { return m_ViewPortArea; }

		//! Sets transformation matrices
		/** override with driver specific code */
		void SetTransform(E_TransformState eState, const Matrix44& matrix);

		//! Sets a render state
		void SetRenderState(E_RenderState eState, bool bEnable);

		//! Sets a render state
		void SetRenderState(E_RenderState eState, int iValue);

		//! Sets a render state
		void SetRenderState(E_RenderState eState, float fValue);

		//! Sets the blend function
		void SetBlendFunc(E_BlendFactor eSrc, E_BlendFactor eDest);

		//! Sets the stencil function
		void SetStencilFunc(E_ComparisonFunction func, int ref, int mask);

		//! Sets the stencil operation
		void SetStencilOps(E_StencilOp sFail, E_StencilOp sOkDepthFail, E_StencilOp sOkDepthOk);

		//! creates a texture from a path
		TextureImpl* CreateTexture();

		//! create a memory texture
		TextureImpl* CreateMemoryTexture();

		//! create a shader
		ShaderImpl* CreateShader();

		//! creates a cubemap texture
		Texture* CreateCubeMapTexture(const char* strPath[6]);

		//! create a vertex buffer
		VertexBuffer* CreateVertexBuffer(bool bInstancedRendering = false);

		//! create a skybox vertex buffer
		VertexBuffer* CreateSkyBoxVertexBufferNoCubemap(Texture* pTexture);

		//! clears graphic buffers
		/** \param mask: a combination of E_BufferFlag flags */
		void ClearBuffers(int mask, Color clearColor = Color::Black);

		//! sets the culling mode
		void SetCullMode(E_CullMode eMode);

		//! returns the driver type
		E_DriverType GetType() const { return DT_OpenGL; }

		//! DiscardDepthBuffer
		void DiscardFrameBuffers(bool depthOnly = true) { }

	private:

		AABBox2D m_ViewPortArea;
	};
}



