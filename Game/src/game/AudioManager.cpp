/* 

Amine Rehioui
Created: August 28th 2013

*/

#include "Game.h"

#include "AudioManager.h"

#include "AudioDriver.h"

#include "GameEngine.h"

namespace shoot
{
	// Sound literals
	const char* const g_strSoundLiterals[] =
	{
		"S_Explosion",
		"S_ExplosionNuclear",
		"S_WheelOpen",
		"S_WheelClose",
		"S_MachineGun",
		"S_Electricity",
		"S_Flames",
		"S_MissileLaunch",
		"S_MetalRiff",
		"S_EnemyPulse",
		"S_None",
		0
	};

	DEFINE_OBJECT(AudioManager);

	//! constructor
	AudioManager::AudioManager()
		: m_MusicType(M_None)
		, m_fMusicVolume(1.0f)
	{
		m_MetalTracks.push_back("Starports_ambiance1.ogg");
		m_MetalTracks.push_back("Starports_ambiance2.ogg");
		m_MetalTracks.push_back("Starports_ambiance3.ogg");
		m_MetalTracks.push_back("Starports_ambiance4.ogg");

		for (size_t i=0; i<m_MetalTracks.size(); ++i)
		{
			m_RemainingMetalTracks.push_back((int)i);
		}
	}

	//! destructor
	AudioManager::~AudioManager()
	{
	}

	//! serializes the entity to/from a PropertyStream
	void AudioManager::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		
		stream.Serialize("SoundInfos", &m_SoundInfos);
	}	

	//! called during the update of the entity
	void AudioManager::Update()
	{
		if(m_Music.IsValid() && !m_Music->IsPlaying())
		{
			//if(m_MusicType == M_Metal)
			{
				SetMusicVolume(m_fMusicVolume);
				m_Music = PickMetalTrack();
				m_Music->Play();
			}
		}
	}

	//! Reads/Writes struct properties from/to a stream
	void AudioManager::SoundInfo::Serialize(PropertyStream& stream)
	{
		stream.Serialize("Sound", &m_Sound, ENUM_PARAMS1(g_strSoundLiterals));
		stream.Serialize("Bank", &m_Bank);
		stream.Serialize("Names", &m_Names);
	}
	
	//! Plays a sound
	void AudioManager::Play(E_Sound sound, bool bStopIfPlaying /*= false*/, bool bIsLoop /*= false*/)
	{
		bool bExists = m_SoundMap.find(sound) != m_SoundMap.end();
		if(!bExists)
		{
			if(SoundInfo* pInfo = GetSoundInfo(sound))
			{
				bool bBankExists = m_SoundBankMap.find(pInfo->m_Bank.GetString()) != m_SoundBankMap.end();
				SoundBank* pBank = bBankExists ? m_SoundBankMap[pInfo->m_Bank.GetString()].Get() : AudioDriver::Instance()->CreateSoundBank(pInfo->m_Bank->c_str());
				for (size_t i = 0; i<pInfo->m_Names.GetSize(); ++i)
				{
					Sound* pSound = AudioDriver::Instance()->CreateSound(pBank, pInfo->m_Names[i].c_str());
					m_SoundMap[sound].push_back(pSound);
				}
				auto index = Random::GetInt(0, (int)pInfo->m_Names.GetSize()-1);
				m_SoundMap[sound][index]->Play();

				if(!bBankExists)
				{
					m_SoundBankMap[pInfo->m_Bank.GetString()] = pBank;
				}
			}
		}
		else
		{
			if(bStopIfPlaying)
			{
				auto index = Random::GetInt(0, (int)m_SoundMap[sound].size() - 1);
				m_SoundMap[sound][index]->Play();
			}
			else if (bIsLoop)
			{
				auto index = Random::GetInt(0, (int)m_SoundMap[sound].size() - 1);
				if (!m_SoundMap[sound][index]->IsPlaying())
					m_SoundMap[sound][index]->Play();
			}
			else
			{
				std::vector<int> indices;
				for (size_t i = 0; i<m_SoundMap[sound].size(); ++i)
				{
					indices.push_back((int)i);
				}

				std::vector<int> randomIndices;
				while(!indices.empty())
				{
					auto randomIndex = Random::GetInt(0, (int)indices.size() - 1);
					randomIndices.push_back(indices[randomIndex]);
					indices.erase(indices.begin()+randomIndex);
				}

				int soundIndex = -1;
				for (size_t i=0; i<randomIndices.size(); ++i)
				{
					if(!m_SoundMap[sound][randomIndices[i]]->IsPlaying())
					{
						soundIndex = randomIndices[i];
						break;
					}
				}

				if(soundIndex < 0)
				{
					SoundInfo* pInfo = GetSoundInfo(sound);
					auto index = Random::GetInt(0, (int)pInfo->m_Names.GetSize()-1);                    
					Sound* pSound = AudioDriver::Instance()->CreateSound(m_SoundBankMap[pInfo->m_Bank.GetString()].Get(), pInfo->m_Names[index].c_str());
					m_SoundMap[sound].push_back(pSound);
					soundIndex = (int)m_SoundMap[sound].size()-1;
                    Log.Print("m_SoundMap[%d].size: %d\n", sound, m_SoundMap[sound].size());
				}

				m_SoundMap[sound][soundIndex]->Play();
			}
		}
	}

	//! Plays a music
	void AudioManager::Play(E_Music music)
	{
		switch(music)
		{
		case M_Metal:
			m_Music = PickMetalTrack();
			break;

		case M_Arpeggio:
			m_Music = AudioDriver::Instance()->CreateMusic("eminor_arpeggios.ogg");
			break;
		}
		SetMusicVolume(m_fMusicVolume);
		m_Music->Play();
		m_MusicType = music;
	}

	//! returns the SoundInfo
	AudioManager::SoundInfo* AudioManager::GetSoundInfo(E_Sound sound)
	{
		for (size_t i = 0; i<m_SoundInfos.GetSize(); ++i)
		{
			if(m_SoundInfos[i].m_Sound == sound)
			{
				return &m_SoundInfos[i];
			}
		}

		return NULL;
	}

	//! picks a random metal track
	Music* AudioManager::PickMetalTrack()
	{
		if(m_RemainingMetalTracks.empty())
		{
			for (size_t i=0; i<m_MetalTracks.size(); ++i)
			{
				m_RemainingMetalTracks.push_back((int)i);
			}
		}

		int slot = Random::GetInt(0, (int)m_RemainingMetalTracks.size()-1);
		int track = m_RemainingMetalTracks[slot];
		m_RemainingMetalTracks.erase(m_RemainingMetalTracks.begin()+slot);
		return AudioDriver::Instance()->CreateMusic(m_MetalTracks[track]);
	}	

	//! sets the music volume
	void AudioManager::SetMusicVolume(float fVolume)
	{
		AudioDriver::Instance()->SetMusicVolume(fVolume);
		m_fMusicVolume = fVolume;
	}
}

