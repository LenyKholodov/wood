#ifndef __CONVER_SIMPLE_MODEL__
#define __CONVER_SIMPLE_MODEL__

#include "cv_mdlm3d.h"

////////////////////////////////////////////////////////////////////////////////
///Конвертер 3DS - SimpleModel (1 material)
////////////////////////////////////////////////////////////////////////////////
class  SmpModelConv: public M3DModelConv
{
  public:
                        SmpModelConv  (Pool*,Object3D&,BOOL _Strip = FALSE,const vector3d_t& axis = vector3d_t (0));
            virtual     ~SmpModelConv ();

////////////////////////////////////////////////////////////////////////////////
///Сохранение модели
////////////////////////////////////////////////////////////////////////////////
    virtual void        Save          (const char*,ostream* = NULL);

  protected:
            BOOL                mStripEnable;
};


#endif