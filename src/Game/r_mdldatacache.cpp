#include "pch.h"
#include "r_mdldatacache.h"

BOOL    ModelDataCache::GetProperties (int index,ModelKeeper*& kpr,size_t& size)
{
  D_Try
  {     
    if (index < 0 || index >= mModelDataNum)
      return FALSE;

    cachedata_t& data = mCacheData [index];

    kpr               = data.owner;
    size              = data.size;
  
    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelDataCache: Error at GetProperties "<<endl;
    #endif
    return FALSE;
  }
}

int     ModelDataCache::CreateData (ModelKeeper* kpr,BOOL _NowLoad,va_list list)
{
  D_Try
  {
    if (mFirstFree == -1 || !kpr)
      return -1;

    int           index = mFirstFree;
    cachedata_t&  data  = mCacheData [index];

    mFirstFree          = data.next;

    int           size  = kpr->GetModelDataSize (list,data.param);

    if (size < 0)
    {
      data.next  = mFirstFree;
      mFirstFree = index;    

      return -1;        
    }  

    data.node           = NULL;
    data.owner          = kpr;
    data.size           = size;
    data.use            = TRUE;

    if (_NowLoad)
    {
      LockData   (index);
      UnlockData (index);
    }  

    return index;    
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelDataCache: Error at CreateData "<<endl;
    #endif
    return -1;
  }
}

void    ModelDataCache::DeleteData (int index)
{
  D_Try
  {
    if (index < 0 || index >= mModelDataNum)
      return ;

    cachedata_t& data = mCacheData [index];    

    if (!data.use)
      return ;

    if (data.node)
    {
      if (data.owner) 
        data.owner->UnloadModelData (data.node->data,data.size,data.param);
     
      data.node->prev->next = data.node->next;
      data.node->next->prev = data.node->prev;        
      data.node->cd->node   = NULL;
      data.node->isLock     = FALSE;

      mDefragPool.free (data.node);
    }

    data.size  = 0;
    data.use   = FALSE;
    data.node  = NULL;
    data.owner = NULL;
    data.next  = mFirstFree;
    mFirstFree = index;    
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG        
      dout<<"ModelDataCache: Error at DeleteData "<<endl;
    #endif
  }
}
        
void*   ModelDataCache::LockData   (int index)
{
  D_Try
  {
    if (index < 0 || index >= mModelDataNum)
      return NULL;

    cachedata_t& data = mCacheData [index];

    if (!data.use)
      return NULL; 

    if (!data.node)
    {
      data.node = data.size ? AllocData (data.size) : NULL;

      if (!data.node && data.size)
        return NULL;

      if (data.node)
      {  
        data.node->cd     = &data;
        data.node->prev   = &mFirst;
        data.node->next   = mFirst.next;
        data.node->isLock = TRUE;
      
        mFirst.next->prev = data.node;
        mFirst.next       = data.node;

        if (data.owner) 
          data.owner->LoadModelData (data.node->data,data.size,data.param);
      }  
    }
    
    if (data.node && data.node != mFirst.next)  //Перемещение элемента в начало списка
    {
      data.node->prev->next = data.node->next;
      data.node->next->prev = data.node->prev;

      data.node->prev       = &mFirst;      
      data.node->next       = mFirst.next;
      mFirst.next           = mFirst.next->prev = data.node;

      data.node->isLock     = TRUE;
    }

    return data.node->data;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelDataCache: Error at LockData "<<endl;
    #endif
    return NULL;
  }
}

void    ModelDataCache::UnlockData (int index)
{
  D_Try
  {
    if (index < 0 || index >= mModelDataNum)
      return ;

    cachedata_t& data = mCacheData [index];    

    if (!data.use || !data.node || !data.node->isLock)
      return;    

    data.node->isLock = FALSE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelDataCache: Error at UnlockData "<<endl;
    #endif
  }
}

BOOL    ModelDataCache::MoveData   (void* _From,void* _To,DWORD)
{
  cachenode_t* src  = (cachenode_t*)_From;
  cachenode_t* dest = (cachenode_t*)_To;

  dout<<"ModeldataCache: Move data!"<<endl;

  if (src->isLock)
    return FALSE;

  src->next->prev       = dest;
  src->prev->next       = dest;
  src->cd->node         = dest;

  return TRUE;
}

ModelDataCache::cachenode_t*   ModelDataCache::AllocData  (size_t size)
{
  size += sizeof (cachenode_t);

  if (size > mDefragPool.size ())
  {
    #ifdef DEBUG
      dout<<"ModelDataCache: Pool size < data size: "<<mDefragPool.size ()<<" "<<size<<endl;
    #endif
    return NULL;
  }

  size_t free = mDefragPool.memavail ();

  if (free < size)
  {
    cachenode_t* prev = mLast.prev->prev;

    for (cachenode_t* data = mLast.prev;data!=&mFirst && free < size;data=prev,prev=prev->prev)
      if (!data->isLock)
      {
        free            += data->cd->size;

        if (data->cd->owner) 
          data->cd->owner->UnloadModelData (data->data,data->cd->size,data->cd->param);
       
        data->prev->next = data->next;
        data->next->prev = data->prev;        
        data->cd->node   = NULL;        
        data->isLock     = FALSE;

        mDefragPool.free (data);
      }                             
  }  

  cachenode_t* data = (cachenode_t*)mDefragPool.alloc (size,MoveData,(DWORD)this);

  if (!data)
  {
    dout<<"ModelDataCache: Defrag"<<endl;
    mDefragPool.defrag ();  
    data = (cachenode_t*)mDefragPool.alloc (size,MoveData,(DWORD)this);

    if (!data)
      return NULL;
  }

  return data;
}

ModelDataCache::ModelDataCache  (Pool* _Pool,size_t _CacheSize,int _ModelDataNum)
               : MemObject      (_Pool),
                 mDefragPool    (_Pool,_CacheSize),
                 mModelDataNum  (_ModelDataNum),
                 mFirstFree     (-1),
                 mCacheData     (NULL)
{
  D_Try
  {
    if (!IsValid (&mDefragPool))
    {
      #ifdef DEBUG
        dout<<"ModeDataCache: No enough memory for defrag pool "<<endl;
      #endif
      mModelDataNum = 0;
      return;
    }

    mCacheData = (cachedata_t*)GetPool()->alloc (sizeof (cachedata_t) * mModelDataNum);

    if (!mCacheData)
    {
      #ifdef DEBUG
        dout<<"ModelDataCache: No enough memory for sys data "<<endl;
      #endif
      mModelDataNum = 0;
      return ;
    }

    for (int i=0;i<mModelDataNum;i++)
    {
      cachedata_t& data = mCacheData [i];

      data.use          = FALSE;
      data.node         = NULL;
      data.owner        = NULL;
      data.next         = i + 1;
      data.size         = 0;
    }

    mCacheData [i-1].next = -1;
    mFirstFree            = 0;

    mFirst.next    = &mLast;
    mLast.prev     = &mFirst;
    mFirst.isLock  = mLast.isLock = FALSE;
    mFirst.cd      = mLast.cd     = NULL;
    mFirst.prev    = mLast.next = NULL;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelDataCache: Error at construct "<<endl;
    #endif
  }
}

ModelDataCache::~ModelDataCache ()
{
  D_Try
  {
   if (mCacheData) GetPool()->free (mCacheData);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelDataCache: Error at destruct "<<endl;
    #endif
  }
}
