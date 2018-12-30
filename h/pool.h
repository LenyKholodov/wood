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
  �뤥��� ������ �� �㫠, ��室�饣��� �� ���設� �⥪� �㫮� ���
    _Size - ࠧ��� ����訢������ ����� �����    
*/

#define   M_Alloc          memoryService->alloc

/*
  �᢮������� ���� �����, �뤥����� �� ����� M_Alloc ()
    _Ptr - 㪠��⥫� �� ���� �����  
*/

#define   M_Free           memoryService->free     

/*
  �����頥� ⥪�騩 �� ��� �뤥����� ��� 
  (operator new (M_Alloc)/operator delete (M_Free))  
*/

inline 
Pool*     M_GetPool ()
{
  return memoryService->GetPool ();
}

/*
  ����頥� �� � ���設� �⥪� �㫮� ���
    _Pool - ��
*/

#define   M_PushPool       memoryService->PushPool

/*
  ��������� �� �� ���設� �⥪� ���
*/

#define   M_PopPool        memoryService->PopPool     

/*
  ������騥 5 �㭪権 �㡫����� ��������� ��⮤� �� class Pool
  �� ����樨 �஢������ � �㫮�, ��室�騬�� � ���設� �⥪� �㫮� ���
*/

#define   M_Size           memoryService->size 
#define   M_MemAvail       memoryService->memavail
#define   M_MaxAvail       memoryService->maxAvail
#define   M_Belong         memoryService->belong
#define   M_Reset          memoryService->reset

#endif
