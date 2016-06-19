/* 

Amine Rehioui
Created: May 30th 2010

*/

#pragma once


#include "Range.h"

#include "AABBox3D.h"
#include "MemoryPool.h"

namespace shoot
{
	//! 3D Particle Generator - only box volume for now and with simplistic implementation
	class ParticleGenerator : public Entity3D				    			   
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(ParticleGenerator, Entity3D);

		//! constructor
		ParticleGenerator();

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		virtual void Init();

		//! called during the update of the entity		
		virtual void Update();

		//! returns the affine transformation matrix
		virtual Matrix44 GetWorldTransform();
			
		//! returns the emit box
		const AABBox3D& GetEmitBox() const { return m_EmitBox; }

		//! sets the active status
		inline void SetActive(bool bActive) { m_bActive = bActive; }

		//! GetSpeed
		inline const Range& GetSpeed() const { return m_rParticleInitialSpeed; }

		//! SetLife
		inline void SetLife(const Range& life) { m_rParticleLife = life; }

	protected:

		//! particle info
		struct Particle
		{
			Vector3 Pos;
			Vector3 PreviousPos;
			Vector3 Speed;
			float Life;
			float Alpha;
			float InitialLife;
			float Size;
		};

		//! emission type
		enum E_EmitType
		{
			ET_Box,
			ET_Explosion,
			ET_Cone
		};

		//! particle shape
		enum E_ParticleShape
		{
			PS_Quad,
			PS_Line
		};

		//! SizeFunc
		enum E_SizeFunc
		{
			SF_Constant,
			SF_Grow,
			SF_Shrink
		};

		//! emit functors
		typedef void (ParticleGenerator::*EmitFunctor)(Particle* pParticle, const Vector3& vCenter, void* pEmitData);
		typedef void* (ParticleGenerator::*PreEmitFunctor)();
		typedef float (ParticleGenerator::*SizeFuncFunctor)(Particle*);
		void* PreEmitNull();
		void* PreEmitCone();
		void EmitBox(Particle* pParticle, const Vector3& vCenter, void* pEmitData);
		void EmitExplosion(Particle* pParticle, const Vector3& vCenter, void* pEmitData);
		void EmitCone(Particle* pParticle, const Vector3& vCenter, void* pEmitData);
		float SizeFuncConstant(Particle*);
		float SizeFuncGrow(Particle*);
		float SizeFuncShrink(Particle*);

		//! EmitConeData
		struct EmitConeData
		{
			Vector3 vLateralAxe;
			Vector3 vVerticalAxe;
			float fAngle;
		};

		//! particle generation functors
		typedef int (ParticleGenerator::*TesselateFunctor)(Particle* pParticle, Vertex3D* pVertices, int current, const Vector3& vScale);
		int TesselateQuad(Particle* pParticle, Vertex3D* pVertices, int current, const Vector3& vScale);
		int TesselateLine(Particle* pParticle, Vertex3D* pVertices, int current, const Vector3& vScale);

		//! called when particle shape has changed
		int OnParticleShapeChanged(VertexBuffer* pVB);

		MemoryPool<Particle> m_Pool;
		float m_fNewParticlesCounter;
		bool m_bActive;
		PreEmitFunctor m_PreEmitFunctor;
		EmitFunctor m_EmitFunctor;
		TesselateFunctor m_TesselateFunctor;
		SizeFuncFunctor m_SizeFuncFunctor;
		float m_fHalfSize;
		Matrix44 m_ViewInverse;
		Color m_ParticleColor;
		EmitConeData m_EmitConeData;

		// properties
		int m_MaxParticles;
		int m_ParticlesPerSecond;
		Range m_rParticleLife;
		Range m_rParticleInitialSpeed;
		float m_fParticleSize;
		Vector3 m_vDirection;		
		Vector3 m_vGravity;
		AABBox3D m_EmitBox;
		float m_fActiveTime;
		E_EmitType m_eEmitType;
		E_ParticleShape m_eParticleShape;
		bool m_bApplyWorldTransform;
		float m_fConeAngle;
		float m_fSparkFactor;		
		E_SizeFunc m_SizeFunc;
		float m_SizeFuncFactor;
		bool m_bInstancedRendering;
	};
}



