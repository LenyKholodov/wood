//////////////////////////////////////////////////////////////////////////////////////////////
//Name:         memory.h
//System:       Memory,Cache
//Abstract:     Memory & cache system
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __MEMORY_ROOT__
#define __MEMORY_ROOT__

#include <types.h>

class Pool;
class MemObject;

extern Pool*      M_DefaultPool     ();
extern Pool*      M_GetPool         ();

namespace Memory 
{
  inline Pool* M_GetPool () { return ::M_GetPool (); }
};

//////////////////////////////////////////////////////////////////////////////////////////////
///Распределялки перегружены!
//////////////////////////////////////////////////////////////////////////////////////////////
extern     void* operator new           (size_t);
extern     void* operator new           (size_t,Pool*);
extern     void* operator new []        (size_t);
extern     void* operator new []        (size_t,Pool*);
extern     void  operator delete        (void*);
extern     void  operator delete []     (void*);

/////////////////////////////////////////////////////////////////////////////////////////////
///Отключение системных библиотек (для нормального функционирования менеджера памяти
/////////////////////////////////////////////////////////////////////////////////////////////
#pragma comment (linker,"/nodefaultlib:libcp.lib")

#ifdef DEBUG
  #pragma comment (lib,"memmand.lib")
#else
  #pragma comment (lib,"memman.lib")
#endif

#endif