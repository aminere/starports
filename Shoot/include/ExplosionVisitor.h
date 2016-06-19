/* 

Amine Rehioui
Created: May 6th 2012

*/

#pragma once


#include "Visitor.h"

#include "VertexBuffer.h"

#include "Timer.h"

#include "MeshResource.h"

namespace shoot
{
	//! ExplosionVisitor
	class ExplosionVisitor : public Visitor
	{
	public:

		//! explosion chunk
		struct Chunk : ISerializableStruct
		{
			//! constructor
			Chunk()
				: vCenter(Vector3::Zero)
				, vPosition(Vector3::Zero)
				, vRotation(Vector3::Zero)
				, vDirection(Vector3::Zero)
			{
				Reset();
			}

			//! Reads/Writes struct properties from/to a stream
			void Serialize(PropertyStream& stream);
			
			//! Resets the chunk
			inline void Reset()
			{
				vCenter = Vector3::Zero;
				vPosition = Vector3::Zero;
				vRotation = Vector3::Zero;
				vDirection = Vector3::Zero;
			}

			Vector3 vCenter;
			Vector3 vPosition;
			Vector3 vRotation;
			Vector3 vDirection;

			// properties
			Array<uint> aTriangleIndices;
		};

		//! ChunkGroup
		struct ChunkGroup : public ISerializableStruct
		{
			//! Reads/Writes struct properties from/to a stream
			void Serialize(PropertyStream& stream)
			{
				stream.Serialize("Chunks", &m_aChunks);
			}

			Array<Chunk> m_aChunks;
		};

		DECLARE_OBJECT(ExplosionVisitor, Visitor);

		//! constructor
		ExplosionVisitor();

		//! destructor
		virtual ~ExplosionVisitor();

		//! Reads/Writes struct properties from/to a stream
		void Serialize(PropertyStream& stream);

		//! visits a particular entity
		void Visit(Entity* pTarget);

		//! updates the visitor
		void Update();

		//! returns the target mesh
		MeshResource* GetMesh() const { return m_Mesh; }

		//! sets the intensity
		inline void SetDefaultIntensity(float fIntensity) { m_fDefaultIntensity = fIntensity; }

		//! sets the default duration
		inline void SetDefaultDuration(float fDuration) { m_fDefaultDuration = fDuration; }

		//! returns the duration
		inline float GetDuration() const { return m_fDuration; }

#ifdef SHOOT_EDITOR
		//! returns the explosion chunk groups
		Array<ChunkGroup>& GetChunkGroups() { return m_aChunkGroups; }
#endif // SHOOT_EDITOR

	private:

		std::vector< Reference<VertexBuffer> > m_VertexBuffers;
		std::vector< Vector3* > m_OriginalVertices;
		std::vector< uint* > m_IndexMaps;
		Timer m_Timer;
		float m_fDefaultDuration;
		float m_fDefaultIntensity;

		// properties
		Array<ChunkGroup> m_aChunkGroups;
		Reference<MeshResource> m_Mesh;
		float m_fDuration;
		float m_fIntensity;
		bool m_bDepthTest;
	};
}




