#include <pch.h>
#include <memory\xpool.h>

void    SharedPool::reset   (void)
{
  CSEntry trans (mTrans);
  owner ()->reset ();
}

bool    SharedPool::belong  (void* x) const
{
  CSEntry trans ((CritSect&)mTrans);
  return owner ()->belong (x);
}

size_t  SharedPool::size     () const
{
  CSEntry trans ((CritSect&)mTrans);
  return owner ()->size ();
}

size_t  SharedPool::memavail () const
{
  CSEntry trans ((CritSect&)mTrans);
  return owner ()->memavail ();
}

size_t  SharedPool::maxavail () const
{
  CSEntry trans ((CritSect&)mTrans);
  return owner ()->maxavail ();
}

void    SharedPool::dump    (ostream& os) const
{
  CSEntry trans ((CritSect&)mTrans);
  owner ()->dump (os);
}

void*   SharedPool::_alloc   (size_t size)
{
  CSEntry trans (mTrans);
  return owner ()->_alloc (size);
}

void    SharedPool::_free    (void* x)
{
  CSEntry trans (mTrans);
  owner ()->_free (x);
}

bool    SharedPool::valid () const
{
  CSEntry trans ((CritSect&)mTrans);
  return owner ()->valid ();
}

SharedPool::SharedPool  (Pool* pool,bool del)
           : Pool     (pool),
             mAutoDel (del)
{   }

SharedPool::~SharedPool ()
{
  CSEntry entry (mTrans);
  if (owner () && mAutoDel)
    delete owner ();
}
