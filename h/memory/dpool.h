///////////////////////////////////////////////////////////////////////////////
//Name:         dpool.h
//System:       Memory
//Abstract:     Pool with defrag float alloc
///////////////////////////////////////////////////////////////////////////////

#ifndef __MEMORY_DEFRAG_POOL__
#define __MEMORY_DEFRAG_POOL__

#include <memory/float.h>

typedef BOOL (*MoveObjectFn)(void* from,void* to,DWORD);

///////////////////////////////////////////////////////////////////////////////
///Пул с возможностью выделения блоков плавающего размера и дефрагментации
///////////////////////////////////////////////////////////////////////////////
class    DefragPool: public FloatPool
{
  friend class DefragCache;
  protected:
                                DefragPool  ();
  public:                                
                                DefragPool  (Pool*);
                                DefragPool  (Pool*,size_t);
          virtual               ~DefragPool ();

                                FloatPool::alloc;
                        void*   alloc    (size_t,void*);
                        void*   alloc    (size_t,MoveObjectFn,DWORD = 0);

          virtual       bool    belong   (void*) const;

          virtual       size_t  maxavail ()      const;
          virtual       size_t  memavail ()      const;
          
////////////////////////////////////////////////////////////////////////////////
///Дефрагментация
////////////////////////////////////////////////////////////////////////////////
                        void    defrag  ();
                        bool    defrag  (size_t);

  protected:
          virtual       void*         _alloc   ( size_t );
          virtual       void          _free    ( void*  );          

  private:         
                        bool          _move    ( freedesc_t* left,freedesc_t* right);

  private:
         DWORD          mParam;
         void*          mChange;
};

#endif