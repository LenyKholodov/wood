#include <pch.h>
#include <memory\clpool.h>
#include <debug.h>

struct  claster_t
{
  claster_t*    next;        
  char          data [];
};

size_t     ClasterPool::clsize  () const
{
  return mClaster - sizeof (claster_t);
}

int        ClasterPool::clcount () const
{
  return mClasterNum;
}     

bool       ClasterPool::belong   (void* x) const
{
  D_Try
  {
    if (!FlatPool::belong (x))
       return false;

    claster_t* mb = (claster_t*)((char*)x-sizeof(claster_t));    
   
    return mb->next == NULL;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
  }  
  return false;
}

void*      ClasterPool::_alloc  ( size_t bytes)
{
  if (mFirst)
  {
    mFree -= mClaster;

    claster_t* _free = mFirst;      

    mFirst           = mFirst->next;

    return _free;   
  }
  else return NULL; 
}

void       ClasterPool::_free   ( void*  x) 
{
  claster_t* mb = (claster_t*)((char*)x-sizeof(claster_t));

  mb->next  =  mFirst;
  mFirst    =  mb; 
  mFree    += mClaster;  
}

size_t     ClasterPool::memavail    () const
{
  return mFree - sizeof (claster_t);
}

size_t     ClasterPool::maxavail()  const
{
  return mClaster - sizeof (claster_t);
}

void       ClasterPool::_reset(void* lump)
{
  char* data = (char*)lump;
  long  ps   = size()/mClaster;
  
  for (long i=0;i<ps;i++,data+=mClaster)
  {
    claster_t*  mb = (claster_t*)data;
    mb->next       = (claster_t*)(data+mClaster);
  }

  ((claster_t*)(data-mClaster))->next = NULL; 

  mFirst      = (claster_t*)lump;
  mClasterNum = ps;
  mFree       = size ();
}
           
ClasterPool::ClasterPool (Pool* pool,size_t cl_size)
            : FlatPool (pool), mClaster (cl_size+sizeof(claster_t)),
              mClasterNum (0)
{    
  if (getbuf()) _reset (getbuf());
}

ClasterPool::ClasterPool ( Pool* pool,size_t total_size,size_t cl_size)
            : FlatPool (pool,total_size), 
              mClaster (cl_size+sizeof(claster_t)),
              mClasterNum (0)
{
  if (getbuf())  _reset  (getbuf());
//  else           D_Error (EXCEPTION_NO_MEMORY);
}
