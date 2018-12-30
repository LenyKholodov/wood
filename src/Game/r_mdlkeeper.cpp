#include "pch.h"
#include "r_mdlkeeper.h"
#include "r_mdlenv.h"

#include <string.h>

BOOL  ModelKeeper::InsertModel      (const model_t* mdl) { return mModelRing.insert ((model_t*)mdl); }
void  ModelKeeper::ExtractModel     (const model_t* mdl) { mModelRing.erase ((model_t*)mdl); }
void  ModelKeeper::SetPosition      (model_t* mdl,const vector3d_t& v) { mdl->pos.pos = v; }
void  ModelKeeper::SetBoundBox      (model_t* mdl,const boundbox_t& box) { mdl->pos.box = box; }
void* ModelKeeper::LockModelData    (MODELDATA x) { return mModelEnv->mDataCache->LockData (x); }
void  ModelKeeper::UnlockModelData  (MODELDATA x) { mModelEnv->mDataCache->UnlockData (x); }
const   frustum_t&     ModelKeeper::GetFrustum       () const {return mModelEnv->mFrustum;}
const   Camera&        ModelKeeper::GetCamera        () const {return mModelEnv->mCamera;}

ModelPool* ModelKeeper::GetModelPool   ()  { return mModelEnv->GetModelPool (); }

const bound_t  ModelKeeper::GetBound     (model_t* mdl)
{
  if (!IsValid (mdl))
  {
    static bound_t error;

    error.box.min  = error.box.max = error.sphere.center = vector3d_t (.0f);
    error.sphere.r = 0; 

    return error;
  }  
  else
  {
    bound_t b;

    b.box           = mdl->pos.box;
    b.sphere.center = (b.box.min + b.box.max) / 2.0f;
    b.sphere.r      = length (b.box.max - b.box.min) / 2.0f;

    return b;
  }
}

BOOL      ModelKeeper::InitModel       (const model_t* src,model_t* dest)
{
  D_Try
  {
    memcpy (dest,src,sizeof (model_t));

    if (src->data.size)
    {
      dest->data.data = GetModelPool ()->alloc (dest,src->data.size);
      dest->data.size = src->data.size;

      if (!dest->data.data)
      {
        dest->data.size = 0;
        return FALSE;
      }

      memcpy (dest->data.data,src->data.data,src->data.size);
    }

    dest->pos.node = NULL;
    dest->pos.tile = -1;

    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {           
    #ifdef DEBUG
      dout<<"ModelKeeper: Error at duplicat model "<<endl;
    #endif
    return FALSE;
  }  
}

void       ModelKeeper::RenderScene      (const modelnode_t* node,const Camera& cam)
{
  D_Try
  {
    render (node,cam);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelKeeper: Error at RenderScene "<<this<<endl;
    #endif
  }
}

void       ModelKeeper::Done  ()
{
  D_Try
  {
    MdlRing::iterator iter  = mModelRing;
    int               count = mModelRing.count ();

    for (int i=0;i<count;i++,iter++)
      mModelEnv->DeleteModel (iter);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelKeeper: Error at Done "<<endl;
    #endif
  }
}

ModelKeeper::ModelKeeper  (Pool* _Pool,ModelEnv* _MdlEnv)
            : Render      (_Pool,_MdlEnv->mMainRender),
              //MemObject   (_Pool),
              mModelEnv   (_MdlEnv),
              mModelRing  (_Pool),
              mType       (-1)//,
//              mPgCount    (0)
{   }

ModelKeeper::~ModelKeeper ()
{   
  D_Try
  {
    Done ();
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelKeeper: Error at destruct "<<endl;
    #endif
  }
}

BOOL    IsValid (model_t* mdl)
{
  D_Try
  {
    return mdl != NULL && mdl->owner != NULL && mdl->modelData != -1;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    return FALSE;
  }
}