#ifndef __MEMORY_REFERENCES__
#define __MEMORY_REFERENCES__

#include <service\memory.h>
#include <memory\memory.h>

//Pool 

#include <memory\object.h>
#include <memory\system.h>
#include <memory\clpool.h>
#include <memory\float.h>
#include <memory\temp.h>
#include <memory\dpool.h>
//#include <memory\hpool.h>
#include <memory\xpool.h>

extern accessptr_t<MemoryService>  memoryService;

MemoryService*      M_Load     (Pool* pool = NULL,size_t pool_size = 0);
void                M_Done     (MemoryService* = memoryService);

/*
  Выделяет память из пула, находящегося на вершине стека пулов нити
    _Size - размер запрашиваемого блока памяти    
*/

#define   M_Alloc          memoryService->alloc

/*
  Освобождает блок памяти, выделенный при помощи M_Alloc ()
    _Ptr - указатель на блок памяти  
*/

#define   M_Free           memoryService->free     

/*
  Возвращает текущий пул для выделения нити 
  (operator new (M_Alloc)/operator delete (M_Free))  
*/

inline 
Pool*     M_GetPool ()
{
  return memoryService->GetPool ();
}

/*
  Помещает пул в вершину стека пулов нити
    _Pool - пул
*/

#define   M_PushPool       memoryService->PushPool

/*
  Извлекает пул из вершины стека нити
*/

#define   M_PopPool        memoryService->PopPool     

/*
  Следующие 5 функций дублируют анологичные методы из class Pool
  Все операции проводятся с пулом, находящимся в вершине стека пулов нити
*/

#define   M_Size           memoryService->size 
#define   M_MemAvail       memoryService->memavail
#define   M_MaxAvail       memoryService->maxAvail
#define   M_Belong         memoryService->belong
#define   M_Reset          memoryService->reset

#endif
