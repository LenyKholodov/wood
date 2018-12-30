#include <pch.h>
#include <memory\float.h>
#include <debug.h>

void*         FloatPool::_alloc   (size_t bytes)
{
  bytes += sizeof (allocdesc_t);
  bytes  = bytes < sizeof (freedesc_t) ? sizeof (freedesc_t) : bytes;

  for (freedesc_t* node = mFirstFree;node;node = node->nextFree)//node = next (node))
     if (node->size >= bytes)
     {
        freedesc_t* block;
        
        if (node->size-bytes >= sizeof(freedesc_t))
        {        
          block            = (freedesc_t*)((char*)node+bytes);
          block->size      = node->size - bytes;
          block->prev      = (allocdesc_t*)node;
          block->id        = FREE_FLAG;
          
          if (next (node)) next (node)->prev = block;          
                    
          node->size       = bytes;
          
          block->nextFree  = node->nextFree;
          block->prevFree  = node->prevFree;
          
          if (block->nextFree)  block->nextFree->prevFree = block;      
          if (block->prevFree)  block->prevFree->nextFree = block;
          else                  mFirstFree                = block;
        }  
        else 
        {
          if (node->nextFree) node->nextFree->prevFree  = node->prevFree; 
          if (node->prevFree) node->prevFree->nextFree  = node->nextFree;
          else                mFirstFree                = node->nextFree;

          bytes   = node->size;
        }

        mFree        -= bytes;
        node->id      = 0;
     
        return (char*)node + sizeof (allocdesc_t);
     }
     
  return NULL;    
}

void          FloatPool::_free    (void* x)
{
  freedesc_t*  node  = (freedesc_t*)((char*)x-sizeof(allocdesc_t)); //OK!!!
  
                //Освобождаем блок

  node->id =  FREE_FLAG;
  mFree    += node->size;
                                
  if (mFirstFree && mFirstFree < node)
  {              
    for (freedesc_t* _free = mFirstFree;_free && _free->nextFree;_free=_free->nextFree)
       if (_free->nextFree >= node)
          break;
    
    node->prevFree = _free;
    node->nextFree = _free->nextFree;

    if (_free->nextFree)
      _free->nextFree->prevFree = node;
      
    _free->nextFree             = node;    
  }
  else if (mFirstFree && mFirstFree > node)
  {  
    node->prevFree               = NULL;
    node->nextFree               = mFirstFree;
    mFirstFree->prevFree         = node;
    mFirstFree                   = node;   
  }
  else
  {
    mFirstFree = node;
    
    node->nextFree = NULL;
    node->prevFree = NULL;
    
    return ;
  }     
  
  if (node->prev == node->prevFree && node->prev)
     node = _concat ((freedesc_t*)node->prev,node);
     
  if (next (node) == node->nextFree && next (node))
     node = _concat (node,next (node));
}

freedesc_t*   FloatPool::_concat  (freedesc_t* left,freedesc_t* right)
{
    //Присоединение правого к левому      
    
  if (next (left) != right)
     return NULL;       
       
  left->size     += right->size;
  left->nextFree  = right->nextFree;  

  if (left->nextFree) left->nextFree->prevFree = left;
  if (next (left))    next (left)->prev        = left;
  
  return left;  
}

size_t   FloatPool::maxavail () const
{
  if (mFirstFree)
  {
    int max = 0;
        
    for (freedesc_t* node = mFirstFree;node;node = node->nextFree)
      if (node->size > max)
        max = node->size;
        
    return max - sizeof(allocdesc_t);
  }
  else  
    return 0;
}

size_t  FloatPool::memavail () const
{
  return mFree - sizeof (allocdesc_t);
}

bool    FloatPool::belong  (void* block) const
{
  D_Try
  {
    if (!FlatPool::belong (block))
       return false;

    freedesc_t* desc = (freedesc_t*)((char*)block-sizeof(allocdesc_t));
    
    return desc->id != FREE_FLAG;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
  }
  return FALSE;
}

void   FloatPool::_reset   (void* lump)
{
  for (freedesc_t* node = mFirstFree;node;node = next (node))
    node->prev = NULL;

  mFree                 = size ();
  mFirstFree            = (freedesc_t*)lump;
  mFirstFree->size      = mFree;
  mFirstFree->prev      = NULL;
  mFirstFree->nextFree  = NULL;
  mFirstFree->prevFree  = NULL;
  mFirstFree->id        = FREE_FLAG;
}

void   FloatPool::MovePtr  (long offs)
{
  for (freedesc_t* node = mFirstFree;node;node = next (node))
  {
    node->prev  = (allocdesc_t*)((char*)node->prev + offs);

    if (node->id == FREE_FLAG)
    {
      node->prevFree = (freedesc_t*)((char*)node->prevFree + offs);
      node->nextFree = (freedesc_t*)((char*)node->nextFree + offs);
    }
  }  

  setbuf ((char*)getbuf () + offs);
  mFirstFree  = (freedesc_t*)((char*)mFirstFree + offs);
}

FloatPool::FloatPool  (Pool* pool)
          : FlatPool   (pool), 
            mFirstFree (NULL)
{
  if (getbuf()) 
    _reset (getbuf());
}

FloatPool::FloatPool  (Pool* pool,size_t _size)
          : FlatPool   (pool,_size), 
            mFirstFree (NULL)
{
  if (getbuf())    _reset (getbuf());
//  else             D_Error (EXCEPTION_NO_MEMORY);  throw
}

FloatPool::FloatPool  (): mFirstFree (NULL) {}

FloatPool::~FloatPool ()
{     }