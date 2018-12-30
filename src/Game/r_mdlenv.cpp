#include "pch.h"
#include "r_mdlenv.h"
#include "r_sbuffer.h"
#include "r_main.h"

#include <graph\utils.h>
#include <string.h>

void       ModelEnv::bind             (MainRender* render)
{
  D_Try
  {
    mMainRender = render;
//    mMatEnv     = render->GetMatEnv ();

    for (int i=0;i<MODEL_KEEPERS_NUM;i++)
      if (mKeeper [i])
        mKeeper [i]->bind (render);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
  }
}

uint       ModelEnv::GetPgCount       ()
{
  uint pgCount = 0;

  D_Try
  {
    for (int i=0;i<MODEL_KEEPERS_NUM;i++)
      if (mKeeper [i])
        pgCount += mKeeper [i]->GetTPS ();
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
  }

  return pgCount;
}

void       ModelEnv::InsertModel      (model_t* mdl)
{
  D_Try
  {  
    mModelMap->InsertModel (mdl);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at InsertModel "<<endl;
    #endif
  }
}

void       ModelEnv::ExtractModel     (model_t* mdl)
{
  D_Try
  {
    mModelMap->ExtractModel (mdl);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at ExtractModel "<<endl;
    #endif
  }
}

void       ModelEnv::SetSBuffer       (const SBuffer* sb)
{
  mSBuffer = (SBuffer*)sb;
}

void       ModelEnv::SetCamera        (const Camera& cam)
{
  mCamera = cam;
}

void       ModelEnv::SetFrustum       (const frustum_t& f)
{
  mFrustum = f;
}

MODELDATA     ModelEnv::LoadModelData    (int type,BOOL _AutoLoad,...)
{
  D_Try
  {
    if (type == -1)
    {
      MODELDATA res = -1;

      for (int i=0;i<MODEL_KEEPERS_NUM;i++)
        if (mKeeper [i])
        {
          va_list list;
          va_start (list,_AutoLoad);
      
          res = mDataCache->CreateData (mKeeper [i],_AutoLoad,list);

          va_end   (list);      
          
          if (res != -1)
            return res;
        }  

      return -1;
    } 
    else if (type < 0 || type >= MODEL_KEEPERS_NUM || !mKeeper [type])
      return -1;    

    va_list list;
    va_start (list,_AutoLoad);

    MODELDATA res = mDataCache->CreateData (mKeeper [type],_AutoLoad,list);

    va_end   (list);

    return res;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at LoadModelData "<<endl;
    #endif  
    return -1;
  }
}

void          ModelEnv::UnloadModelData  (MODELDATA md)
{
  D_Try
  {     
    mDataCache->DeleteData (md);    
  } 
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {           
    #ifdef DEBUG
      dout<<"ModelEnv: Error at UnloadModelData "<<endl;
    #endif
  }
}

MODEL         ModelEnv::CreateModel      (MODELDATA md,...)
{
  D_Try
  {
    ModelKeeper* owner = NULL;
    size_t       size  = 0;

    if (md == -1 || !mDataCache->GetProperties (md,owner,size))
      return -1;

    model_t* mdl = mModelDescPool.alloc ();

    if (!mdl)
      return -1;

    mdl->owner       = owner;
    mdl->modelData   = md;
    mdl->data.data   = NULL;
    mdl->data.size   = 0;
    mdl->pos.pos     = vector3d_t (.0f);
    mdl->pos.box.min = mdl->pos.box.max = vector3d_t (.0f);    
  
    mdl->pos.node    = NULL;
    mdl->pos.tile    = -1;

    va_list list;
    va_start (list,md);

    if (!owner->InitModel (mdl,md,list))
    {
      mModelDescPool.free (mdl);
      return -1;
    }
    
    va_end   (list);

    if (!owner->InsertModel (mdl))
    {
      owner->DoneModel (mdl);
      mModelDescPool.free  (mdl);
      return -1;
    } 

    return mModelDescPool.index (mdl);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at CreateModel "<<endl;
    #endif
    return -1;
  }
}

MODEL         ModelEnv::DuplicateModel   (const MODEL model)
{
  D_Try
  {
    if (model < 0 || model >= mModelDescPool.num ())
      return -1;

    model_t* src = mModelDescPool.data (model);

    if (!src) return -1;

    model_t* mdl = mModelDescPool.alloc ();

    if (!mdl)
      return -1;
    
    if (!src->owner->InitModel (src,mdl))
    {
      mModelDescPool.free (mdl);      
      return -1;
    }   

    mdl->pos.node  = NULL;
    mdl->pos.tile  = -1;

    if (!src->owner->InsertModel (mdl))
    {
      src->owner->DoneModel (mdl);
      mModelDescPool.free   (mdl);
      return -1;
    } 

    return mModelDescPool.index (mdl);    
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at Duplicate model "<<endl;
    #endif    
    return -1;
  }
}

void       ModelEnv::DeleteModel      (MODEL model)
{
  D_Try
  {
    if (model < 0 || model >= mModelDescPool.num ())
      return ;

    model_t* mdl = mModelDescPool.data (model);

    mModelMap->ExtractModel  (mdl);
    mdl->owner->ExtractModel (mdl);
    mdl->owner->DoneModel    (mdl);
    mModelDescPool.free          (mdl);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at DeleteModel "<<endl;
    #endif
  }
}

void       ModelEnv::DeleteModel      (model_t* mdl)
{
  D_Try
  {
    if (mdl)
    {
      mModelMap->ExtractModel  (mdl);
      mdl->owner->ExtractModel (mdl);
      mdl->owner->DoneModel    (mdl);
      mModelDescPool.free      (mdl);
    }  
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at DeleteModel "<<endl;
    #endif
  }
}

void       ModelEnv::ChangeProperties (MODEL model,...)
{
  D_Try
  {
    if (model < 0 || model >= mModelDescPool.num ())
      return ;

    va_list  list;
    model_t* mdl = mModelDescPool.data (model); 

    if (IsValid (mdl))
    {
      va_start (list,model);
  
      mdl->owner->ChangeProperties (mModelDescPool.data (model),list);

      va_end   (list);
    }  
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at ChangeProperties (model: "<<model<<")"<<endl;
    #endif
  }
}

void       ModelEnv::SetPosition      (MODEL model,const vector3d_t& v)
{
  D_Try
  {
    if (model < 0 || model >= mModelDescPool.num ())
      return ;

    model_t* mdl = mModelDescPool.data (model);

    mdl->owner->SetPosition (mdl,v);
    mModelMap->InsertModel  (mdl);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at SetPosition "<<endl;
    #endif
  }
}

void       ModelEnv::SetBoundBox      (MODEL model,const boundbox_t& box)
{
  D_Try
  {
    if (model < 0 || model >= mModelDescPool.num ())
      return ;

    model_t* mdl = mModelDescPool.data (model);

    mdl->owner->SetBoundBox (mdl,box);
    mModelMap->InsertModel  (mdl);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at SetBoundBox "<<endl;
    #endif
  }
}

MODELKEEPER       ModelEnv::BindKeeper       (ModelKeeper* kpr)
{
  D_Try
  {
    if (!kpr)
      return -1;

    int type = -1;

    for (int i=0;i<MODEL_KEEPERS_NUM;i++)
      if (!mKeeper [i])
        type = i;

    if (type < 0 || type >= MODEL_KEEPERS_NUM)
      return -1;
  
    mKeeper [type]        = kpr;
    mKeeper [type]->mType = type;

    return type;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at BindKeeper "<<this<<endl;
    #endif
    return -1;
  }
}

void       ModelEnv::UnbindKeeper     (int type)
{
  D_Try
  {
    if (type < 0 || type >= MODEL_KEEPERS_NUM || !mKeeper [type])
      return;

    if (mKeeper [type])
    {
      mKeeper [type]->Done ();
    }  

    mKeeper [type] = NULL;    
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at UnbindKeeper "<<endl;
    #endif
  }
}

BOOL       ModelEnv::IsPresent        (int type)
{
  D_Try
  {
    if (type < 0 || type >= MODEL_KEEPERS_NUM)
      return FALSE;

    return mKeeper [type] != NULL;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at IsPresent "<<endl;
    #endif
    return FALSE;
  }
}

void       ModelEnv::insert (int x,int y,int tile,int detail)
{
  D_Try
  {
    mdlmapnode_t* iter = mModelMap->get (x,y);

    if (!iter)
      return;

    for (;iter && mVisPos < mVisNum;iter = iter->next)
      if (mSBuffer->check (iter->model->pos.box) && IsInBound (iter->model->pos.box,6,mFrustum.clip))
      {
        modelnode_t& node     = mVisNode [mVisPos++];
        int          type     = iter->model->owner->type ();

        node.model            = iter->model;
        node.detail           = detail;
        node.next             = mKprDraw [type];
        mKprDraw [type]       = &node;
      }    
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {           
    #ifdef DEBUG
      dout<<"ModelEnv: Error at insert "<<endl;
    #endif
  }
}

void       ModelEnv::reset            ()
{
  D_Try
  {
    mVisPos = 0;

    memset (mKprDraw,0,sizeof (modelnode_t*) * MODEL_KEEPERS_NUM);

    mModelMap->reset ();
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at reset "<<endl;
    #endif
  }
}

void       ModelEnv::render           ()
{
  D_Try
  {
    for (int i=0;i<MODEL_KEEPERS_NUM;i++)
      if (mKprDraw [i] && mKeeper [i])
      {      
        mKeeper [i]->RenderScene (mKprDraw [i],mCamera);
        mKprDraw [i] = NULL;
      }  
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at render "<<endl;
    #endif
  }
}

const bound_t   ModelEnv::GetBound         (MODEL model)
{  
  D_Try
  {
    if (model >=0 && model < mModelDescPool.num ())
    {
      model_t* mdl = mModelDescPool.data (model);

      if (mdl) return mdl->owner->GetBound (mdl);
    }

    static bound_t error;

    error.box.min  = error.box.max = error.sphere.center = vector3d_t (.0f);
    error.sphere.r = 0; 

    return error;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at GetBound "<<endl;
    #endif 

    static bound_t error;

    error.box.min  = error.box.max = error.sphere.center = vector3d_t (.0f);
    error.sphere.r = 0; 

    return error;
  }  
}

void            ModelEnv::RenderShadow     (MODEL model,const lightsrc_t& light)
{
  D_Try
  {
    if (model >= 0 && model < mModelDescPool.num ())
    {
      model_t* mdl = mModelDescPool.data (model);

      if (mdl) mdl->owner->RenderShadow (mdl,light);
    }
  }
  D_Exc ("ModelEnv: Error at RenderShadow");
}

ModelEnv::ModelEnv  
 ( Pool*         _Pool,
   LandscapeMap* _Map,
   uint          _ModelDataNum,
   uint          _ModelNum,             
   uint          _VisNum,
   size_t        _DataCacheSize,
   size_t        _ModelPoolSize
 )
         : MemObject   (_Pool),
           mLandMap    (_Map),
           mModelMap   (NULL),
           mDataCache  (NULL),
           mModelDescPool  (_Pool,_ModelNum),
           mModelPool  (_Pool,_ModelPoolSize),
           mVisNode    (NULL),
           mVisNum     (_VisNum),
           mVisPos     (0),
           mSBuffer    (NULL),
           mMainRender (NULL)
{
  D_Try
  {
    if (!IsValid (&mModelDescPool) || !IsValid (&mModelPool))
    {
      D_Message ("ModelEnv: No enough memory for model pools");
      return;
    }

    mDataCache  = new (GetPool()) ModelDataCache (GetPool(),_DataCacheSize,_ModelDataNum); 
    mModelMap   = new (GetPool()) ModelMap (GetPool (),this,RENDER_MODEL_MAP_SHIFT);
    mVisNode    = (modelnode_t*)GetPool()->alloc (sizeof (modelnode_t) * mVisNum);

    if (!mVisNode || !mDataCache || !mModelMap)
    {  
      D_Message ("ModelEnv: No enough memory for sys data");
      return;
    }

    memset (mKeeper,0,sizeof (ModelKeeper*) * MODEL_KEEPERS_NUM);
    reset  ();   

    for (int i=0;i<_ModelNum;i++)
    {
      model_t* mdl   = mModelDescPool.data (i);

      mdl->owner     = NULL;
      mdl->modelData = -1;
    }  
  }
  D_Exc ("ModelEnv: Error at construct");
}

ModelEnv::~ModelEnv ()
{
  D_Try
  {
    if (mVisNode)       GetPool()->free (mVisNode);
    if (mDataCache)     delete mDataCache;
    if (mModelMap)      delete mModelMap;
  }
  D_Exc ("ModelEnv: Error at destruct");
}
