/* 

Amine Rehioui
Created: December 4th 2014
*/

#pragma once


#include "RandomPool.h"
#include "Defender.h"

namespace shoot
{
	class Button;

	//! DefenderManager
	class DefenderManager : public Entity3D
	{
		DECLARE_OBJECT(DefenderManager, Entity3D);

	public:	

		//! E_State
		enum E_State
		{
			S_Ready,
			S_Spawning,
			S_Defending,
			S_PullBack
		};

		// Entity
		DefenderManager();
		void Serialize(PropertyStream& stream);
		void Init();
		void Update();
		//

		void SpawnDefenders();
		void UpdateSpawning();
		inline std::list< Handle<Defender> >& GetDefenders() { return m_lDefenders; }

	private:

		void SwitchState(E_State state);		

		E_State m_State;
		float m_WaveTimer;
		float m_SpawnTimer;
		RandomPool<int> m_RandomSpots;
		std::list< Handle<Defender> > m_lDefenders;
		Handle<Shader> m_CooldownShader;
		float m_CooldownTimer;
		float m_DiseappearTimer;
		Handle<Button> m_CallDownButton;

		// properties
		Reference<Actor> m_PreloadedActor;
	};
}



