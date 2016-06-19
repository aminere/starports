/* 

Amine Rehioui
Created: May 8th 2010

*/

#pragma once


#include "RenderableEntity.h"

#include "Camera.h"

namespace shoot
{
	class Effect;

	//! EntityRenderer class	
	class EntityRenderer : public Object
	{
	public:

		//! rendering stats
		struct Stats
		{
			Stats()
				: NumDrawCalls(0)
				, NumMaterials(0)
				, NumVertices(0)
				, NumTriangles(0)
				, Enabled(true)
			{
			}

			int NumDrawCalls;
			int NumMaterials;
			int NumVertices;
			int NumTriangles;
			bool Enabled;

			//! increment from a vertex buffer
			void Increment(VertexBuffer* pVB, int numInstances = 1);

			//! increment from a material
			void Increment(Material* pMaterial);
		};

		//! Constructor
		EntityRenderer();
		
		//! Destructor
		virtual ~EntityRenderer();

		//! creates a renderer
		static void CreateInstance();

		//! destroys the renderer
		static void DestroyInstance();

		//! returns the instance
		static EntityRenderer* Instance() { return m_spInstance; }

		//! Initializes the renderer
		virtual void Init();

		//! called when the screen has been resized
		virtual void OnResize();

		//! registers the entity for rendering
		virtual bool RegisterEntity(RenderableEntity* pEntity) = 0;

		//! register an effect for rendering
		virtual void RegisterEffect(Effect* pEffect) = 0;

		//! registers the entity for rendering
		virtual void RegisterDebugEntity(RenderableEntity* pEntity) = 0;

		//! clears the rendering lists
		virtual void Clear() = 0;

		//! renders the entities
		virtual void Render() = 0;

		//! change the current 3D camera
		virtual void Set3DCamera(Camera* pCamera) { m_3DCamera = pCamera; }

		//! changes the current 2D camera
		void Set2DCamera(Camera* pCamera);
	
		//! returns the current 3D camera
		inline Camera* Get3DCamera() { return m_3DCamera; }

		//! returns the current 2D camera
		inline  Camera* Get2DCamera()	{ return m_2DCamera; }

		//! returns rendering stats
		inline  Stats& GetStats() { return m_Stats; }

		//! sets view field handling status
		inline void SetRenderViewField(bool bRender) { m_bRenderViewField = bRender; }

		//! gets view field handling status
		inline bool GetRenderViewField() const { return m_bRenderViewField; }

		//! renders the black bars
		void RenderBlackBars();

		//! RenderToTexture
		struct RenderToTexture
		{
			RenderToTexture() { m_spInstance->EnableRenderToTexture(true); }
			~RenderToTexture() { m_spInstance->EnableRenderToTexture(false); }
		};

		//! Gets a texture renderer
		virtual EntityRenderer* GetTextureRenderer() = 0;

		//! GetFinalRenderTarget
		virtual RenderTarget* GetFinalRenderTarget() const = 0;

	protected:

		Handle<Camera> m_3DCamera;
		Handle<Camera> m_2DCamera;

		Stats m_Stats;
		bool m_bRenderViewField;
		Reference<VertexBuffer> m_BlackBarsVB;
		Reference<Material> m_ColorMaterial;

		//! RenderToTexture
		virtual void EnableRenderToTexture(bool enable) = 0;

	private:

		static EntityRenderer* m_spInstance;
	};
}




