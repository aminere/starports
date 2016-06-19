/* 

Amine Rehioui
Created: August 12th 2011

*/

#pragma once


namespace shoot
{
	//! Mutex class
	class Mutex
	{
	public:

		//! _Lock
		class _Lock
		{
		public:

			_Lock(Mutex* pMutex)
				: m_pMutex(pMutex)
			{
				m_pMutex->Lock();
			}

			~_Lock()
			{
				m_pMutex->Unlock();
			}

		private:

			Mutex* m_pMutex;
		};

		//! constructor
		static Mutex* Create();

		//! destructor
		virtual ~Mutex()
		{
		}

		//! Attempts to lock the mutex
		virtual void Lock() = 0;

		//! Unlocks the mutex
		virtual void Unlock() = 0;
	};
}



