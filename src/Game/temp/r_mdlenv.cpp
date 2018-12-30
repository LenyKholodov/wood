#include "r_mdlenv.h"
#include "r_sbuffer.h"

#include <graph\utils.h>

void       ModelEnv::SetEnvFn         (SetMaterialFn smfn,ChangeEnvFn chfn,DWORD par)
{
  mParam          = par;
  mSetMaterial    = smfn;
  mChangeEnv      = chfn;
}

void       ModelEnv::SetMaterial      (int mat)
{
  if (mSetMaterial) (*mSetMaterial)(mParam,mat);
}

void       ModelEnv::ChangeEnv        (uint tile,uint _InterruptDrawing)
{
  if (mChangeEnv) (*mChangeEnv)(mParam,tile,_InterruptDrawing);
}

void       ModelEnv::InsertModel      (MODEL mdl)
{
  D_Try
  {
    if (mdl->tile != -1)
    {
      if (mdl->prevModel) mdl->prevModel->nextModel = mdl->nextModel;
      else                mMap [mdl->tile]          = mdl->nextModel;
      if (mdl->nextModel) mdl->nextModel->prevModel = mdl->prevModel;
    }

    int tile = int (mdl->pos.z / mLandMap->step) * mLandMap->width +
               int (mdl->pos.x / mLandMap->step);
    
    mdl->nextModel = mMap [tile];
    mdl->prevModel = NULL;
    mMap [tile]    = mdl;

    if (mdl->nextModel) mdl->nextModel->prevModel = mdl;

    mdl->tile      = tile;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at InsertModel"<<endl;
    #endif
  }
}

void       ModelEnv::ExtractModel      (MODEL mdl)
{
  D_Try
  {
    if (mdl->tile != -1)
    {
      if (mdl->prevModel) mdl->prevModel->nextModel = mdl->nextModel;
      else                mMap [mdl->tile]          = mdl->nextModel;
      if (mdl->nextModel) mdl->nextModel->prevModel = mdl->prevModel;
    }
    
    mdl->nextModel = mdl->prevModel = NULL;
    mdl->tile      = -1;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at ExtractModel"<<endl;
    #endif
  }
}

MODEL      ModelEnv::CreateModel      (int type,...)
{
  D_Try
  {
    if (type < 0 || type >= MODEL_KEEPERS_NUM || !mKeeper [type])
      return NULL;

    mdlnode_t* node = mFirstFree;

    if (!node)
      return NULL;

    mFirstFree   = node->next;
    MODEL mdl    = &node->model;

    mdl->data    = NULL;
    mdl->prop    = NULL;
    mdl->type    = type;
    mdl->owner   = mKeeper [type];
    mdl->memNode = node;
    mdl->pos     = vector3d_t (0);
    mdl->m       = matrix_t (1);
    mdl->box.min = mdl->box.max = vector3d_t (0);
  
    mdl->nextModel = mdl->prevModel = NULL;
    mdl->tile      = -1;

    va_list list;
    va_start (list,type);    

    if (!mKeeper [type]->InitModel (mdl,list))
    {
      node->next = mFirstFree;
      mFirstFree = node;

      return NULL;
    }

    InsertModel (mdl);
    
    va_end   (list);

    return mdl;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at CreateModel "<<endl;
    #endif
    return NULL;
  }
}

MODEL      ModelEnv::DuplicateModel   (const MODEL src)
{
  D_Try
  {
    if (!src) return NULL;

    mdlnode_t* node = mFirstFree;

    if (!node)
      return NULL;

    mFirstFree   = node->next;
    MODEL mdl    = &node->model;
    
    if (!mKeeper [src->type]->InitModel (src,mdl))
    {
      node->next = mFirstFree;
      mFirstFree = node;

      return NULL;
    }   

    mdl->memNode   = node;
    mdl->nextModel = mdl->prevModel = NULL;
    mdl->tile      = -1;

    return mdl;    
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at Duplicate model "<<endl;
    #endif    
    return NULL;
  }
}

void       ModelEnv::DeleteModel      (MODEL mdl)
{
  D_Try
  {
    ExtractModel          (mdl);
    mdl->owner->DoneModel (mdl);

    mdlnode_t* node = (mdlnode_t*)mdl->memNode;

    node->next      = mFirstFree;
    mFirstFree      = node;    
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at DeleteModel "<<endl;
    #endif
  }
}

void       ModelEnv::ChangeProperties (MODEL mdl,...)
{
  D_Try
  {
    va_list list;

    va_start (list,mdl);

    mdl->owner->ChangeProperties (mdl,list);

    va_end   (list);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at ChangeProperties "<<endl;
    #endif
  }
}

void       ModelEnv::SetPosition      (MODEL mdl,const vector3d_t& v)
{
  D_Try
  {
    mdl->owner->SetPosition (mdl,v);
    InsertModel             (mdl);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at SetPosition "<<endl;
    #endif
  }
}

void       ModelEnv::SetTransMatrix   (MODEL mdl,const matrix_t& m)
{
  D_Try
  {
    mdl->owner->SetTransMatrix (mdl,m);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at SetTransMatrix "<<endl;
    #endif
  }
}

void       ModelEnv::SetBoundBox      (MODEL mdl,const boundbox_t& box)
{
  D_Try
  {
/*    int x1    = mdl.box.min.x / mLandMap->step,
        y1    = mdl.box.min.z / mLandMap->step;
    int x1    = mdl.box.max.x / mLandMap->step,
        y1    = mdl.box.max.z / mLandMap->step;
    int width = mLandMap->width;
    int index = y1 * width + x1;      

    for (int y=y1;y<=y2;y++,index += w - x2 + x1)
      for (int x=x1;x<x2;x++,index++)*/
              

    mdl->owner->SetBoundBox (mdl,box);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at SetBoundBox "<<endl;
    #endif
  }
}

BOOL       ModelEnv::BindKeeper       (int type,ModelKeeper* kpr)
{
  D_Try
  {
    if (type < 0 || type >= MODEL_KEEPERS_NUM)
      return FALSE;

    if (mKeeper [type])
      mKeeper [type]->Done ();

    mKeeper [type] = kpr;

    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at BindKeeper "<<this<<endl;
    #endif
    return FALSE;
  }
}

void       ModelEnv::UnbindKeeper     (int type)
{
  D_Try
  {
    if (type < 0 || type >= MODEL_KEEPERS_NUM)
      return;

    if (mKeeper [type])
      mKeeper [type]->Done ();

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

BOOL      ModelEnv::MoveData         (void* _From,void* _To,DWORD par)
{
  D_Try
  {
    memnode_t* src  = (memnode_t*)_From;
    memnode_t* dest = (memnode_t*)_To;

    *src->node      = dest->data;

    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at MoveData "<<endl;
    #endif
    return FALSE;
  }
}

void*      ModelEnv::AllocModelData   (void** ptr,size_t size)
{
  D_Try
  {
    size += sizeof (memnode_t);

    if (mDefragPool->memavail () < size || !ptr)
      return NULL;
      
    memnode_t* data = (memnode_t*)mDefragPool->alloc (size,MoveData);

    if (!data)
    {
      mDefragPool->defrag ();
      data = (memnode_t*)mDefragPool->alloc (size,MoveData);
    }

    data->node = ptr; 

    if (!data)      return NULL;
    else            return *ptr = data->data;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at AllocModelData "<<endl;
    #endif
    return NULL;
  }    
}

void       ModelEnv::FreeModelData    (void** ptr)
{
  D_Try
  {
    if (ptr)
    {
      mDefragPool->free (*ptr);
      *ptr = NULL;
    }  
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at FreeModelData "<<endl;
    #endif
  }  
}

void       ModelEnv::insert           (const SBuffer& _sb,int tile,int detail)
{
  D_Try
  {
    if (!mMap [tile])
      return;

    SBuffer& sb = (SBuffer&)_sb;

    for (MODEL mdl = mMap [tile];mdl && mVisPos < mVisNum;mdl = mdl->nextModel)
      if (sb.check (mdl->box) && IsInBound (mdl->box,6,mFrustum.clip))
      {
        modelnode_t& node     = mVisNode [mVisPos++];
     
        node.model            = mdl;
        node.detail           = detail;
        node.next             = mKprDraw [mdl->type];
        mKprDraw [mdl->type]  = &node;
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
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at reset "<<endl;
    #endif
  }
}

void       ModelEnv::set              (const frustum_t& f)
{
  mFrustum = f;
}

void       ModelEnv::render           ()
{
  D_Try
  {
    for (int i=0;i<MODEL_KEEPERS_NUM;i++)
      if (mKprDraw [i])
        mKeeper [i]->RenderScene (mKprDraw [i]);
    reset ();
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelEnv: Error at render "<<endl;
    #endif
  }
}

ModelEnv::ModelEnv  (Pool* _Pool,LandscapeMap* _Map,uint _ModelNum,uint _VisNum)
         : MemObject   (_Pool),
           mLandMap    (_Map),
           mDefragPool (NULL),
           mMap        (NULL),
           mVisNode    (NULL),
           mVisNum     (_VisNum),
           mVisPos     (0),
           mParam      (0),
           mSetMaterial(NULL),
           mChangeEnv  (NULL),
           mModelNode  (NULL),
           mFirstFree  (NULL)
{
  D_Try
  {
    mMap        = (MODEL*)pool()->alloc (sizeof (MODEL) * mLandMap->width * mLandMap->height);
    mVisNode    = (modelnode_t*)pool()->alloc (sizeof (modelnode_t) * mVisNum);
    mModelNode  = (mdlnode_t*)pool()->alloc (sizeof (mdlnode_t) * _ModelNum);
    mFirstFree  = mModelNode;

    if (!mMap || ! mVisNode || !mModelNode)
    {  
      #ifdef DEBUG
        dout<<"ModelEnv: No enough memory for sys data "<<endl;
      #endif
      return;
    }

    memset (mMap,0,sizeof (MODEL) * mLandMap->width * mLandMap->height);
    memset (mKeeper,0,sizeof (ModelKeeper*) * MODEL_KEEPERS_NUM);
    reset  ();
    
    for (int i=0;i<_ModelNum;i++)
      mModelNode [i].next   = &mModelNode [i+1];

    mModelNode [i-1].next = NULL;

    mDefragPool = new (pool()) DefragPool (pool());

    if (!mDefragPool)
    {
      #ifdef DEBUG
        dout<<"ModelEnv: No enough memory for defrag pool "<<endl;
      #endif
      return;      
    }
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {           
    #ifdef DEBUG
      dout<<"ModelEnv: Error at construct "<<endl;
    #endif
  }
}

ModelEnv::~ModelEnv ()
{
  D_Try
  {
    if (mDefragPool)    delete mDefragPool;
    if (mMap)           pool()->free (mMap);
    if (mVisNode)       pool()->free (mVisNode);
    if (mModelNode)     pool()->free (mModelNode);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {           
    #ifdef DEBUG
      dout<<"ModelEnv: Error at destruct "<<endl;
    #endif
  }
}
