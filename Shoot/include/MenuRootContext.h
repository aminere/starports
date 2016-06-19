/*

Amine Rehioui
Created: May 8th 2015

*/

#pragma once

namespace shoot
{
	//! MenuRootContext
	class MenuRootContext : public Context
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(MenuRootContext, Context);

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream) override;

		//! AddReference
		void AddReference(Context* context);

	private:

		Array< Reference<Context> > m_PreloadedContexts;
	};
}

