/* 

Amine Rehioui
Created: July 6th 2013

*/

#ifndef _GRAPHIC_EXTENSION_HANDLER_H_INCLUDED_
#define _GRAPHIC_EXTENSION_HANDLER_H_INCLUDED_

namespace shoot
{
	//! GraphicExtensionHandler
	class GraphicExtensionHandler
	{
	public:

		//! CreateInstance
		static void CreateInstance();

		//! DestroyInstance
		static void DestroyInstance();
		
		//! returns true if the instance was created
		static bool IsCreated() { return m_spInstance != 0; }

		//! returns the instance of the graphics driver
		inline static GraphicExtensionHandler* Instance() { return m_spInstance; }

		//! extensions
		enum E_Extension
		{
			E_FBO, // frame buffer object
			E_MultiTexture,
			E_CubeMap,
			E_PointParams,
			E_VBO,
			E_VAO,
			E_VertexShader,
			E_FragmentShader,
			E_RectangleTexture,
			E_Instancing,
			E_DepthTexture,
			E_PackedDepthStencil,
			E_Count
		};
		
		//! shader types
		enum E_ShaderType
		{
			ST_Vertex,
			ST_Fragment,
			ST_Count
		};

		//! returns if an extensiion is available
		inline bool HasExtension(E_Extension eExt) const { return m_bHasExtension[eExt]; }

	protected:

		bool m_bHasExtension[E_Count];

		//! constructor
		GraphicExtensionHandler();

		//! destructor
		virtual ~GraphicExtensionHandler()
		{
		}

		static GraphicExtensionHandler* m_spInstance;
	};
}

#endif // _GRAPHIC_EXTENSION_HANDLER_H_INCLUDED_

