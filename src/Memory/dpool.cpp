#include <pch.h>
#include <memory\dpool.h>
#include <debug.h>
#include <string.h>
#include <misc.h>

#pragma pack(push,1)

struct   defraginfo_t
{
  DWORD         param;
  void*         change;
  char          data [];
};

#pragma pack(pop)

size_t   DefragPool::maxavail () const
{
  if (mFirstFree)
  {
    int max = 0;
        
    for (freedesc_t* node = mFirstFree;node;node = node->nextFree)
      if (node->size > max)
        max = node->size;
        
    return max - sizeof(allocdesc_t) - sizeof (defraginfo_t);
  }
  else  
    return 0;
}

size_t  DefragPool::memavail () const
{
  return mFree - sizeof (allocdesc_t) - sizeof (defraginfo_t);
}

bool    DefragPool::belong  (void* block) const
{
  D_Try
  {
    if (!FlatPool::belong (block))
       return false;

    freedesc_t* desc = (freedesc_t*)((char*)block-sizeof(allocdesc_t)-sizeof (defraginfo_t));
    
    return desc->id != FREE_FLAG;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
  }
  return false;
}

void*   DefragPool::_alloc   (size_t bytes)
{
  defraginfo_t* info = (defraginfo_t*)FloatPool::_alloc (bytes + sizeof (defraginfo_t));  

  if (!info)
     return NULL;
  
  info->change       = mChange;
  info->param        = mParam;
  mChange            = (void*)-1;
  mParam             = 0;
  
  return info->data;
}

void    DefragPool::_free  (void* x)
{
  defraginfo_t* info = (defraginfo_t*)((char*)x - sizeof (defraginfo_t));  

  if (info->change != (void*)-1)
  {
    D_Try
    {
      MoveObjectFn change    = (MoveObjectFn)info->change; 
      (*change)(info->data,NULL,info->param);
    }
    D_Exception (EXCEPTION_EXECUTE_HANDLER)
    {
    }  
  }
  else if (info->param)
  {
    void** addr = (void**)info->param;
    *addr       = NULL;
  } 
    
  FloatPool::_free (info);
}

void*   DefragPool::alloc   (size_t bytes,MoveObjectFn fn,DWORD param)
{
  if (fn)
  {
    mChange = fn;
    mParam  = param;
  }
  else
  {
    mChange = (void*)-1;
    mParam  = 0;
  }

  return _alloc (bytes);
}

void*   DefragPool::alloc   (size_t bytes,void* change)
{
  mChange = (void*)-1;
  mParam  = (DWORD)change;

  return _alloc (bytes);
}

bool       DefragPool::_move    (freedesc_t* left,freedesc_t* right)
{
         //Перемещение правого объекта к левому                        
  //left - free; right - data

  defraginfo_t* info = (defraginfo_t*)((char*)right+sizeof (allocdesc_t));
        
  if (!right || (info->change == (void*)-1 && !info->param))
    return false;           
        
  freedesc_t   temp      = *left;

  if (info->change != (void*)-1)
  {
    D_Try
    {
      MoveObjectFn change    = (MoveObjectFn)info->change; 

      if (!(*change)((char*)right+sizeof (allocdesc_t) + sizeof (defraginfo_t),
                     (char*)left+sizeof (allocdesc_t) + sizeof (defraginfo_t),
                     info->param)) 
                       return false;
    }
    D_Exception (EXCEPTION_EXECUTE_HANDLER)
    {
      return false;
    }  
  }
  else if (info->param)
  {
    void** addr = (void**)info->param;
    *addr       = (char*)left + sizeof (allocdesc_t) + sizeof (defraginfo_t);
  }  
  
  memmove (left,right,right->size);

  //left - data; right - free

  right             = (freedesc_t*)((char*)left+left->size);

  left->prev        = temp.prev;
  *right            = temp;
  right->prev       = left;
  
  if (right->prevFree) right->prevFree->nextFree  = right;  
  else                 mFirstFree                 = right; 
  if (right->nextFree) right->nextFree->prevFree  = right;

  for (;this->next (right) && right->nextFree == this->next (right);right = _concat (right,this->next (right)));
    
  return true;  
}

void       DefragPool::defrag  ()
{
  D_Try
  { 
    if (mFirstFree)   
    {
      for (freedesc_t* node = mFirstFree;node;)
          if (_move (node,next (node))) node = mFirstFree;
          else                          node = node->nextFree;
    }  
  }
  D_Exc ("DefragPool: Error defrag pool");
}

bool       DefragPool::defrag  (size_t size)
{
  D_Try
  { 
    size_t free  = 0; 
    size        += sizeof (allocdesc_t) + sizeof (defraginfo_t);

    if (mFirstFree && size < mFree)
    {
      for (freedesc_t* node = mFirstFree;node && free < size;)
          if (_move (node,next (node))) 
          {
            node  = next (node);
            free  = node->size;
          }
          else                          
          {
            node = node->nextFree;
            free = node ? node->size : 0; 
          } 

      return free >= size;
    }  

    return false;            
  }
  D_ExcRet ("DefragPool: Error defrag pool (size_t)",false);
}

DefragPool::DefragPool  (Pool* pool)
          : FloatPool   (pool), mChange ((void*)-1), mParam (0)
{    }

DefragPool::DefragPool  (Pool* pool,size_t _size)
          : FloatPool   (pool,_size), mChange ((void*)-1), mParam (0)
{    }

DefragPool::DefragPool  (): mChange ((void*)-1), mParam (0) {}

DefragPool::~DefragPool ()
{     }