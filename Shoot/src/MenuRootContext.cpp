/*

Amine Rehioui
Created: May 8th 2015

*/

#include "Shoot.h"

#include "MenuRootContext.h"

namespace shoot
{
	DEFINE_OBJECT(MenuRootContext);

	//! serializes the entity to/from a PropertyStream
	void MenuRootContext::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		stream.Serialize("PreloadedContexts", &m_PreloadedContexts);
	}

	//! AddReference
	void MenuRootContext::AddReference(Context* context)
	{
		bool exists = false;
		for (auto _context : m_PreloadedContexts)
			if (_context->Get() == context)
			{
				exists = true;
				break;
			}

		if (!exists)
			m_PreloadedContexts.Add(snew Reference<Context>(context));
	}
}

