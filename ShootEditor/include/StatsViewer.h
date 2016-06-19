/* 

Amine Rehioui
Created: March 31st 2011

*/

#pragma once


namespace shoot
{
	//! Stats Viewer
	class StatsViewer : public wxFrame
	{
		typedef wxFrame super;

	public:
		
		//! Constructor
		StatsViewer();

		//! Destructor
		virtual ~StatsViewer();	

		//! returns the static instance
		inline static StatsViewer* Instance() { return ms_pInstance; }

	private:

		static StatsViewer* ms_pInstance;
	};
}




