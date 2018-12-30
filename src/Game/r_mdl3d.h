#ifndef __RENDER_MODEL3D_PRO_KEEPER__
#define __RENDER_MODEL3D_PRO_KEEPER__

#include "r_mdlkeeper.h"

const    ulong M3D_MODEL_ID = *((ulong*)"WM3D");
const    ulong SMP_MODEL_ID = *((ulong*)"WSMP");
const    ulong MULTI_MODEL_ID = *((ulong*)"WMLT");
const    ulong SPRITE_MODEL_ID = *((ulong*)"WSPR");

const    uint  MODEL_STRIP_DATA     = 1;
const    uint  MODEL_TRIANGLES_DATA = 2;

enum ModelProperties {
  MODEL_CHANGE_MATRIX = 0,
  MODEL_MULT_MATRIX,
  MODEL_SCALE,
  MODEL_TRANSFORM_ROUND_POINT,
  MODEL_GET_MATRIX,
  MODEL_GET_POS
};

struct   m3dheader_t
{
  ulong         id;
  size_t        size;
};

struct  m3dmodel_t
{
  boundbox_t    box;            //Начальное состояние BoundBox        
  matrix_t      trans;          //Матрица преобразований в формате OpenGL
  boundbase_t   base;           //Базис для модели
  uint          pgCount;        //Количество полигонов
};

////////////////////////////////////////////////////////////////////////////////
///Хранилище простых моделей (один материал)
////////////////////////////////////////////////////////////////////////////////
class    M3DModelKeeper: public ModelKeeper
{
  public:
                        M3DModelKeeper   (Pool*,ModelEnv*);
            virtual     ~M3DModelKeeper  ();

  protected:
     virtual    int        filetype      () const { return M3D_MODEL_ID; }

////////////////////////////////////////////////////////////////////////////////
///Работа с моделью
////////////////////////////////////////////////////////////////////////////////
                BOOL       InitPosition     (model_t*,
                                             const boundbox_t&,
                                             const vector3d_t&,
                                             const matrix_t&
                                            );

    virtual     void       DoneModel        (model_t*);

    virtual     void       ChangeProperties (model_t*,va_list);
    virtual     void       SetPosition      (model_t*,const vector3d_t&);
    virtual     void       SetBoundBox      (model_t*,const boundbox_t&);

////////////////////////////////////////////////////////////////////////////////
///Загрузка моделей
////////////////////////////////////////////////////////////////////////////////
    virtual int            GetModelDataSize (va_list src,va_list dest);
            
    virtual BOOL           LoadModelData    (void*,size_t,va_list);
    virtual BOOL           UnloadModelData  (void*,size_t,va_list);

////////////////////////////////////////////////////////////////////////////////
///Рендеринг теней по умолчанию отсутствует
////////////////////////////////////////////////////////////////////////////////
    virtual void           RenderShadow     (const model_t*)  { }
};

#endif