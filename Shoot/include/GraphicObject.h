/* 

Amine Rehioui
Created: July 14th 2013

*/

#pragma once


namespace shoot
{
	//! GraphicObject
	class GraphicObject
	{
	public:

		//! constructor
		GraphicObject();

		//! destructor
		virtual ~GraphicObject();

		//! Load into video memory
		virtual void GraphicLoad() { }

		//! Unload from video memory
		virtual void GraphicUnload() { }

		//! returns loaded status
		virtual bool IsLoaded() const { return true; }
	};
}



