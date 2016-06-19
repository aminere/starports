/* 

Amine Rehioui
Created: December 10th 2010

*/

#pragma once


#include <functional>

namespace shoot
{
	//! vertex buffer
	class VertexBuffer : public Object, public GraphicObject
	{
		DECLARE_OBJECT(VertexBuffer, Object);

	public:

		//! E_AttributeDataType
		enum E_AttributeDataType
		{
			ADT_Float,
			ADT_U32,
			ADT_Count
		};

		//! AttributeInfo
		struct AttributeInfo
		{
			std::string m_Name;
			size_t m_Offset;
			E_AttributeDataType m_DataType;
			int m_DataCount;
		};

		//! constructor
		VertexBuffer();

		//! destructor
		virtual ~VertexBuffer();

		//! begins rendering using this vertex buffer
		virtual void Begin(Shader* pShader);

		//! render method
		virtual void Draw(const std::vector<Matrix44>& aWorldTransforms, const std::vector<Matrix44>& aTextureTransforms) { }
		
		//! returns a pointer to the vertices
		inline void* GetVertices() const { return m_pVertices; }

		//! returns a pointer to the vertices
		inline uchar* GetVertices() { return m_pVertices; }

		//! sets the pointers to the vertices
		void SetVertices(Vertex3D* pVertices, uint numVertices, uint format);

		//! sets the pointers to the vertices
		void SetVertices(uchar* pVertices, uint numVertices, uint vertexSize, std::vector<AttributeInfo>& attribInfos);
		
		//! returns the number of vertices
		inline uint GetNumVertices() const { return m_NumVertices; }

		//! returns the max vertices
		inline uint GetMaxVertices() const { return m_MaxVertices; }

		//! sets the number of vertices
		inline void SetNumVertices(uint numVertices)
		{
			SHOOT_ASSERT(numVertices+m_StartVertex <= m_MaxVertices, "VertexBuffer MaxVertices reached");
			m_NumVertices = numVertices;
		}

		//! sets the max number of vertices
		inline void SetMaxVertices(uint maxVertices)
		{
			SHOOT_ASSERT(maxVertices <= m_AllocatedVertices, "VertexBuffer AllocatedVertices reached");
			m_MaxVertices = maxVertices;
		}

		//! sets the starting vertex index used for rendering
		inline void SetStartVertex(uint startVertex)
		{
			SHOOT_ASSERT(startVertex < m_MaxVertices, "VertexBuffer m_MaxVertices reached");
			m_StartVertex = startVertex;
		}

		//! returns the vertex format
		inline uint GetFormat() const { return m_Format; }		

		//! sets the vertex flags
		inline void SetFormat(uint format) { m_Format = format; }

		//! returns a pointer to the indices
		inline void* GetIndices() const { return m_pIndices; }

		//! returns an index
		inline uint GetIndex(uint index) const { return _getIndex(index); }

		//! SetIndices
		void SetIndices(ushort* indices, uint numIndices)
		{
			SetIndices(indices, numIndices, false);
		}

		//! SetIndices
		void SetIndices(uint* indices, uint numIndices)
		{
			SetIndices(indices, numIndices, true);
		}
		
		//! returns the number of indices
		inline uint GetNumIndices() const { return m_NumIndices; }

		//! returns index type status
		inline bool Has32bitIndices() const { return _sizeOfIndex == 4; }

		//! sets the number of indices
		inline void SetNumIndices(uint numIndices)
		{
			SHOOT_ASSERT(numIndices <= m_MaxIndices, "VertexBuffer m_MaxIndices reached");
			m_NumIndices = numIndices;
		}

		//! sets the primitive type
		inline void SetPrimitiveType(GraphicsDriver::E_PrimitiveType eType) { m_ePrimitiveType = eType; }

		//! returns the primitive type
		inline GraphicsDriver::E_PrimitiveType GetPrimitiveType() const { return m_ePrimitiveType; }

		//! sets the dynamic flag
		inline void SetDynamic(bool bDynamic) { m_bDynamic = bDynamic; }

		//! gets the dynamic flag
		inline bool GetDynamic() const { return m_bDynamic; }

		//! sets the dirty flag
		inline void SetDirty(bool bDirty) { m_bDirty = bDirty; }

		//! updates the vertex buffer data
		virtual void UpdateData() { }

		//! set apply world transform status
		inline void SetApplyWorldTransforms(bool bApply) { m_bApplyWorldTransforms = bApply; }

		//! get world transform application status
		inline bool GetApplyWorldTransforms() const { return m_bApplyWorldTransforms; }

		//! returns an attribute info
		AttributeInfo* GetAttribInfo(const std::string& name);

		//! render helper
		void Draw();

		//! render helper
		void Draw(Shader* pShader);

	protected:

		//! sets the pointers to the indices
		void SetIndices(void* pIndices, uint numIndices, bool _32bitIndices = false);

		//! deletes the vertices
		void DeleteVertices();

		//! deletes the indices
		void DeleteIndices();

		Handle<Shader> m_Shader;
		uchar* m_pVertices;
		uint m_MaxVertices;
		uint m_StartVertex;
		uint m_NumVertices;
		uint m_AllocatedVertices;
		uint m_Format;
		void* m_pIndices;
		uint m_MaxIndices;
		uint m_NumIndices;
		GraphicsDriver::E_PrimitiveType m_ePrimitiveType;
		bool m_bDynamic;
		bool m_bDirty;
		bool m_bApplyWorldTransforms;
		uint m_VertexSize;
		std::vector<AttributeInfo> m_aAttribInfos;
		uint _sizeOfIndex;
		std::function<uint (uint)> _getIndex;
	};
}



