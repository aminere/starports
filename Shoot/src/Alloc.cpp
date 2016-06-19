/* 

Amine Rehioui
Created: October 17th 2011

*/

#include "Shoot.h"

#include "MemoryManager.h"

void* operator new(SizeType size, const char *strFile, int line)
{
	return shoot::MemoryManager::Alloc(size, strFile, line);
}

void* operator new[](SizeType size, const char *strFile, int line)
{
	return shoot::MemoryManager::Alloc(size, strFile, line);
}

void operator delete(void* ptr) throw()
{
	shoot::MemoryManager::Free(ptr);
}

void operator delete[](void* ptr) throw()
{
	shoot::MemoryManager::Free(ptr);
}

void operator delete(void* ptr, const char *strFile, int line)
{
	shoot::MemoryManager::Free(ptr);
}

void operator delete[](void* ptr, const char *strFile, int line)
{
	shoot::MemoryManager::Free(ptr);
}
