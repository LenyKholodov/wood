#define  DEBUG
#include <memory\memory.h>

const ulong  FREE_FLAG = 0xF1BA0A21;

struct allocdesc_t
{
  size_t        size;
  allocdesc_t*  prev;  
};

struct freedesc_t: public allocdesc_t
{
  long          id;
  freedesc_t*   prevFree;
  freedesc_t*   nextFree;
};

class  XPool: public FlatPool
{
  public:
                                XPool  (Pool*);
                                XPool  (Pool*,size_t);                                
          virtual               ~XPool ();

          virtual       bool    belong  ( void* ) const;
          
          virtual       size_t  maxavail () const;
          virtual       size_t  memavail () const { return mFree -  sizeof (allocdesc_t); }
          
//          virtual       void    dump    (ostream&) const;

  protected:
          virtual       void*         _alloc   ( size_t );
          virtual       void          _free    ( void*  );          

          inline        freedesc_t*   next     ( const freedesc_t* node) const;

  private:          
                        freedesc_t*   _concat  ( freedesc_t* left,freedesc_t* right);
          virtual       void          _reset   ( void*  );
          
  private:
//          allocdesc_t*  mFirst;
          freedesc_t*   mFirstFree;
};

inline freedesc_t*  XPool::next (const freedesc_t* node) const
{
  if ((char*)node - (char*)getbuf() + node->size < size ())  
     return (freedesc_t*)((char*)node+node->size);
  else            
     return NULL;
}

void*         XPool::_alloc   (size_t bytes)
{
  bytes += sizeof (allocdesc_t);
  bytes  = bytes < sizeof (freedesc_t) ? sizeof (freedesc_t) : bytes;

  for (freedesc_t* node = mFirstFree;node;node = next (node))
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
                    
          node->size      = bytes;
          
          block->nextFree = node->nextFree;
          block->prevFree = node->prevFree;
          
          if (block->nextFree)  block->nextFree->prevFree = block;      
        }  
        else if (node->nextFree)
        {       
          block           = node->nextFree;          
          block->prevFree = node->prevFree;         
        }                 
        else   
          return NULL;
        
        if (block->prevFree) block->prevFree->nextFree = block;
        else                 mFirstFree                = block;        
                
        mFree        -= bytes;
     
        return (char*)node + sizeof (allocdesc_t);
     }
     
  return NULL;    
}

void          XPool::_free    (void* x)
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

freedesc_t*   XPool::_concat  (freedesc_t* left,freedesc_t* right)
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

size_t   XPool::maxavail () const
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

bool    XPool::belong  (void* block) const
{
  D_Try
  {
    if (!FlatPool::belong (block))
       return false;

    allocdesc_t* desc = (allocdesc_t*)((char*)block-sizeof(allocdesc_t));
    
    return next ((freedesc_t*)desc->prev) == (freedesc_t*)desc;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
  }
  return FALSE;
}

void   XPool::_reset   (void* lump)
{
  mFree                 = size ();
  mFirstFree            = (freedesc_t*)lump;
  mFirstFree->size      = mFree;
  mFirstFree->prev      = NULL;
  mFirstFree->nextFree  = NULL;
  mFirstFree->prevFree  = NULL;
}

XPool::XPool  (Pool* pool)
          : FlatPool   (pool), 
            mFirstFree (NULL)
{
  if (getbuf()) 
    _reset (getbuf());
}

XPool::XPool  (Pool* pool,size_t _size)
          : FlatPool   (pool,_size), 
            mFirstFree (NULL)
{
  if (getbuf())    _reset (getbuf());
  else             D_Error (EXCEPTION_NO_MEMORY);
}

XPool::~XPool ()
{     }

void  main ()
{
  SystemPool sys;
  XPool      pool (&sys,5000000);          

  void*   x  = pool.alloc (1000000);
  void*   x1 = pool.alloc (1000000);
  void*   x2 = pool.alloc (1000000);

  cout<<x1<<endl;

  pool.free (x1);

  x1 = pool.alloc (1000000);

  cout<<x1<<endl;
}

