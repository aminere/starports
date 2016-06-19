/* 

Amine Rehioui
Created: April 26th 2014

*/

#ifndef _GAME_ENTITY_PROCESSOR_H_INCLUDED_
#define _GAME_ENTITY_PROCESSOR_H_INCLUDED_

namespace shoot
{
	//! GameEntityProcessor
	class GameEntityProcessor
	{
	public:

		//! processes entities
		void Process();

	private:

		//! recursively process entities
		void ProcessEntities(const std::string& strDirectory);
	};
}

#endif // _GAME_ENTITY_PROCESSOR_H_INCLUDED_

