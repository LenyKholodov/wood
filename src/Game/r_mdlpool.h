#ifndef __RENDER_MODEL_POOL__
#define __RENDER_MODEL_POOL__

#include "r_mdlkeeper.h"

////////////////////////////////////////////////////////////////////////////////
///Пул моделей
////////////////////////////////////////////////////////////////////////////////
class    ModelPool: public MemObject
{
  public:
                        ModelPool  (Pool*,size_t);
         virtual        ~ModelPool ();

         virtual bool   valid      () const { return mDefragPool.valid (); }

////////////////////////////////////////////////////////////////////////////////
///Распределение памяти
////////////////////////////////////////////////////////////////////////////////
        void*           alloc      (model_t* mdl,size_t size);
        void            free       (model_t* mdl);

        size_t          size       () const { return mDefragPool.size (); }
        size_t          memavail   () const { return mDefragPool.memavail (); }
        size_t          maxavail   () const { return mDefragPool.maxavail (); }

        bool            belong     (void* x) const { return mDefragPool.belong (x); }

  private:
        DefragPool      mDefragPool;
};

#endif