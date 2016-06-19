/* 

Amine Rehioui
Created: October 17th 2011

*/

#pragma once


#if SHOOT_PLATFORM == SHOOT_PLATFORM_IOS
#define SizeType long unsigned int
#else
#define SizeType size_t
#endif

void* operator new(SizeType size, const char *strFile, int line);

void* operator new[](SizeType size, const char *strFile, int line);

void operator delete(void* ptr) throw();

void operator delete[](void* ptr) throw();

void operator delete(void* ptr, const char *strFile, int line);

void operator delete[](void* ptr, const char *strFile, int line);

#define snew new(__FILE__, __LINE__)

#define sdelete(x) if((x)) { delete (x); (x) = 0; }

#define sdelete_array(x) if((x)) { delete[] (x); (x) = 0; }



