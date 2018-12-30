#ifndef __RENDER_MODEL_EFFECT__
#define __RENDER_MODEL_EFFECT__

#include "r_mdlkeeper.h"

enum EffectsId {
  MEFF_MODEL_ID = 0,
  EFFECT_WATER_ID
};

enum EffectPropertiesId {
  EFFECT_TICK = 0
};

////////////////////////////////////////////////////////////////////////////////
///Класс абстрактного эффекта
////////////////////////////////////////////////////////////////////////////////
class    EffectModelKeeper: public ModelKeeper
{
  public:
                        EffectModelKeeper   (Pool*,ModelEnv*);
            virtual     ~EffectModelKeeper  ();

  protected:
     virtual    int        filetype      () const { return MEFF_MODEL_ID; }

////////////////////////////////////////////////////////////////////////////////
///Работа с моделью
////////////////////////////////////////////////////////////////////////////////
                BOOL       InitPosition     (model_t*,
                                             const boundbox_t&,
                                             const vector3d_t&
                                            );

    virtual     void       DoneModel        (model_t*) {}

    virtual     void       ChangeProperties (model_t*,va_list);
    virtual     void       SetPosition      (model_t*,const vector3d_t&);
    virtual     void       SetBoundBox      (model_t*,const boundbox_t&) {}

////////////////////////////////////////////////////////////////////////////////
///Тикер
////////////////////////////////////////////////////////////////////////////////
    virtual     void       tick             (model_t*) { }

////////////////////////////////////////////////////////////////////////////////
///Загрузка моделей
////////////////////////////////////////////////////////////////////////////////
    virtual int            GetModelDataSize (va_list src,va_list dest) { return 0; }
            
    virtual BOOL           LoadModelData    (void*,size_t,va_list) { return TRUE; }
    virtual BOOL           UnloadModelData  (void*,size_t,va_list) { return TRUE; }

////////////////////////////////////////////////////////////////////////////////
///Рендеринг теней по умолчанию отсутствует
////////////////////////////////////////////////////////////////////////////////
    virtual void           RenderShadow     (const model_t*,const lightsrc_t&)  { }
};

#endif