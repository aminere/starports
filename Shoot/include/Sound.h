/* 

Amine Rehioui
Created: August 26th 2013

*/

#pragma once


// forwards
class CkSound;

namespace shoot
{
	// forwards
	class SoundBank;

	//! Sound
	class Sound : public Resource
	{
		DECLARE_OBJECT(Sound, Resource);

	public:

		//! constructor
		Sound() : m_pSound(NULL)
		{
			SHOOT_ASSERT(false, "Invalid Constructor");
		}

		//! constructor
		Sound(SoundBank* pBank, const char* strName);

		//! destructor
		virtual ~Sound();

		//! plays the sound
		void Play();

		//! stops the sound
		void Stop();

		//! returns the playing status
		bool IsPlaying() const;

		//! returns the cricket sound
		inline CkSound* GetSound() { return m_pSound; }

	private:

		CkSound* m_pSound;
	};
}



