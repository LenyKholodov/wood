#include <pch.h>
#include <memory\hpool.h>
#include <string.h>
#include <misc.h>
#include <debug.h>

size_t  HeapPool::size     () const
{
  return owner ()->size ();
}

size_t  HeapPool::memavail () const
{
  return owner ()->memavail () + mFree + mStartPool.memavail ();
}

size_t  HeapPool::maxavail () const
{
  size_t  fm   = owner ()->maxavail ();
  lump_t* lump = mLumpTree.GetMax ();
  size_t _max  = 0;

  if (lump)
   _max = lump->size;

  return fm > _max ? fm : _max;
}

void    HeapPool::reset   ()
{
  D_Try
  {
    clear (LTree::iterator (mLumpTree));
    mStartPool.reset ();
    mTempPool.reset ();
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"HeapPool: Error at reset "<<this<<endl;
    #endif
  }
}

bool    HeapPool::belong  (void* x) const
{
  return owner ()->belong (x);
}

void*   HeapPool::find    (LTree::iterator node,size_t size)
{
  D_Try
  {
    LTree::iterator _min = node;
    size_t          smin = mMaxFree;

    for (;node.valid();node++)
    {    
      if (size <= node->size && node->size - size <= smin)
      {
        smin = node->size - size;
        _min = node;
      }
    }

    if (_min.valid () && _min->size >= size)
    {
      void* buf  = _min;
      mFree     -= _min->size;

      mLumpTree.erase (_min);

      return buf;
    }
    else        
      return NULL;                 
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {           
    #ifdef DEBUG
      dout<<"HeapPool: Error at alloc (iter,size_t) "<<this<<endl;
    #endif
    return NULL;
  }
}

void*   HeapPool::_alloc   (size_t size)
{
  D_Try
  {
    size        += sizeof (lump_t);
    lump_t* buf  = (lump_t*)mStartPool.alloc (size);

    if (buf)    
    {
      buf->size = size;
      return buf->data;
    }  

    buf = (lump_t*)find (mLumpTree.GetFirst (),size);

    if (buf)
    {
      buf->size = size;
      return buf->data;
    }  

    buf = (lump_t*)owner()->alloc (size);

    if (buf)
    {
      buf->size = size;
      return buf->data;
    }

    return NULL;
  }     
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"HeapPool: Error at _alloc () size: "<<size<<endl;
    #endif
    return NULL;
  }
}

void    HeapPool::_free    (void* ptr)
{
  D_Try
  {
    lump_t* buf = (lump_t*)((char*)ptr - sizeof (lump_t));

    if (mStartPool.belong (buf))
    {
      mStartPool.free (buf);
      return;
    }

    if (!mLumpTree.insert (buf->size,buf))
      owner()->free (buf);
    else
    {
      if (buf->size > mMaxFree) mMaxFree = buf->size;
      mFree += buf->size;
    }        
  }     
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {           
    #ifdef DEBUG
      dout<<"HeapPool: Error at _free () ptr: "<<ptr<<endl;
    #endif
  }
}

void    HeapPool::clear    ()
{
  clear (LTree::iterator (mLumpTree));
  mLumpTree.clear ();
}

void    HeapPool::clear    (LTree::iterator node)
{
  D_Try
  {     
    for (LTree::iter iter = mLumpTree;iter.valid ();iter++)
      owner ()->free (iter);
  } 
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {           
    #ifdef DEBUG
      dout<<"HeapPool: Error at clear() "<<this<<endl;
    #endif
  }        
}

HeapPool::HeapPool (Pool* _Pool,size_t _StartSize)
         : Pool (_Pool),
           mStartPool (_Pool,_StartSize),
           mTempPool  (_Pool,DEFAULT_HEAP_TEMP_SIZE),
           mFree      (0),
           mMaxFree   (0),
           mLumpTree  (&mTempPool)           
{
}

HeapPool::~HeapPool ()
{
  clear ();
}
