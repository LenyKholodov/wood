#include "pch.h"
#include "r_mdlpool.h"

void*     ModelPool::alloc  (model_t* mdl,size_t size) 
{ 
  void* x = mDefragPool.alloc (size,&mdl->data.data);

  if (!x)
  {
    mDefragPool.defrag ();
    x = mDefragPool.alloc (size,&mdl->data.data);
  }

  if (!x) 
  {
    mdl->data.data = NULL;
    mdl->data.size = 0;
    return NULL;
  }

  mdl->data.data = x;
  mdl->data.size = size;
    
  return x; 
}

void      ModelPool::free   (model_t* mdl) 
{ 
  if (mdl->data.data)
    mDefragPool.free (mdl->data.data); 

  mdl->data.data = NULL;
  mdl->data.size = 0;  
}

ModelPool::ModelPool  (Pool* _Pool,size_t size)
          : MemObject (_Pool),
            mDefragPool (_Pool,size)            
{
  if (!IsValid (&mDefragPool))
  {
    #ifdef DEBUG
      dout<<"ModelPool: No enough memory for defrag pool "<<endl;
    #endif
    return;    
  }  
}

ModelPool::~ModelPool ()
{
}
