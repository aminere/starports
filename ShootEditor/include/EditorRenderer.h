/* 

Amine Rehioui
Created: August 20th 2011

*/

#pragma once


#include "MeshResource.h"

#include "BillBoardEntity.h"

#include "QuadTree.h"

namespace shoot
{
	// forwards
	class Entity2D;
	class Entity3D;
	class Path;
	class CollisionComponent;

	//! Editor renderer
	class EditorRenderer : public Singleton<EditorRenderer>
	{
		DECLARE_SINGLETON(EditorRenderer);

	public:

		//! Init
		virtual void Init();

		//! performs editor rendering
		void Render();

		//! sets enabled status
		void SetEnabled(bool bEnabled) { m_bEnabled = bEnabled; }

		//! set picking info
		void SetPickingInfo(const Vector3& vRayStart, const Vector3& vRayDir, const Vector3& vIntersection);

		//! set picking status
		inline void SetPickingInfoValid(bool bValid) { m_bPickingInfoValid = bValid; }

		//! get pick intersection
		inline const Vector3& GetPickIntersection() const { return m_vPickIntersection; }

		//! renders a line
		void DrawLine(const Vector3& vStart, const Vector3& vEnd, const Color& color);

		//! renders a debug box
		void DrawDebugBox(const AABBox3D& box, const Color& color);

		//! renders a debug box
		void DrawDebugBox(const AABBox2D& box, const Color& color);

		//! renders a cross
		void DrawCross(const Vector3& vOffset, float fSize, const Color& color);

	protected:

		//! extracts entities for rendering
		void ExtractEntities(Entity* pEntity);

		//! registers an entity for post rendering
		virtual void RegisterEntity(Entity* pEntity);

		//! custom Post 3D rendering
		virtual void RenderPost3D();

		//! custom Post 2D rendering
		virtual void RenderPost2D();

		//! Renders collision shapes
		void RenderCollisionShapes(CollisionComponent* pCollisionComponent);

		//! Render path
		void RenderPost3DPath(Path* pPath);

		//! Render quad tree nodes
		void RenderQuadTreeNodes(QuadTreeNode* pNode);

		//! Renders visible quad tree nodes
		void RenderVisibleQuadTreeNodes(QuadTreeNode* pNode);

		//! Renders a quad tree node
		void RenderQuadTreeNode(QuadTreeNode* pNode, const Color& color);

		//! Renders a camera frustum
		void RenderFrustum(const Frustum& frustum);

		// lists for post rendering passes
		std::vector<Entity2D*> m_2DEntities;
		std::vector<Entity3D*> m_3DEntities;
		std::vector<Path*> m_aPaths;

		// debug icons
		typedef std::map<int, Reference<Texture> > DebugIconMap;
		DebugIconMap m_DebugIconMap;
		Reference<BillBoardEntity> m_DebugIconEntity;

		Reference<MeshResource> m_UnitSphere;
		Reference<VertexBuffer> m_GridVertexBuffer;
		Reference<Texture> m_PinTexture;

		bool m_bEnabled;
		bool m_bPickingInfoValid;
		Vector3 m_vPickStart, m_vPickEnd, m_vPickIntersection;

		// debug rendering
		Reference<Material> m_DebugMaterial;
		Reference<VertexBuffer> m_DebugVB;
		int m_MaxDebugVertices;
	};
}



