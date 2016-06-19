/* 

Amine Rehioui
Created: April 2nd 2011

*/

#pragma once


namespace shoot
{
	// forwards
	class Visitor;

	//! Visitor listener interface
	class VisitorListener
	{
	public:

		//! called when a visitor finished its job
		virtual void OnVisitorFinished(Visitor* pVisitor) { }
	};
}



