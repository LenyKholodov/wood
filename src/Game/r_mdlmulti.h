#ifndef __RENDER_MULTI_MODEL__
#define __RENDER_MULTI_MODEL__

#include "r_mdl3d.h"
#include "r_meshrender.h"
#include <misc.h>

struct   multimodeldata_t
{
  bound_t  bound;       //Ограничивающие тела

  struct lump_t
  {
    ulong       offs;   //Смещение данных (от data [])
    ulong       num;    //Количество элементов
    ulong       size;   //Размер данных
  }vertex, normal, facet, mesh, matname;

  char     data [];
};

struct  multimodel_t: public m3dmodel_t
{
  int              num;        

  ptroffs_t<int>   listTable;
  ptroffs_t<int>   reindex;

  char             data [];
};

////////////////////////////////////////////////////////////////////////////////
///Создание модели (аналогично SmpModel)
///InitModel: pos: vector3d_t
///ChangeProperties: type:(MODEL_MULT_MATRIX,MODEL_CHANGE_MATRIX), matrix_t
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
///Хранилище простых моделей (один материал)
////////////////////////////////////////////////////////////////////////////////
class    MultiModelKeeper: public M3DModelKeeper
{
  public:
                        MultiModelKeeper   (Pool*,ModelEnv*,int _MeshesNum);
            virtual     ~MultiModelKeeper  ();

  protected:
     virtual    int        filetype      () const { return MULTI_MODEL_ID; }

////////////////////////////////////////////////////////////////////////////////
///Работа с моделью
////////////////////////////////////////////////////////////////////////////////
    virtual     BOOL       InitModel        (model_t*,MODELDATA,va_list);

////////////////////////////////////////////////////////////////////////////////
///Рендеринг
////////////////////////////////////////////////////////////////////////////////
    virtual void           render           (const modelnode_t*,const Camera&);
    virtual void           RenderShadow     (const model_t*,const lightsrc_t&);
    virtual void           DrawAlpha        (DWORD,const Camera&);

  protected:
        MeshRender         mMeshRender;
        MATGROUP           mMatGroup;
};

#endif