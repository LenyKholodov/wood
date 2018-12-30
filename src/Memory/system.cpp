#include <pch.h>
#include <memory/system.h>
#include <debug.h>

struct   sysnode_t
{
  sysnode_t*    next;
  sysnode_t*    prev;        
  size_t        size;
  char          data [];
};

bool       SystemPool::belong  ( void* block) const
{
  MEMORY_BASIC_INFORMATION      info;

  VirtualQuery ((char*)block-sizeof (sysnode_t),&info,sizeof (info));

  return info.State == MEM_COMMIT;
}

void       SystemPool::reset   ()
{
  D_Try
  {
    if (mFirst)
    {
      sysnode_t* next = mFirst->next;
      for (sysnode_t* node = mFirst;node;node = next, next = next ? next->next : NULL)
          if (node)
            _free (node->data);
    }    
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Pool: Error at reset pool "<<this<<endl;
    #endif 
  }
}

size_t     SystemPool::size    () const
{
  MEMORYSTATUS  status;

  status.dwLength = sizeof (status);
  
  GlobalMemoryStatus (&status);
  
  return  status.dwTotalVirtual;
}

size_t     SystemPool::memavail    () const
{
  MEMORYSTATUS  status;

  status.dwLength = sizeof (status);
  
  GlobalMemoryStatus (&status);
  
//  return  status.dwAvailVirtual;
  return  status.dwAvailPhys;
}

size_t     SystemPool::maxavail () const
{
  return memavail ();
}

void*      SystemPool::alloc   (size_t bytes,int flags)
{
  D_Try
  {
    void* buf = Pool::alloc (bytes);

    if (flags == MEM_LOCK)
    {
      if (!VirtualLock (buf,bytes)) 
      {
        free (buf);
        return NULL;
      }
      else
        return buf;  
    }  
    return buf;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"SystemPool: Error at alloc (size_t,int) "<<this<<endl;
    #endif
    return NULL;
  }
}

void*      SystemPool::_alloc ( size_t bytes)
{
  D_Try
  {
//    sysnode_t* lump = (sysnode_t*)VirtualAlloc (VirtualAlloc (
//                      NULL,bytes+sizeof (sysnode_t),MEM_RESERVE,PAGE_READWRITE),
//                      bytes+sizeof (sysnode_t),MEM_COMMIT,PAGE_READWRITE);
    sysnode_t* lump = (sysnode_t*)VirtualAlloc (NULL,bytes+sizeof (sysnode_t),MEM_COMMIT,PAGE_READWRITE); 
   
    lump->size = bytes+sizeof (sysnode_t);

    if (!mFirst || !mLast)
    {
       mLast = mFirst = lump;
     
       mLast->next  = NULL; mLast->prev  = NULL;
       mFirst->next = NULL; mFirst->prev = NULL;
    }   
    else
    {            
      mLast->next  = lump;     
      lump->next   = NULL;
      lump->prev   = mLast;
      mLast        = lump;
    }  
  
    return lump->data;
  }   
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {           
    #ifdef DEBUG
      dout<<"System pool: Error alloc block size = "<<bytes<<endl;
    #endif
  }
  return NULL;
}

void       SystemPool::_free  ( void* block)
{  
  D_Try        
  {
    sysnode_t* lump= (sysnode_t*)((char*)block-sizeof(sysnode_t));
  
    if (lump->next) lump->next->prev = lump->prev;
    if (lump->prev) lump->prev->next = lump->next;
  
    if (lump == mLast)   mLast  = lump->prev;
    if (lump == mFirst)  mFirst = lump->next;

    VirtualFree (lump,lump->size,MEM_DECOMMIT|MEM_RELEASE);
  }  
  D_Exc ("System pool: Error at block delete "<<block<<" from pool "<<this);
}
 
SystemPool::SystemPool ()
           : Pool (NULL), 
             mFirst (NULL), 
             mLast (NULL)
{  }

SystemPool::~SystemPool()
{
  reset (); 
}
