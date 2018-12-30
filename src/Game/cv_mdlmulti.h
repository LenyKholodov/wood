#ifndef __CONVERT_MULTI_MODEL__
#define __CONVERT_MULTI_MODEL__

#include "cv_mdlm3d.h"

typedef RingTmpl<int>    FacetRing;

////////////////////////////////////////////////////////////////////////////////
///Конвертер 3DS - MultiModel (multi - material)
////////////////////////////////////////////////////////////////////////////////
class  MultiModelConv: public M3DModelConv
{
  public:
                        MultiModelConv  (Pool*,
                                         Object3D&,
                                         const vector3d_t& axis = vector3d_t (.0f)
                                        );
            virtual     ~MultiModelConv ();

////////////////////////////////////////////////////////////////////////////////
///Сохранение модели
////////////////////////////////////////////////////////////////////////////////
   virtual  void        Save          (const char*,ostream* = NULL);

  protected:
            BOOL        BuildMeshes   ();

  private:
            FacetRing*          mMesh;
};

#endif