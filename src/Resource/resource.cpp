#include <pch.h>
#include <resource\resource.h>
#include <debug.h>

///////////////////////////implementation of class Resource/////////////////////

Pool* Resource::mResourcePool = NULL;

uint      Resource::lock    ()
{
  if (IsWrong ())
    return mLock = 0;

  if (!IsLoad ())
  {
    mLock = 1;
    
    if (!load ())
    {
      enable (RES_WRONG);
      return mLock = 0;    
    } 
    else enable (RES_LOAD); 
  }
  else
    mLock++;
  
  if (mOwner)
    mOwner->lock (*this);

  return mLock;
}

void      Resource::unlock  ()
{
  if (mLock)
  {
    mLock--;  

    if (mOwner)
      mOwner->unlock (*this);
  }  
}

void      Resource::unload     ()
{
  disable (RES_LOAD);
  disable (RES_DIRTY);

  mLock  = 0;
}

void      Resource::SetOwner   (ResourceGroup* own)
{
  unload ();

  if (mOwner)
    mOwner->erase (*this);

  mOwner = own;

  if (own) own->insert (*this);
}

void*     Resource::operator new    (size_t size,Pool* pool)
{
  return ::operator new (size,pool);
}

void      Resource::operator delete (void* x)
{
  ::operator delete (x);
}

Resource::Resource  (ResourceGroup* own)
{
  mLock  = 0;
  mFlags = 0;
  mNext  = mPrev = NULL;
  mOwner = NULL;
  mOwnerData = NULL;

  SetOwner (own);  
}

Resource::~Resource  () 
{ 
  if (mOwner) mOwner->erase (*this); 
}

///////////////////////////////ResourceGroup/////////////////////////////////////////////////

bool     ResourceGroup::erase (Resource& r)
{
  if (r.mOwner == this)
  {
    if (r.IsLoad ())
      r.unload ();

    r.mNext->mPrev = r.mNext;
    r.mPrev->mNext = r.mPrev;
    r.mNext        = NULL;
    r.mPrev        = NULL;
    r.mOwner       = NULL;

    return true;
  }
  else return false;
}

bool     ResourceGroup::first (Resource& r)
{
  if (r.mOwner == this)
  {
    r.mNext  = mFirst.mNext;
    r.mPrev  = &mFirst;

    mFirst.mNext->mPrev = &r;
    mFirst.mNext        = &r;  

    return true;
  }  
  else return false;
}

bool     ResourceGroup::last           (Resource& r)
{
  if (r.mOwner == this)
  {
    r.mPrev  = mLast.mPrev;
    r.mNext  = &mLast;

    mLast.mPrev->mNext = &r;
    mLast.mPrev        = &r;  

    return true;
  }
  else return false;
}

void     ResourceGroup::UnlinkAll  ()
{
  iterator iter = *this,
           next = iter++; iter--;

  for (;iter.more ();iter=next,next++)
    erase (*iter);
}

ResourceGroup::ResourceGroup  ()
{
  mFirst.mNext = &mLast;
  mLast.mPrev  = &mFirst;
  mFirst.mPrev = mLast.mNext = NULL;
}

ResourceGroup::~ResourceGroup ()
{
  UnlinkAll ();
}
