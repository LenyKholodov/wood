#ifndef __RENDER_SIMPLE_MODEL__
#define __RENDER_SIMPLE_MODEL__

#include "r_mdlmulti.h"

////////////////////////////////////////////////////////////////////////////////
///Хранилище простых моделей (один материал)
////////////////////////////////////////////////////////////////////////////////
class    SimpleModelKeeper: public MultiModelKeeper
{
  public:
                        SimpleModelKeeper   (Pool*,ModelEnv*,int _MeshesNum);
            virtual     ~SimpleModelKeeper  ();

  protected:
////////////////////////////////////////////////////////////////////////////////
///Рендеринг
////////////////////////////////////////////////////////////////////////////////
    virtual void           render           (const modelnode_t*,const Camera&);
    virtual void           RenderShadow     (const model_t*,const lightsrc_t&);
//    virtual void           DrawAlpha        (DWORD param = 0);
};

#endif