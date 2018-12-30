#ifndef __RENDER_MODEL_KEEPER__
#define __RENDER_MODEL_KEEPER__

#include <stdarg.h>
#include <pool.h>

#include "defs.h"

class    ModelKeeper;

struct   model_t
{
  int           type;           //Тип 
  vector3d_t    pos;            //Позиция модели
  matrix_t      m;              //Матрица преобразований для модели
  boundbox_t    box;            //Ограничивающий куб
  ModelKeeper*  owner;          //Владелец
  int           tile;           //Тайл модели
  model_t       *nextModel, 
                *prevModel;     //Список моделей в ячейке карты
  void*         memNode;        //Структура в пуле
  size_t        propSize,
                dataSize;       //Размеры
  void*         prop;          //Указатель на свойства
  void*         data;          //Указатель на данные
};

typedef model_t*        MODEL;

struct  modelnode_t
{
  modelnode_t*  next;
  MODEL         model;
  int           detail;
};

////////////////////////////////////////////////////////////////////////////////
///Абстрактное хранилище моделей
////////////////////////////////////////////////////////////////////////////////
class   ModelEnv;
class   ModelKeeper: public MemObject 
{
  public:
                        ModelKeeper  (Pool*,ModelEnv*);
        virtual         ~ModelKeeper ();

////////////////////////////////////////////////////////////////////////////////
///Работа с моделями
////////////////////////////////////////////////////////////////////////////////
                BOOL       InitModel        (MODEL,va_list);
                BOOL       InitModel        (const MODEL,MODEL);
                void       DoneModel        (MODEL);
    virtual     void       ChangeProperties (MODEL,va_list) {}

    virtual     void       SetPosition      (MODEL,const vector3d_t&);
    virtual     void       SetTransMatrix   (MODEL,const matrix_t&);        
    virtual     void       SetBoundBox      (MODEL,const boundbox_t&);

////////////////////////////////////////////////////////////////////////////////
///Рендеринг
////////////////////////////////////////////////////////////////////////////////
                void       RenderScene      (modelnode_t* node);

////////////////////////////////////////////////////////////////////////////////
///Доступ
////////////////////////////////////////////////////////////////////////////////
                void       Done             ();

  protected:
            BOOL           AllocModelData   (MODEL,size_t _PropSize,size_t _DataSize);
            void           FreeModelData    (MODEL model);

    virtual BOOL           _InitModel       (MODEL,va_list) { return TRUE; }
    virtual BOOL           _InitModel       (const MODEL,MODEL);
    virtual void           _DoneModel       (MODEL) {}

    virtual void           render           (modelnode_t*) = 0;

    typedef RingTmpl<MODEL> MdlRing;

  protected:
        ModelEnv*          mModelEnv;
        MdlRing            mModelRing;
};

////////////////////////////////////////////////////////////////////////////////
///Test keeper
////////////////////////////////////////////////////////////////////////////////
class   TestKeeper: public ModelKeeper
{
  public:
                        TestKeeper  (Pool*,ModelEnv*);
        virtual         ~TestKeeper ();

////////////////////////////////////////////////////////////////////////////////
///Работа с моделями
////////////////////////////////////////////////////////////////////////////////
    virtual     void       SetPosition      (MODEL,const vector3d_t&);

  protected:
    virtual BOOL           _InitModel       (MODEL,va_list);

    virtual void           render           (modelnode_t*);
};

#endif