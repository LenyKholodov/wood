#ifndef __CONVER_M3D_MODEL__
#define __CONVER_M3D_MODEL__

#include <graph\x3f.h>
#include "defs.h"

////////////////////////////////////////////////////////////////////////////////
///Конвертер 3DS - Abstract
////////////////////////////////////////////////////////////////////////////////
class  M3DModelConv: public MemObject
{
  public:
                        M3DModelConv  (Pool*,Object3D&,const vector3d_t& axis = vector3d_t (.0f));
            virtual     ~M3DModelConv ();

////////////////////////////////////////////////////////////////////////////////
///Сохранение модели
////////////////////////////////////////////////////////////////////////////////
   virtual  void        Save          (const char*,ostream* = NULL) {}

  protected:
            void        normalize     (const vector3d_t&);
            void        convArrays    ();

////////////////////////////////////////////////////////////////////////////////
///Служебный класс - данные открыты
////////////////////////////////////////////////////////////////////////////////
  public:
            bound_t             mBound;

            facet_t*            mFacet;
            vertex_t*           mVertex;
            vector3d_t*         mNormal;
            textcoord_t*        mTextCoord;
            textmat_t*          mMat;

            int                 mFCount,mVCount,mNCount,mMCount;

            char*               mMatStr;
            size_t              mMatStrSize;
};


#endif