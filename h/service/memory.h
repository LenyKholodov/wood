////////////////////////////////////////////////////////////////////////////////
///Name:        defs.h
///System:      kernel\Units
///Abstract:    Memory alloc\free unit
////////////////////////////////////////////////////////////////////////////////

#ifndef __KERNEL_MEMORY_SERVICE__
#define __KERNEL_MEMORY_SERVICE__

#include <kernel\service.h>

const    int    POOL_STACK_SIZE = 128;

////////////////////////////////////////////////////////////////////////////////
///Модуль распределения памяти в нити
////////////////////////////////////////////////////////////////////////////////
class    MemoryService: public Service
{
  friend bool K_Init (uint,...);
  public:
                        MemoryService  (Pool* pool = NULL,size_t pool_size = 0);
          virtual       ~MemoryService ();

    virtual   const char*       ServiceName () const { return "Memory service"; }

////////////////////////////////////////////////////////////////////////////////
///Распределение памяти
////////////////////////////////////////////////////////////////////////////////
          void*                 alloc       (size_t);
          void*                 alloc       (size_t,Pool*);
          void                  free        (void*);

////////////////////////////////////////////////////////////////////////////////
///Стек пулов
////////////////////////////////////////////////////////////////////////////////
          bool                  PushPool    (Pool*);
          void                  PopPool     ();
  inline  Pool*                 GetPool     () const { return mPoolStack [mPoolPos]; }

////////////////////////////////////////////////////////////////////////////////
///Перекрывание интерфейса с пулами
////////////////////////////////////////////////////////////////////////////////
          bool                  belong      (void* x) const { return mPoolStack [mPoolPos]->belong (x); }
          void                  reset       ()        { mPoolStack [mPoolPos]->reset (); }
          size_t                size        () const  { mPoolStack [mPoolPos]->size (); }
          size_t                memavail    () const  { mPoolStack [mPoolPos]->memavail (); }
          size_t                maxavail    () const  { mPoolStack [mPoolPos]->maxavail (); }

  protected:
    virtual bool        valid       () const { return mMainPool != NULL; }
    static  void*       AllocChunk  (size_t);  
    static  void        FreeChunk   (void*);

  private:
          Pool*         mMainPool;
          int           mPoolPos;
          Pool*         mPoolStack [POOL_STACK_SIZE];
};

extern void*  StdAlloc (size_t);
extern void   StdFree  (void*);

#endif