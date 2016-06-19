/* 

Amine Rehioui
Created: December 6th 2014

*/

#pragma once


namespace shoot
{
	//! RandomPool
	template<class T>
	class RandomPool
	{
	public:

		RandomPool(int size)
		{
			for(int i=0; i<size; ++i)
				m_Pool.push_back(i);
		}

		T GetNext()
		{
			if(m_RandomPool.empty())
				m_RandomPool = m_Pool;

			auto randomIndex = Random::GetInt(0, (int)m_RandomPool.size()-1);
			auto random = m_RandomPool[randomIndex];
			m_RandomPool.erase(m_RandomPool.begin()+randomIndex);
			return random;
		}

	private:

		std::vector<T> m_Pool;
		std::vector<T> m_RandomPool;		
	};
}



