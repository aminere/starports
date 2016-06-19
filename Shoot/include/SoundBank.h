/* 

Amine Rehioui
Created: August 26th 2013

*/

#pragma once


// forwards
class CkBank;

namespace shoot
{
	//! SoundBank
	class SoundBank : public Resource
	{
		DECLARE_OBJECT(SoundBank, Resource);

	public:

		//! constructor
		SoundBank() : m_pBank(NULL)
		{
			SHOOT_ASSERT(false, "Invalid Constructor");
		}

		//! constructor
		SoundBank(const char* strBank);

		//! destructor
		virtual ~SoundBank();

		//! returns the cricket bank
		inline CkBank* GetBank() { return m_pBank; }

	private:

		CkBank* m_pBank;
	};
}



