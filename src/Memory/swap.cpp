#include <pch.h>
#include <memory\swap.h>
#include <debug\defs.h>
#include <binfile.h>

struct  swapnode_t
{
  long   next, num;
  long   uid;
  size_t size;
  char   data [];
};              

struct  swalloc_t
{
  long   pos;  
  long   num;
  long   next;
};

long          SwapFile::write     (const void* ptr,size_t size)
{
  D_Try
  {
    if (mSize && size > mFree)
    {
      D_Message ("SwapFile: No enough swap-file space");
      return -1; 
    }

    if (size == 0)
    {
      D_Message ("SwapFile: Write size = 0");
      return -1;
    }

    const char*   data = (const char*)ptr;
    long          pos  = 0;
    long          ws   = 0;
    long          fst  = 0;
    swapnode_t    node;
    swalloc_t     lump;

//    dout<<"begin!"<<endl;

    for (;size;pos+=ws,size-=ws)
    {
      ws = alloc (&lump,size);

      if (lump.pos == -1)
      {
        D_Message ("SwapFile: File is full");
        return -1;
      }    

//      dout<<"..."<<lump.pos<<endl;

      if (!fst) 
      {
        fst       = lump.pos;
        node.uid  = fst;
      }

      node.next    = lump.next;
      node.num     = lump.num;
      node.size    = ws;
      node.uid     = fst;

      mFile->seek  (lump.pos);
      mFile->write (&node,sizeof (node));    

      mFile->write (data+pos,ws);
    }

//    dout<<"end!"<<endl;

    return fst;
  }
  D_ExcRet ("SwapFile: Error at write",-1);
}

bool          SwapFile::read      (void* ptr,long fst)
{
  D_Try
  {
    if (fst == -1)
    {
      D_Message ("SwapFile: Wrong UID");
      return false;
    }

    char*         data = (char*)ptr;
    size_t        size = 0;
    long          pos  = 0;
    long          uid  = fst;
    swapnode_t    node;

    do
    {
      mFile->seek (fst);
      mFile->read (&node,sizeof (node));
      mFile->read (data+pos,node.size);

      pos         += node.size;
      mFree       += node.num << mShift;
      fst          = node.next;

    } while (fst != -1);

    free (uid);    

    return true;
  }
  D_ExcRet ("SwapFile: Error at read",false);
}
          
size_t        SwapFile::alloc     (swalloc_t* lump,size_t size)
{
  size_t rsize = 0;

  if (mFirstPos != -1)
  {
    swapnode_t node;

    mFile->seek (mFirstPos);
    mFile->read (&node,sizeof (node));

    if (node.size <= size)
    {
      lump->pos  = mFirstPos;
      lump->num  = node.num;
      lump->next = node.next;
      size      -= node.size;      
      rsize      = node.size;
      mFirstPos  = node.next;
      mFree     -= node.size;
    }
    else
    {
      int num = (size + sizeof (swapnode_t)) >> mShift;
          num = (num << mShift) - (size + sizeof (swapnode_t)) < 0 ? num+1 : num;

      lump->pos      = mFirstPos;
      lump->num      = num;
      lump->next     = node.next + (num << mShift);
      mFirstPos     += num << mShift;
      mFree         -= num << mShift;
      node.num      -= num;
      node.size     -= num << mShift;
      rsize          = size;
      
      mFile->seek  (mFirstPos);
      mFile->write (&node,sizeof (node));      
    }
  }
  else if ((mSize && size + sizeof (swapnode_t) <= mFree) || !mSize)
  {
    int num = (size + sizeof (swapnode_t)) >> mShift;
        num = (num << mShift) - (size + sizeof (swapnode_t)) < 0 ? num+1 : num;

    lump->pos  = mLastPos;
    lump->num  = num;
    lump->next = -1;
    mLastPos  += num << mShift;
    rsize      = size;
  }
  else
    lump->pos  = -1;

  return rsize;
}

void          SwapFile::free      (long fst)
{
  swapnode_t node;
  long       lst = mFirstPos;
  mFirstPos      = fst;

  while (1)
  {
    mFile->seek (fst);
    mFile->read (&node,sizeof (node));

    if (node.next == -1)
    {
      node.next = lst;
      node.size = node.num << mShift;
      node.uid  = -1;

      mFile->seek  (fst);
      mFile->write (&node,sizeof (node));      

      break;
    }

    node.uid = -1;

    mFile->seek  (fst);
    mFile->write (&node,sizeof (node));
    fst = node.next;
  }
}

SwapFile::SwapFile  (Pool* pool,const char* fname,size_t size,size_t cl_size)
         : MemObject (pool),
           mFile     (F_OpenFile (fname,BF_OPEN_RW)),
           mSize     (size), mFree (size ? size : -1), mClaster (cl_size),
           mFirstPos (-1), mLastPos (0)
{  
  uint mask  = cl_size - 1;
  mMask      = mask;

  for (mShift=0;mask;mask>>=1,mShift++);
}

SwapFile::~SwapFile ()
{  }
