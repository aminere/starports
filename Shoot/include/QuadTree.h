/* 

Amine Rehioui
Created: March 30th 2013

*/

#pragma once


#include "Frustum.h"
#include "Camera.h"
#include "EnvironmentCollisionComponent.h"

namespace shoot
{
	// forwards
	class QuadTree;
	class Context;

	//! QuadTreeNode
	class QuadTreeNode : public Object
	{		
	public:

		//! node type
		enum E_NodeType
		{
			T_XnYn,
			T_XpYn,
			T_XnYp,
			T_XpYp,
			T_Count
		};

		//! constructor
		QuadTreeNode();

		//! destructor
		~QuadTreeNode();

		//! builds the quad tree node
		void Build(const Vector2& vNodeSize);

		//! adds an entity to the node
		void AddEntity(Entity3D* pEntity3D);

		//! returns a child node
		inline QuadTreeNode* GetChild(E_NodeType type) const { return m_Nodes[type]; }

		//! sets the bounding box
		inline void SetBBox(const AABBox3D& bbox) { m_BBox = bbox; }
			
		//! returns the bounding box
		inline const AABBox3D& GetBBox() const { return m_BBox; }

		//! returns the entity list
		inline const std::list< Handle<Entity3D> >& GetEntityList() const { return m_EntityList; }

		//! Updates the node and its children
		void Update(QuadTree* pQuadTree);

		//! Registers the node and its children for rendering
		void RegisterForRendering(const Frustum& frustum, QuadTree* pQuadTree, bool bSkipFrustumTest = false);

		//! returns in frustum status
		inline bool IsInFrustum() const { return m_FrustumTestResult != Frustum::TR_Out; }

		//! returns true if the node is a leaf
		inline bool IsLeaf() const { return m_bLeaf; }

		//! returns the collision components
		inline std::list< Handle<EnvironmentCollisionComponent> >& GetCollisionComponents() { return m_CollisionComponents; }

	private:

		bool m_bLeaf;
		AABBox3D m_BBox;
		std::list< Handle<Entity3D> > m_EntityList;
		std::list< Handle<EnvironmentCollisionComponent> > m_CollisionComponents;
		Frustum::E_TestResult m_FrustumTestResult;

		Reference<QuadTreeNode> m_Nodes[T_Count];
	};

	//! QuadTree
	class QuadTree : public Object
	{
		DECLARE_OBJECT(QuadTree, Object);
	public:

		//! constructor
		QuadTree();

		//! destructor
		~QuadTree();

		//! serializes the object to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! builds the quad tree
		void Build(Context* pContext);

		//! returns the root node
		inline QuadTreeNode* GetRoot() const { return m_Root; }

		//! updates the QuadTree
		void Update();

		//! registers the QuadTree for rendering
		void RegisterForRendering();

		//! called when an entity is dynamically added
		void OnEntityAdded(Entity* pEntity);

		//! increments the node count
		inline void IncrNodeCount() { ++m_NodeCount; }

		//! increments the nodes drawn
		void IncrNodesDrawn(QuadTreeNode* pNode);

		//! returns the node count
		inline int GetNodeCount() const { return m_NodeCount; }

		//! returns the nodes drawn
		inline int GetNodesDrawn() const { return m_NodesDrawn; }

		//! sets the camera
		inline void SetCamera(Camera* pCamera) { m_Camera = pCamera; }

		//! returns the active nodes
		inline const std::list<QuadTreeNode*>& GetActiveNodes() const { return m_lActiveNodes; }

		//! recursively extracts 3D entities
		void ExtractEntities(Entity* pEntity);

		//! rebuilds the QuadTree
		void Rebuild() const;

		//! returns entity processed status
		inline bool GetEntityProcessed(Entity* pEntity) const { return m_ProcessedEntities.find(pEntity) != m_ProcessedEntities.end(); }

		//! set entity processed status
		inline void SetEntityProcessed(Entity* pEntity) { m_ProcessedEntities[pEntity] = 1; }

	private:

		Reference<QuadTreeNode> m_Root;
		Handle<Context> m_Context;
		std::list< Handle<Entity> > m_lAlwaysActive;
		std::list<QuadTreeNode*> m_lActiveNodes;
		std::map<Entity*, int> m_ProcessedEntities;
		Handle<Camera> m_Camera;

		int m_NodeCount;
		int m_NodesDrawn;

		// properties
		Vector2 m_vNodeSize;
	};
}



