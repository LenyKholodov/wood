#include "pch.h"
#include "r_mdleffect.h"
#include "r_mdlenv.h"

BOOL       EffectModelKeeper::InitPosition     
 ( model_t*             mdl,
   const boundbox_t&    box,
   const vector3d_t&    pos
 )
{
  D_Try
  {
    mdl->pos.pos     = pos;
    mdl->pos.box.min = box.min + pos;
    mdl->pos.box.max = box.max + pos;

    mModelEnv->InsertModel (mdl);
      
    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"EffectModelKeeper: Error at InitPosition "<<endl;
    #endif
    return FALSE;
  }
}

void       EffectModelKeeper::ChangeProperties (model_t* mdl,va_list list)
{
  D_Try
  {
    switch (va_arg (list,uint))
    {
      case EFFECT_TICK: tick (mdl); break;
    }
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"EffectModelKeeper: Error at ChangeProperties"<<endl;
    #endif
  }
}

void       EffectModelKeeper::SetPosition      (model_t* mdl,const vector3d_t& v)
{
  vector3d_t   oldPos = mdl->pos.pos;
  vector3d_t   d      = v - oldPos;

  ModelKeeper::SetPosition (mdl,v);
  
  mdl->pos.box.min  += d;
  mdl->pos.box.max  += d;
}

EffectModelKeeper::EffectModelKeeper   (Pool* pool,ModelEnv* env)
                  : ModelKeeper (pool,env)
{   }

EffectModelKeeper::~EffectModelKeeper  ()
{   }

