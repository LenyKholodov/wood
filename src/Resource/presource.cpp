#include <pch.h>
#include <resource\presource.h>

class piterator: public ResourceGroup::iterator
{
  public:
              piterator (const ResourceGroup::iterator& iter):iterator (iter) {}

              piterator& operator =  (const ResourceGroup::iterator& iter) { return (piterator&)iterator::operator = (iter);}

     PriorResource* operator -> () const { return (PriorResource*)mCurrent; }
};

PriorResource::PriorResource (PriorResourceGroup* group)
              : Resource (group), mCount (0), mPrior (0) 
{    }

void     PriorResourceGroup::process  ()
{
  uint   maxCount = 0,
         minCount = (uint)-1l;
  size_t size     = 0;

  for (piterator res = GetLast ();res.more ();res--)
  {
    if (res->mCount > maxCount)  maxCount = res->mCount;
    if (res->mCount < minCount)  minCount = res->mCount;

    size        += res->size ();
  }

  float k = (maxCount - minCount ? 1.0f / float (maxCount - minCount) : 0) * (mMaxPrior - mMinPrior);  

  for (res = (piterator)GetLast ();res.more () && size > mMaxSize;res--)
  {
    if (res->IsLoad () && !res->IsLock ())
    {              
      size -= res->size ();
      res->unload       ();

      res->mCount = 0;
      res->mPrior = mMinPrior;
    }  
    else
    {
      res->mPrior = float (res->mCount-minCount) * k + mMinPrior;
      res->mCount = 0;
    }
  }  

  for (;res.more ();res--)
  {
    res->mPrior = float (res->mCount-minCount) * k + mMinPrior;
    res->mCount = 0;
  }
}

PriorResourceGroup::PriorResourceGroup  (size_t size)
                    : mMaxSize   (size),
                      mMinPrior  (0), mMaxPrior (1)                      
{   }
