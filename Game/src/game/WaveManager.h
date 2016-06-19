/* 

Amine Rehioui
Created: March 9th 2014
*/

#pragma once


#include "BaseAttacker.h"
#include "AttackerSettings.h"

namespace shoot
{
	class UIMarker;
	class HQ;
	class SpawnPoint;

	//! WaveManager
	class WaveManager : public Entity
	{
		//! Macro to provide type information
		DECLARE_OBJECT(WaveManager, Entity);

	public:		

		//! Wave
		struct Wave : public ISerializableStruct
		{
			//! constructor
			Wave()
				: m_fDuration(20.0f)
				, m_fFrequency(1.0f)
				, m_SpawnIndex(-1)
				, m_TargetIndex(-1)
				, m_fZOffset(2.0f)
				, m_fHitPoints(100.0f)
			{
			}

			//! Reads/Writes struct properties from/to a stream
			void Serialize(PropertyStream& stream);

			float m_fDuration;
			float m_fFrequency;
			int m_SpawnIndex;
			int m_TargetIndex;
			float m_fZOffset;
			float m_fHitPoints;
			Reference<AttackerSettings> m_AttackerSettings;
			Reference<BaseAttacker> m_Attacker;
		};

		//! constructor
		WaveManager();

		//! destructor
		virtual ~WaveManager();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		void Init();

		//! called during the update of the entity		
		void Update();

		//! Decrements attacker count
		inline void DecrAttackers()
		{
			--m_AttackerCount;
			SHOOT_ASSERT(m_AttackerCount >= 0, "Negative AttackerCount detected");
		}

		//! called when the terran has changed
		void OnTerrainChanged();

		//! called to test if the path is blocked
		bool IsPathBlocked() const;

		//! OnHQDestroyed
		void OnHQDestroyed() const;

		//! GetNextHQ
		HQ* GetNextHQ() const;

		//! returns the attackers
		inline std::list< Handle<BaseAttacker> >& GetAttackers() { return m_lAttackers; }

		//! returns the objectives
		const std::vector< Handle<HQ> >& GetObjectives() const { return m_aObjectives; }

		//! GetAverageAttackerPos
		inline Vector3 GetAverageAttackerPos() const { return m_vAverageAttackerPos; }

	private:

		float m_fWaveTimer;
		float m_fSpawnTimer;
		size_t m_CurrentWave;
		int m_AttackerCount;
		std::vector< Handle<SpawnPoint> > m_aSpawnPoints;
		std::vector<int> m_aSpawnIndices;
		size_t m_SpawnIndex;
		std::list< Handle<BaseAttacker> > m_lAttackers;
		Handle<UIMarker> m_Marker;
		Handle<Entity3D> m_MarkerTarget;
		std::vector<AttackerInfo> m_aAttackerInfos;
		std::vector< Handle<HQ> > m_aObjectives;
		Vector3 m_vAverageAttackerPos;
		Handle<Entity> m_Pathway;
		Tile m_DummyTargetTile;
		float m_UITransitionTimer;
		std::vector< Handle<Entity2D> > m_WaveIcons;
		Reference<Shader> m_WaveIconShader;
		Handle<Entity2D> m_WavePanel;
		Handle<Entity2D> m_CutOffLine;
		Handle<Text> m_WaveText;
		bool m_Succeeded;

		void InitWave(size_t waveIndex);
		int GetRandomSpawnIndex();
		bool AttackersReachedTarget() const;
		void UpdateCurrentPath();
		void UpdateUI();
		void UpdateWaveText();

		//! IconInfo
		struct IconInfo : public ISerializableStruct
		{
			//! Reads/Writes struct properties from/to a stream
			void Serialize(PropertyStream& stream)
			{
				stream.Serialize("Icon", &m_Icon);
				stream.Serialize("Actor", &m_Actor);
			}

			IPath m_Icon;
			IPath m_Actor;
		};

		// properties
		Vector3 m_vSpawnRange;
		float m_fUITransitionDuration;
		float m_fUIOffset;
		Vector2 m_WaveIcon0Pos;
		Vector2 m_WaveIcon1Pos;
		Array<IconInfo> m_IconInfos;
		Array<Wave> m_aWaves;
	};
}



