/* 

Amine Rehioui
Created: August 28th 2013

*/

#pragma once


#include "Sound.h"
#include "Music.h"

namespace shoot
{
	//! AudioManager
	class AudioManager : public Entity
	{
		DECLARE_OBJECT(AudioManager, Entity);

	public:

		//! E_Sound
		enum E_Sound
		{
			S_Explosion,
			S_ExplosionNuclear,
			S_WheelOpen,
			S_WheelClose,
			S_MachineGun,
			S_Electricity,
			S_Flames,
			S_MissileLaunch,
			S_MetalRiff,
			S_EnemyPulse,
			S_None,
			S_Count
		};

		//! E_Music
		enum E_Music
		{
			M_Metal,
			M_Arpeggio,
			M_Count,
			M_None
		};

		//! constructor
		AudioManager();

		//! destructor
		virtual ~AudioManager();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! called during the update of the entity
		void Update();

		//! Plays a sound
		void Play(E_Sound sound, bool bStopIfPlaying = false, bool bIsLoop = false);

		//! Plays a music
		void Play(E_Music music);

		//! sets the music volume
		void SetMusicVolume(float fVolume);

	private:

		//! picks a random metal track
		Music* PickMetalTrack();

		//! SoundInfo
		struct SoundInfo : public ISerializableStruct
		{
			//! constructor
			SoundInfo()
				: m_Sound(S_None)
			{
			}

			//! Reads/Writes struct properties from/to a stream
			void Serialize(PropertyStream& stream);

			E_Sound m_Sound;
			IPath m_Bank;
			Array<std::string> m_Names;
		};		

		std::map<std::string, Reference<SoundBank> > m_SoundBankMap;		
		std::map<E_Sound, std::vector< Reference<Sound> > > m_SoundMap;

		E_Music m_MusicType;
		Reference<Music> m_Music;		
		std::vector<const char*> m_MetalTracks;
		std::vector<int> m_RemainingMetalTracks;
		float m_fMusicVolume;

		//! returns the SoundInfo
		SoundInfo* GetSoundInfo(E_Sound sound);

		// properties
		Array<SoundInfo> m_SoundInfos;
	};
}



