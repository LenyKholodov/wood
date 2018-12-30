#ifndef __RENDER_SIMPLE_ANIM_MODEL__
#define __RENDER_SIMPLE_ANIM_MODEL__

#include "r_mdl3d.h"
#include "r_meshrender.h"

#include <graph\x3f.h>

struct  animnode_t
{        
  bound_t       bound;          //Ограничивающее тело для объекта
  int           parent;         //Родитель
  int           node;           //Первый узел
  int           next;           //Следующий объект на данном уровне иерархии

  struct  group3d_t
  {
    nodelist_t    vertex;
    nodelist_t    facet;
    nodelist_t    normal;
  };

  struct 
  {
    int    flags;      //Rotate|Translate|Scale;
    int    node;       //Индекс кластера с анимацией для объекта
  }anim;  
};

struct   smpmodeldata_t
{
  int      material;    //Материал
  int      root;        //Корневой узел
  bound_t  bound;       //Ограничивающие тела

  struct lump_t
  {
    ulong       offs;   //Смещение данных (от data [])
    ulong       num;    //Количество элементов
    ulong       size;   //Размер данных
  }vertex, normal, facet, mat;

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
     virtual    uint       filetype      () const { return SMP_MODEL_ID; }

////////////////////////////////////////////////////////////////////////////////
///Работа с моделью
////////////////////////////////////////////////////////////////////////////////
    virtual     BOOL       InitModel        (model_t*,MODELDATA,va_list);

////////////////////////////////////////////////////////////////////////////////
///Рендеринг
////////////////////////////////////////////////////////////////////////////////
    virtual void           render           (const modelnode_t*,const Camera&);

  protected:
        MeshRender         mMeshRender;
};

#endif