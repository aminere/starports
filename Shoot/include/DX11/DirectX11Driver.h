/* 

Amine Rehioui
Created: July 27th 2013

*/

#pragma once

namespace shoot
{
	//! OpenGL 3D driver
	class DirectX11Driver : public GraphicsDriver
	{
		typedef GraphicsDriver super;

	public:

		//! constructor
		DirectX11Driver();

		//! destructor
		virtual ~DirectX11Driver();

		//! returns the instance of the graphics driver
		inline static DirectX11Driver* Instance() { return static_cast<DirectX11Driver*>(m_spInstance); }

		//! driver initialization
		void Init();

		//! begins taking graphic commands
		void Begin(bool bClearBackBuffer = true, bool bClearDepthBuffer = true, Color clearColor = Color::Black);

		//! create a render target texture
		RenderTarget* CreateRenderTarget(const Vector2& vSize, int typeMask);

		//! sets a render target
		/** \param pTarget: render target texture. Pass NULL to select the regular back buffer */
		void SetRenderTarget(RenderTarget* pTarget);

		//! sets the viewport area
		void SetViewPort(const AABBox2D& area);

		//! resizes the screen surface
		void ResizeScreen(const Vector2& vSize);

		//! returns the viewport area
		const AABBox2D& GetViewPort() const { return m_ViewPortArea; }

		//! Sets transformation matrices
		/** override with driver specific code */
		void SetTransform(E_TransformState eState, const Matrix44& matrix);

		//! Sets a render state
		void SetRenderState(E_RenderState eState, bool bEnable);

		//! Sets a render state
		void SetRenderState(E_RenderState eState, int iValue);

		//! Sets the blend function
		void SetBlendFunc(E_BlendFactor eSrc, E_BlendFactor eDest);

		//! creates a texture from a path
		TextureImpl* CreateTexture();

		//! create a shader
		ShaderImpl* CreateShader();

		//! creates a cubemap texture
		Texture* CreateCubeMapTexture(const char* strPath[6]);

		//! create a vertex buffer
		VertexBuffer* CreateVertexBuffer(bool bInstancedRendering = false);

		//! create a skybox vertex buffer
		VertexBuffer* CreateSkyBoxVertexBufferNoCubemap(Texture* pTexture);

		//! draws using a vertex buffer
		void Draw(int numVertices, int startVertex);

		//! draws using an index buffer
		void DrawIndexed(int numIndices, int startIndex, int startVertex);

		//! clears graphic buffers
		/** \param mask: a combination of E_BufferFlag flags */
		void ClearBuffers(int mask, Color clearColor = Color::Black);

		//! sets the culling mode
		void SetCullMode(E_CullMode eMode);

		//! presents the graphics
		void Present();

		//! sets the window handle
		inline void SetWindow(HWND hWindow) { m_hWindow = hWindow; }

		//! returns the device
		inline ID3D11Device1* GetDevice() const { return m_Device; }

		//! returns the immediate context
		inline ID3D11DeviceContext1* GetContext() const { return m_ImmediateContext; }

		//! binds material states
		void BindMaterialStates();

		//! registers a COM reference
		void RegisterReference(IUnknown* pObject);

		//! unregisters a COM reference
		void UnregisterReference(IUnknown* pObject);
				
		//! returns the driver type
		E_DriverType GetType() const { return DT_DirectX11; }

	private:
		
		std::map<IUnknown*, int> m_COMReferences;

		AABBox2D m_ViewPortArea;
		HWND m_hWindow;

		COMReference<IDXGISwapChain1> m_SwapChain;
		COMReference<ID3D11RenderTargetView> m_RenderTargetView;
		COMReference<ID3D11DepthStencilView> m_DepthStencilView;
		COMReference<ID3D11Buffer> m_MVPBuffer;
		COMReference<ID3D11Buffer> m_TextureTransformBuffer;
		COMReference<ID3D11Buffer> m_MaterialBuffer;
		COMReference<ID3D11SamplerState> m_TextureSamplerLinear;
		COMReference<ID3D11BlendState> m_BlendState[2][D3D11_BLEND_INV_SRC1_ALPHA+1][D3D11_BLEND_INV_SRC1_ALPHA+1]; // blendEnable / srcBlend / destBlend
		COMReference<ID3D11DepthStencilState> m_DepthStencilState[2][2][D3D11_COMPARISON_ALWAYS+1]; // testEnable / writeEnable / depthFunc
		COMReference<ID3D11RasterizerState> m_RasterizerState[2][D3D11_FILL_SOLID+1][D3D11_CULL_BACK+1]; // frontCCW / fillMode / cullMode
		COMReference<ID3D11DeviceContext1> m_ImmediateContext;
		COMReference<ID3D11Device1> m_Device;
		D3D11_BLEND_DESC m_BlendDesc;
		D3D11_DEPTH_STENCIL_DESC m_DSDesc;
		D3D11_RASTERIZER_DESC  m_RSDesc;
		D3D_FEATURE_LEVEL m_FeatureLevel;
	};
}

