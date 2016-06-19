/* 

Amine Rehioui
Created: June 13th 2010

*/

#pragma once


#include "Color.h"
#include "AABBox2D.h"
#include "AABBox3D.h"
#include "Material.h"
#include "Vertex3D.h"
#include "MemoryTexture.h"

namespace shoot
{
	// forwards
	class VertexBuffer;
	class RenderTarget;

	//! base 3D graphics driver interface	
	class GraphicsDriver
	{
	public:

		//! driver types
		enum E_DriverType
		{
			DT_OpenGL,
			DT_OpenGLES2,
			DT_DirectX11,
			DT_NULL,
			DT_Count
		};

		//! Constructor
		GraphicsDriver();

		//! Destructor
		virtual ~GraphicsDriver();

		//! creates a driver - the singleton pattern was not used because GraphicsDriver has pure virtual methods
		static void CreateInstance();

		//! destroys the driver
		static void DestroyInstance();

		//! returns the instance of the graphics driver
		inline static GraphicsDriver* Instance() { return m_spInstance; }

		//! transformation states
		enum E_TransformState
		{
			TS_Projection,			
			TS_View,
			TS_ViewProjection,
			TS_World,
			TS_Texture,
			TS_Count
		};

		//! render states
		enum E_RenderState
		{
			RS_DepthTesting,
			RS_DepthWriting,
			RS_AlphaBlending,
			RS_DepthFunc, // Uses E_ComparisonFunction enumeration
			RS_SrcBlend,
			RS_DestBlend,
			RS_FillMode,
			RS_LineWidth,
			RS_StencilTesting,
			RS_StencilWriting,
			RS_ColorWriting
		};

		//! buffer flags
		enum E_BufferFlag
		{
			BF_ColorBuffer =	1 << 0,
			BF_DepthBuffer =	1 << 1,
			BF_StencilBuffer =	1 << 2,
			BF_EnableWrap =		1 << 3,
		};

		//! comparison functions
		enum E_ComparisonFunction
		{			
			CF_Never,			
			CF_Less,
			CF_Equal,
			CF_LessOrEqual,
			CF_Greater,
			CF_NotEqual,
			CF_GreaterOrEqual,
			CF_Always,
			CF_Count
		};

		//! blending factor
		enum E_BlendFactor
		{
			BF_Zero,
			BF_One,			
			BF_SrcColor,
			BF_InvSrcColor,
			BF_SrcAlpha,
			BF_InvSrcAlpha,
			BF_DestAlpha,
			BF_InvDestAlpha,
			BF_DestColor,
			BF_InvDestColor,
			BF_SrcAlphaSaturate,
			BF_Count
		};

		//! primitive type
		enum E_PrimitiveType
		{
			PT_Point,
			PT_Line,
			PT_LineLoop,
			PT_LineStrip,
			PT_Triangle,
			PT_TriangleStrip,
			PT_TriangleFan
		};

		//! face culling modes
		enum E_CullMode
		{
			CM_ClockWise,
			CM_CounterClockWise,
			CM_None
		};

		//! polygon fill mode
		enum E_FillMode
		{
			FM_Point,
			FM_Line,
			FM_Solid,
			FM_Count
		};

		//! Stencil ops
		enum E_StencilOp
		{
			SO_Keep,
			SO_Zero,
			SO_Replace,
			SO_Incr,
			SO_IncrWrap,
			SO_Decr,
			SO_DecrWrap,
			SO_Invert
		};

		//! driver initialization
		virtual void Init();

		//! create a render target texture
		virtual RenderTarget* CreateRenderTarget(const Vector2& vSize, int typeMask) = 0;

		//! sets a render target
		/** \param pTarget: render target texture. Pass NULL to select the regular back buffer */
		virtual void SetRenderTarget(RenderTarget* pTarget) = 0;
		
		//! sets the viewport area
		virtual void SetViewPort(const AABBox2D& area) = 0;

		//! resizes the screen surface
		virtual void ResizeScreen(const Vector2& vSize);

		//! returns the viewport area
		virtual const AABBox2D& GetViewPort() const = 0;

		//! Sets transformation matrices
		/** override with driver specific code */
		virtual void SetTransform(E_TransformState eState, const Matrix44& matrix) { m_StateMatrices[eState] = matrix; }

		//! Returns a transformation matrix		
		const Matrix44& GetTransform(E_TransformState eState) const { return m_StateMatrices[eState]; }

		//! Sets a render state
		virtual void SetRenderState(E_RenderState eState, bool bEnable) = 0;

		//! Sets a render state
		virtual void SetRenderState(E_RenderState eState, int iValue) = 0;

		//! Sets a render state
		virtual void SetRenderState(E_RenderState eState, float fValue) = 0;

		//! Sets the blend function
		virtual void SetBlendFunc(E_BlendFactor eSrc, E_BlendFactor eDest) = 0;

		//! Sets the stencil function
		virtual void SetStencilFunc(E_ComparisonFunction func, int ref, int mask) = 0;

		//! Sets the stencil operation
		virtual void SetStencilOps(E_StencilOp sFail, E_StencilOp sOkDepthFail, E_StencilOp sOkDepthOk) = 0;
		
		//! creates a texture from a path
		virtual TextureImpl* CreateTexture() = 0;

		//! create a memory texture
		virtual TextureImpl* CreateMemoryTexture() = 0;

		//! create a shader
		virtual ShaderImpl* CreateShader() = 0;

		//! creates a cubemap texture
		virtual Texture* CreateCubeMapTexture(const char* strPath[6]) = 0;

		//! create a vertex buffer
		virtual VertexBuffer* CreateVertexBuffer(bool bInstancedRendering = false) = 0;

		//! create a skybox vertex buffer
		virtual VertexBuffer* CreateSkyBoxVertexBufferNoCubemap(Texture* pTexture) = 0;
				
		//! clears graphic buffers
		/** \param mask: a combination of E_BufferFlag flags */
		virtual void ClearBuffers(int mask, Color clearColor = Color::Black) = 0;

		//! sets the culling mode
		virtual void SetCullMode(E_CullMode eMode) = 0;

		//! returns the driver type
		virtual E_DriverType GetType() const = 0;

		//! DiscardDepthBuffer
		virtual void DiscardFrameBuffers(bool depthOnly = true) = 0;

		//! presents the graphics
		virtual void Present() { }

		//! registers a graphic object
		void RegisterObject(GraphicObject* pObject);

		//! unregister a graphic object
		void UnregisterObject(GraphicObject* pObject);

		//! unloads graphic objects
		void UnloadObjects();

		//! loads graphic objects
		void LoadObjects();		

	protected:

		Matrix44 m_StateMatrices[TS_Count];

		static GraphicsDriver* m_spInstance;

	private:

		typedef std::map<GraphicObject*, int> GraphicObjectMap;
		GraphicObjectMap m_GraphicObjectMap;
	};

	//! null GraphicsDriver
	class NullGraphicsDriver : public GraphicsDriver
	{
	public:
		
		//! begins taking graphic commands
		void Begin(bool bClearBackBuffer = true, bool bClearDepthBuffer = true, Color clearColor = Color::Black) { }

		//! create a render target texture
		RenderTarget* CreateRenderTarget(const Vector2& size, int typeMask) { return NULL; }

		//! sets a render target
		/** \param pTarget: render target texture. Pass NULL to select the regular back buffer */
		void SetRenderTarget(RenderTarget* pTarget) { }

		//! sets the viewport area
		void SetViewPort(const AABBox2D& area) { }

		//! resizes the screen surface
		void ResizeScreen(const Vector2& vSize) { }

		//! returns the viewport area
		const AABBox2D& GetViewPort() const { return m_ViewPortArea; }

		//! Sets transformation matrices
		/** override with driver specific code */
		void SetTransform(E_TransformState eState, const Matrix44& matrix) { }

		//! Sets a render state
		void SetRenderState(E_RenderState eState, bool bEnable) { }

		//! Sets a render state
		void SetRenderState(E_RenderState eState, int iValue) { }

		//! Sets a render state
		void SetRenderState(E_RenderState eState, float fValue) { }

		//! Sets the blend function
		void SetBlendFunc(E_BlendFactor eSrc, E_BlendFactor eDest) { }

		//! creates a texture from a path
		TextureImpl* CreateTexture() { return snew NULLTextureImpl(); }

		//! create a memory texture
		TextureImpl* CreateMemoryTexture() { return snew NULLTextureImpl(); }

		//! create a shader
		ShaderImpl* CreateShader() { return snew NULLShaderImpl(); }

		//! creates a cubemap texture
		Texture* CreateCubeMapTexture(const char* strPath[6]) { return snew Texture(); }

		//! create a vertex buffer
		VertexBuffer* CreateVertexBuffer(bool bInstancedRendering = false);

		//! create a skybox vertex buffer
		VertexBuffer* CreateSkyBoxVertexBufferNoCubemap(Texture* pTexture);

		//! clears graphic buffers
		/** \param mask: a combination of E_BufferFlag flags */
		void ClearBuffers(int mask, Color clearColor = Color::Black) { }

		//! sets the culling mode
		void SetCullMode(E_CullMode eMode) { }

		//! returns the driver type
		E_DriverType GetType() const { return DT_NULL; }

		//! DiscardDepthBuffer
		void DiscardFrameBuffers(bool depthOnly = true) { }

	private:

		AABBox2D m_ViewPortArea;
	};
}



