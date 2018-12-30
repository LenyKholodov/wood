#ifndef __RENDER_SIMPLE_MODEL__
#define __RENDER_SIMPLE_MODEL__

#include "r_mdl3d.h"
#include "r_meshrender.h"

struct   smpmodeldata_t
{
  int      material;    //Материал
  bound_t  bound;       //Ограничивающие тела
  int      type;        //MODEL_TRIANGLES_DATA / MODEL_STRIP_DATA

  struct lump_t
  {
    ulong       offs;   //Смещение данных (от data [])
    ulong       num;    //Количество элементов
    ulong       size;   //Размер данных
  }vertex, normal, facet, mat, strip, tcoord;

  char     data [];
};

struct  smpmodel_t: public m3dmodel_t
{
  textmat_t     mat;            //Материал
  int           texture;        //Индекс распределённой текстуры
  int           list;           //Индекс распределённого списка
  ulong         crc;            //CRC имени текстуры
};

////////////////////////////////////////////////////////////////////////////////
///Создание модели
///InitModel: pos: vector3d_t
///ChangeProperties: type:(MODEL_MULT_MATRIX,MODEL_CHANGE_MATRIX), matrix_t
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
///Хранилище простых моделей (один материал)
////////////////////////////////////////////////////////////////////////////////
class    SimpleModelKeeper: public M3DModelKeeper
{
  public:
                        SimpleModelKeeper   (Pool*,ModelEnv*,int _MeshesNum);
            virtual     ~SimpleModelKeeper  ();

  protected:
     virtual    int        filetype      () const { return SMP_MODEL_ID; }

////////////////////////////////////////////////////////////////////////////////
///Работа с моделью
////////////////////////////////////////////////////////////////////////////////
    virtual     BOOL       InitModel        (model_t*,MODELDATA,va_list);

////////////////////////////////////////////////////////////////////////////////
///Рендеринг
////////////////////////////////////////////////////////////////////////////////
    virtual void           render           (const modelnode_t*,const Camera&);
    virtual void           RenderShadow     (const model_t*);
//    virtual void           DrawAlpha        (DWORD param = 0);

  protected:
        MeshRender         mMeshRender;
};

#endif