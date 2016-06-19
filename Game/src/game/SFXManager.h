/* 

Amine Rehioui
Created: May 18th 2013

*/

#pragma once


#include "PulseManager.h"
#include "LaserManager.h"
#include "PelletManager.h"
#include "StencilEffects.h"
#include "ParticleGenerator.h"

namespace shoot
{
	// fowards
	class Bullet;
	class Laser;
	class Actor;

	//! SFXManager
	class SFXManager : public Entity
	{
		DECLARE_OBJECT(SFXManager, Entity);

	public:

		//! constructor
		SFXManager();

		//! destructor
		virtual ~SFXManager();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		void Init();

		//! called during the update of the entity
		void Update();

		//! called when a bullet collided
		void OnBulletCollided(Bullet* pBullet);

		//! called when a laser collided
		void OnLaserCollided(const Vector3& vIntersection);

		//! called when damage is done
		void OnDamageDone(const Vector3& vPosition);

		//! OnBulletImpact
		float OnBulletImpact(const Vector3& position, Object* shooter);

		//! called when an actor is destroyed
		void OnActorDestroyed(Actor* pActor);

		//! Makes an Actor explode
		float Explode(Actor* pActor);

		//! animates the skybox color
		void SetSkyBoxColor(const Color& color);

		//! SetActorExplosionsEnabled
		inline void SetActorExplosionsEnabled(bool enabled) { m_bActorExplosionsEnabled = enabled; }

		inline PulseManager* GetPlayerPulseManager() { return m_PlayerPulseManager; }
		inline LaserManager* GetPlayerLaserManager() { return m_PlayerLaserManager; }
		inline PelletManager* GetPlayerPelletManager() { return m_PlayerPelletManager; }
		inline PulseManager* GetEnemyPulseManager() { return m_EnemyPulseManager; }
		inline LaserManager* GetEnemyLaserManager() { return m_EnemyLaserManager; }
		inline PelletManager* GetEnemyPelletManager() { return m_EnemyPelletManager; }
		inline StencilEffects* GetStencilEffects() const { return m_StencilEffects; }
		inline ParticleGenerator* GetFireDamage() const { return m_FireDamage; }

		//! clears all effects
		void Clear();
			
	private:

		Handle<Entity> m_Root;

		Handle<PulseManager> m_PlayerPulseManager;
		Handle<LaserManager> m_PlayerLaserManager;
		Handle<PelletManager> m_PlayerPelletManager;
		Handle<PulseManager> m_PlayerSidePulseManager;
		Handle<LaserManager> m_PlayerSideLaserManager;
		Handle<PelletManager> m_PlayerSidePelletManager;
		Handle<PulseManager> m_EnemyPulseManager;
		Handle<LaserManager> m_EnemyLaserManager;
		Handle<PelletManager> m_EnemyPelletManager;
		Handle<StencilEffects> m_StencilEffects;
		Handle<ParticleGenerator> m_FireDamage;
		float m_fExplosionTracker;
		bool m_bActorExplosionsEnabled;
		
		//! returns the ExplosionVisitor corresponding to a template
		Object* GetExplosionVisitor(const std::string& templatePath);		

		//! updates the skybox
		void UpdateSkyBox();
		
		// skybox anim
		Color m_SrcSkyBoxColor;
		Color m_DestSkyBoxColor;
		float m_fInterpolator;
		bool m_bSkyBoxAnim;
		Handle<Material> m_SkyBoxMaterial;

		//! ExplosionInfo
		struct ExplosionInfo : public ISerializableStruct
		{
			//! Reads/Writes struct properties from/to a stream
			void Serialize(PropertyStream& stream)
			{
				stream.Serialize("Explosion", &m_Explosion);
				stream.Serialize("TemplatePaths", &m_TemplatePaths);
			}

			Reference<Object> m_Explosion;
			Array<IPath> m_TemplatePaths;
		};

		// properties
		float m_fExplosionIntensity;
		float m_fExplosionDuration;
		float m_fSkyBoxAnimDuration;
		float m_fCameraShakeIntensity;
		float m_fCameraShakeFrequency;
		Reference<Entity> m_ExplosionEntity;
		Reference<Entity> m_ImpactEntity;
		Reference<Entity> m_HitEntity;
		Reference<Entity> m_NoHitEntity;		
		Array<ExplosionInfo> m_ExplosionInfos;
	};
}



