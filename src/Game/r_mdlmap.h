#ifndef __RENDER_MODEL_MAP__
#define __RENDER_MODEL_MAP__

#include <misc.h>

#include "r_mdlkeeper.h"

////////////////////////////////////////////////////////////////////////////////
///Карта моделей
////////////////////////////////////////////////////////////////////////////////
class    ModelMap: public MemObject
{
  public:
                        ModelMap  (Pool*,ModelEnv*,int _GradShiftKoef = 2);
             virtual    ~ModelMap ();

             virtual bool valid   () const { return mNodePool.valid () && 
                                             mFlag.valid () && mMap != NULL && mOffs != NULL;
                                           }

////////////////////////////////////////////////////////////////////////////////
///Вставка/удаление модели из карты
////////////////////////////////////////////////////////////////////////////////
        BOOL            InsertModel  (model_t*);
        void            ExtractModel (model_t*);

////////////////////////////////////////////////////////////////////////////////
///Работа с тайлами
////////////////////////////////////////////////////////////////////////////////
        mdlmapnode_t*   get          (int x,int y);
        void            reset        ();

  protected:
        ModelEnv*       mModelEnv;
        FlagBit         mFlag;
        mdlmapnode_t**  mMap;
        int*            mOffs;
        int             mShift, mWidth, mHeight;

        ClPool<mdlmapnode_t> mNodePool;
};

#endif