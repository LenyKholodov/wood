#include <pch.h>
#include <memory/flat.h>
#include <string.h>
#include <debug.h>

bool    FlatPool::belong  ( void* x) const
{
  return  mPoolSpace<=x && (char*)mPoolSpace+mSize>x;
}

void    FlatPool::reset   ( void  )
{
  if (getbuf ())  _reset ( getbuf () );
  mFree = mSize;
}

void  FlatPool::setbuf   (void* x,size_t size)
{
  mPoolSpace = x;

  if (size)
  {
    mSize = mFree = size;    
  }
}
          
FlatPool::FlatPool  (Pool* pool)
      : Pool (pool), mPoolSpace (NULL)
{
  for (int total = pool->maxavail();!(mPoolSpace=pool->alloc(total)) && total>2;total/=2);   
  
  if (mPoolSpace)  mFree = mSize = total;
  else             mFree = mSize = 0;
}

FlatPool::FlatPool  (Pool* pool,size_t _size)
      : Pool(pool), mPoolSpace (NULL)
{
  mPoolSpace = pool->alloc (_size);

  if (mPoolSpace)  mFree = mSize = _size;
  else             mFree = mSize = 0;
}

FlatPool::FlatPool  ()
         : Pool (NULL), mPoolSpace (NULL) 
{ }

FlatPool::~FlatPool ()
{
  if (mPoolSpace && owner ())
    owner()->free (mPoolSpace);
}
