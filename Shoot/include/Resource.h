/* 

Amine Rehioui
Created: June 13th 2010

*/

#pragma once


#include "Utils.h"

namespace shoot
{
	//! Resource interface
	class Resource : public Object
	{
		DECLARE_OBJECT(Resource, Object);

	public:

		//! constructor
		Resource();

		//! destructor
		virtual ~Resource();

		//! Load from disk
		virtual void ResourceInit() { }

		//! Unload disk data
		virtual void ResourceUninit() { }

		//! returns true if this resource is using the specified data
		virtual bool IsUsingData(const char* strPath) const { return Utils::Equals(GetTemplatePath().c_str(), strPath); }
	};
}



