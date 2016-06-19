/* 

Amine Rehioui
Created: August 15th 2010

Uses code from the Irrlicht engine extension handler

*/

#include "Shoot.h"

#include "OpenGLExtensionHandler.h"

namespace shoot
{
	//! Constructor
	OpenGLExtensionHandler::OpenGLExtensionHandler(const char* const strExtensionStrings[E_Count])	
	{
		const char* strExtensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
		if (!strExtensions)
		{
			Log.Print("glGetString failed\n");
			return;
		}
		auto length = strlen(strExtensions);

		std::string extension;

		for(size_t i=0; i<length; ++i)
		{			
			if(strExtensions[i] == ' ')
			{
				Log << "GL EXTENSION: " << extension.c_str() << "\n";

				for(int j=0; j<E_Count; ++j)
				{
					if(extension == strExtensionStrings[j])
					{
						m_bHasExtension[j] = true;
						break;
					}
				}

				extension = "";
			}
			else
			{
				extension = extension + strExtensions[i];
			}
		}
		
		for(int i=0; i<E_Count; ++i)
		{
			if(std::string(strExtensionStrings[i]) == "Native")
			{
				m_bHasExtension[i] = true;
			}
		}
	}	
}

