/* 

Amine Rehioui
Created: August 4th 2010

*/

#pragma once


namespace shoot
{
	//! App class
	class App
	{
	public:

		//! constructor
		App(bool bFullScreen = false);

		//! destructor
		virtual ~App();

		//! updates the app
		virtual bool Update(float fDeltaTime) = 0;

		virtual void Run() = 0;

		//! returns the fullscreen status
		inline bool IsFullScreen() const { return m_bFullScreen; }

		//! returns the instance
		inline static App* Instance() { return m_spInstance; }		

	protected:

		bool m_bFullScreen;
		static App* m_spInstance;
	};
}



