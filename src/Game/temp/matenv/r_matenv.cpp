#include "r_matenv.h"

#include <graph\x3f.h>
#include <string.h>
#include <misc.h>

void         MatEnv::unbind ()
{
  mCacheText.unbind   ();
}

BOOL         MatEnv::bind              (int index)
{
  D_Try
  {
    if (index < mStaticCount) return mStaticText->bind (index);
    else if (index < mStaticCount + mCacheCount)
    {
      BOOL state = mCacheText.bind   (index - mStaticCount);
      if (state)   start             (mCacheNode + index - mStaticCount);
      return state;
    }      
    else
    {
      TextEnvironment* env = GetEnv (index);

      if (env)
        env->bind (index);
    }

    return FALSE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"MatEnv: Error at bind "<<endl;
    #endif
    return FALSE;
  }
}

void         MatEnv::SetMaterial       (int index)
{
  D_Try
  { 
    TextEnvironment* env = GetEnv (index);

    if (env)
      env->SetMaterial (index);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"MatEnv: Error at SetMaterial "<<endl;
    #endif
  }
}

int          MatEnv::AllocTexture      (const char* name,ulong& crc)
{
  D_Try
  {
    crc      = CRC32 (name,strlen (name));
    int find = -1;

    if (mCRCDict.search (crc,find))
    {
      mCacheNode [find-mStaticCount].count++;

      start (mCacheNode + find - mStaticCount);

      return find;
    }    

    cachenode_t* node = mLast.prev;

    SetTextureName (node->index,name);
    start          (mCacheNode + node->index - mStaticCount);    

    return node->index;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"MatEnv: Error at AllocTexture '"<<name<<"'"<<endl;
    #endif
    return -1;
  }
}

void         MatEnv::FreeTexture       (int index)
{
  D_Try
  {
    if (index < mStaticCount || index >= mStaticCount + mCacheCount)
      return;

    cachenode_t* node = mCacheNode + index - mStaticCount;

    if (--node->count == 0)
    {
      mCRCDict.erase (node->crc);
      end            (node);
    }    
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"MatEnv: Error at FreeTexture "<<endl;
    #endif
  }
}

uint         MatEnv::UseCount          (int index)
{
  if (index < 0 || index >= mStaticCount + mCacheCount)
    return 0;

  if (index < mStaticCount) return 1;
  else                      return mCacheNode [index - mStaticCount].count;
}

ulong        MatEnv::GetCRC            (int index)
{
  if (index < 0 || index >= mStaticCount + mCacheCount)
    return 0;

  if (index < mStaticCount) return CRC32 (GetTextureName (index),strlen (GetTextureName (index)));
  else                      return mCacheNode [index - mStaticCount].crc;
}

void         MatEnv::start             (cachenode_t* node)
{
  node->prev->next  = node->next;
  node->next->prev  = node->prev;

  node->next        = mFirst.next;
  node->prev        = &mFirst;
  mFirst.next->prev = node;
  mFirst.next       = node;

  mCacheText.SetFirst (node->index - mStaticCount);
}

void         MatEnv::end               (cachenode_t* node)
{
  node->prev->next  = node->next;
  node->next->prev  = node->prev;

  node->prev        = mLast.prev;
  node->next        = &mLast;
  mLast.prev->next  = node;
  mLast.prev        = node;

  mCacheText.SetLast (node->index - mStaticCount);
}

void         MatEnv::SetTextmat        (int index,const textmat_t& mat)
{
  SetMaterial    (index,mat.m);
  SetTextureName (index,mat.text);
}

void         MatEnv::SetMaterial       (int index,const material_t& m)
{
  TextEnvironment* env = GetEnv (index);

  if (env)
    env->SetMaterial (index,m);
}

material_t&  MatEnv::GetMaterial       (int index)
{
  TextEnvironment* env = GetEnv (index);  

  if (env)
  {
    return env->GetMaterial (index);
  }  
  else
    return mStaticText->GetMaterial (index);  
}

const  char* MatEnv::GetTextureName    (int index)
{
  TextEnvironment* env = GetEnv (index);

  if (env)
    return env->GetTextureName (index);
  else
    return NULL;
}

BOOL         MatEnv::SetTextureName    (int index,const char* name)
{
  D_Try
  {
    if (index < mStaticCount) return mStaticText->SetTextureName (index,name);
    else if (index < mStaticCount + mCacheCount)
    {
      cachenode_t& node = mCacheNode [index - mStaticCount];

      mCRCDict.erase  (node.crc);      
      mCRCDict.insert (node.crc = CRC32 (name,strlen (name)),node.index);
      
      return mCacheText.SetTextureName (index - mStaticCount,name);
    }
    else
    {
      TextEnvironment* env = GetEnv (index);

      if (env)
        return env->SetTextureName (index,name);
      else
        return FALSE;
    }
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"MatEnv: Error at SetTextureName "<<index<<" '"<<name<<"'"<<endl;
    #endif
    return FALSE;
  }
}

bool         MatEnv::valid             () const
{
  return IsValid (&mCacheText) && IsValid (mStaticText) && mCacheNode != NULL && mCRCDict.valid ();
}

int          MatEnv::AllocTextureRange (uint count,uint cacheSize)
{
  D_Try
  {
    if (!count)
      return -1;

    textrange_t      range;
    TextEnvironment* env = new (pool()) TextEnvironment (pool(),count,cacheSize);

    range.env   = env;

    if (!IsValid (range.env))
      return -1;

    range.start = mTextAllocPos;
    range.end   = mTextAllocPos += count - 1;

    if (!mEnvRing.insert (range))
    {
      mTextAllocPos -= count;
      delete range.env;
      return NULL;
    }

    return range.start;
  }
  D_ExcRet ("MatEnv: Error at AllocTextureRange",-1);  
}

void         MatEnv::FreeTextureRange  (int first)
{
  D_Try
  {
    if (GetEnv (first) && first >= mStaticCount + mCacheCount)
    {
      TextEnvRing::iterator iter   = mEnvRing;
      int                   count  = mEnvRing.count ();

      for (int i=0;i<count;i++,iter++)
        if (first >= iter.data().start && first <= iter.data().end)
        {
          delete iter.data().env;

          mEnvRing.erase (iter);
          return;
        }
    }      
  }
  D_Exc ("MatEnv: Error at FreeTextureRange");
}

TextEnvironment*     MatEnv::GetEnv            (int& index)
{
  D_Try
  {
    if      (index < mStaticCount)                return mStaticText;
    else if (index < mStaticCount + mCacheCount)  
    {
      index -= mStaticCount;
      return &mCacheText;
    }  
    else
    {
      TextEnvRing::iterator iter   = mEnvRing;
      int                   count  = mEnvRing.count ();

      for (int i=0;i<count;i++,iter++)
        if (index >= iter.data().start && index <= iter.data().end)
        {
          index -= iter.data().start;
          return iter.data().env;    
        }  
    }

    return NULL;
  }
  D_ExcRet ("MatEnv: Error at GetEnv",NULL);
}

MatEnv::MatEnv  
 ( Pool*        _Pool,
   GameLevel*   _Level,
   int          _CacheTexturesNum,
   int          _HWStaticTexturesNum
 )
   : MemObject    (_Pool),
     mCacheText   (_Pool,_CacheTexturesNum,_CacheTexturesNum),
     mCRCDict     (_Pool),
     mStaticText  (NULL),
     mCacheNode   (NULL),
     mCacheCount  (_CacheTexturesNum),
     mStaticCount (0),
     mCreateStaticText (TRUE),
     mTextAllocPos (mStaticCount+mCacheCount)
{
  D_Try
  {
    if (!IsValid (&mCRCDict) || !IsValid (&mCacheText))
    {
      #ifdef DEBUG
        dout<<"MatEnv: No enough memory for sys data "<<endl;
      #endif      
      return;
    }

    BinFile* file = _Level->CreateLump (LL_TEXTURES,GameLevel::OPEN_RO);

    mStaticText   = new (pool()) TextEnvironment (pool(),file,_HWStaticTexturesNum);

    if (file) delete file;

    if (!IsValid (mStaticText))
    {
      #ifdef DEBUG
        dout<<"MatEnv: No enough memory for StaticText "<<endl;
      #endif
      return;
    }                            

    mStaticCount  = mStaticText->count ();
    mCacheNode    = (cachenode_t*)pool()->alloc (sizeof (cachenode_t) * mCacheCount);

    if (!mCacheNode)
    {
      #ifdef DEBUG
        dout<<"MatEnv: No enough memory for sys data "<<endl;
      #endif      
      return;
    }

    for (int i=0;i<mCacheCount;i++)
    {
      mCacheNode [i].prev   = &mCacheNode [i-1];
      mCacheNode [i].next   = &mCacheNode [i+1];
      mCacheNode [i].index  = i + mStaticCount;
      mCacheNode [i].crc    = 0;      
      mCacheNode [i].count  = 0;
    }    

    mCacheNode [0].prev                 = &mFirst;
    mCacheNode [mCacheCount - 1].next   = &mLast;
    mFirst.prev = mLast.next            = NULL;
    mFirst.next                         = mCacheNode;
    mLast.prev                          = &mCacheNode [mCacheCount - 1];

    mTextAllocPos                       = mStaticCount + mCacheCount;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"MatEnv: Error at construct "<<endl;
    #endif
  }
}

MatEnv::MatEnv  
 ( Pool*                _Pool,
   TextEnvironment*     _StaticText,
   int                  _CacheTexturesNum
 ) 
   : MemObject   (_Pool),
     mCRCDict    (_Pool),
     mCacheText  (_Pool,_CacheTexturesNum,_CacheTexturesNum),
     mStaticText (_StaticText),
     mCacheCount (_CacheTexturesNum),
     mStaticCount(_StaticText->count ()),
     mCacheNode  (NULL),
     mCreateStaticText (FALSE),
     mTextAllocPos (mStaticCount+mCacheCount)
{
  D_Try
  {     
    if (!IsValid (&mCRCDict) || !IsValid (mStaticText) || !IsValid (&mCacheText))
    {
      #ifdef DEBUG
        dout<<"MatEnv: No enough memory for sys data "<<endl;
      #endif      
      return;
    }

    mCacheNode = (cachenode_t*)pool()->alloc (sizeof (cachenode_t) * mCacheCount);

    if (!mCacheNode)
    {
      #ifdef DEBUG
        dout<<"MatEnv: No enough memory for sys data "<<endl;
      #endif      
      return;
    }

    for (int i=0;i<mCacheCount;i++)
    {
      mCacheNode [i].prev   = &mCacheNode [i-1];
      mCacheNode [i].next   = &mCacheNode [i+1];
      mCacheNode [i].index  = i + mStaticCount;
      mCacheNode [i].crc    = 0;      
      mCacheNode [i].count  = 0;
    }    

    mCacheNode [0].prev                 = &mFirst;
    mCacheNode [mCacheCount - 1].next   = &mLast;
    mFirst.prev = mLast.next            = NULL;
    mFirst.next                         = mCacheNode;
    mLast.prev                          = &mCacheNode [mCacheCount - 1];
    mTextAllocPos                       = mStaticCount + mCacheCount;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {           
    #ifdef  DEBUG
      dout<<"MatEnv: Error at construct "<<endl;
    #endif
  }
}

MatEnv::~MatEnv ()
{
  D_Try
  {
    if (mCreateStaticText && mStaticText) delete mStaticText;
    if (mCacheNode)                       pool()->free (mCacheNode);

    TextEnvRing::iterator iter   = mEnvRing;
    int                   count  = mEnvRing.count ();

    for (int i=0;i<count;i++,iter++)
      delete iter.data().env;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {           
    #ifdef DEBUG
      dout<<"MatEnv: Error at destruct "<<endl;
    #endif
  }
}
