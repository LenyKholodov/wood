#include "pch.h"
#include "g_boxcache.h"

#define TI(X,Y) ((Y-mYs)*mLoadWidth+(X-mXs))
#define LTI(X,Y) ((Y-box->y)*box->width+(X-box->x))

dcachenode_t*     BoxCache::FindNode (int x,int y)
{
  for (cachenode_t* node = mFirst.next;node!=&mLast;node=node->next)
  {
    boxdesc_t* box = (boxdesc_t*)node->buf;
    if (box->x <= x && box->y <= y && box->x + box->width > x && box->y + box->height > y)
      return (dcachenode_t*)node;
  }
  return NULL;
}

void       BoxCache::SetCacheBox     (int width,int height)
{
  mCacheWidth  = width  ? width  : mWidth;
  mCacheHeight = height ? height : mHeight;
}

BoxCache::boxdesc_t* BoxCache::AllocBox     (int x,int y,BOOL is_lock)
{
  dcachenode_t*      node  = FindNode (x,y);

  if (!node)    
  {
    int xs     = x - x % mCacheWidth,
        ys     = y - y % mCacheHeight;
    int width  = (xs + mCacheWidth <= mWidth)   ? mCacheWidth  : mWidth  - xs;
    int height = (ys + mCacheHeight <= mHeight) ? mCacheHeight : mHeight - ys;

    LoadInfo (xs,ys,width,height);

    if (!mInfo) return NULL;

    node       = AllocBox (xs,ys,width,height);

    while (!node && width >= 1 && height >= 1)
    {
      width>>=1; height>>=1;
      node = AllocBox (x,y,width,height);
    }  

    if (!node)
      node = AllocBox (x,y,1,1); 

    if (node)   
    {
      if (!LoadData ((boxdesc_t*)node->buf))
        return NULL;
    }  
    else       
      return NULL;
  }

  start (node);

  if (is_lock) FloatCache::lock (node);

  return (boxdesc_t*)node->buf;
}

BOOL    BoxCache::LoadData        (BoxCache::boxdesc_t* box)
{
  D_Try
  {     
    box->first     = box->data + sizeof (ulong) * box->height +
                     sizeof (tileinfo_t) * box->width * box->height;
    box->info      = (tileinfo_t*)(box->data + sizeof (ulong) * box->height);

    int         pos   = TI (box->x,box->y);
    ulong*      table = (ulong*)box->data;
    ulong       offs  = 0;
    char*       data  = box->first;
    tileinfo_t* loffs = box->info;

    if (box->width == 0)        
    {
      table [0] = 0;
      return FALSE;   
    }  

    for (int i=0;i<box->height;i++,pos += mLoadWidth)
    {
      ulong len  = mInfo [pos+box->width-1].offs - mInfo [pos].offs + 
                   mInfo [pos+box->width-1].size;

      mFile->seek (mInfo [pos].offs);
      mFile->read (data,len);

      table [i]  = offs;
      offs      += len;
      data      += len;

      tileinfo_t* src  = mInfo + pos;
      tileinfo_t* dest = loffs;

      for (int x=0;x<box->width;*dest++ = *src++,x++);

      loffs += box->width;
    }

    return TRUE;
  } 
  D_ExcRet ("BoxCache: Error at LoadData",FALSE);
}

void*      BoxCache::lock            (int x,int y)
{
  D_Try
  {
    boxdesc_t* box = AllocBox (x,y,TRUE); 

    if (box)
    {
      mLockData++;

      ulong*             offs  = (ulong*)box->data;
      const tileinfo_t*  info  = box->info + LTI (x,y);
      const tileinfo_t&  first = box->info [LTI (box->x,y)];

      return box->first + offs [y-box->y] + info->offs - first.offs;
    }
    else
      return NULL;      
  }
  D_ExcRet ("BoxCache: Error at lock",NULL);
}

void*      BoxCache::get             (int x,int y)
{
  D_Try
  {
    boxdesc_t* box = AllocBox (x,y);    

    if (box)
    {
      ulong*             offs  = (ulong*)box->data;
      const tileinfo_t*  info  = box->info + LTI (x,y);
      const tileinfo_t&  first = box->info [LTI (box->x,y)];

      return box->first + offs [y-box->y] + info->offs - first.offs;
    }  
    else
      return NULL;      
  }
  D_ExcRet ("BoxCache: Error at get (int)",NULL);
}

void       BoxCache::unlock          (int x,int y)
{
  D_Try
  {
    const tileinfo_t*  info  = mInfo + TI (x,y);
    dcachenode_t*      node  = FindNode (info->coord.x,info->coord.y);

    if (node)
    {
      FloatCache::unlock (node);
      mLockData--;
    }  
  }
  D_Exc ("BoxCache: Error at unlock");
}

void       BoxCache::reset           ()
{
  FloatCache::reset ();

  mLockData = FALSE;
}

dcachenode_t*     BoxCache::AllocBox  (int x,int y,int width,int height)
{
  D_Try
  {
    size_t size       = sizeof (boxdesc_t) + sizeof (ulong) * height +
                        sizeof (tileinfo_t) * width * height;
    ulong  pos        = TI (x,y);

    for (int i=0;i<height;i++,pos += mLoadWidth)
      size += mInfo [pos+width-1].offs - mInfo [pos].offs +
              mInfo [pos+width-1].size;

    dcachenode_t* node = AllocChunk (size);

    if (!node)
      return NULL;

    boxdesc_t* box = (boxdesc_t*)node->buf;

    box->x      = x;
    box->y      = y;
    box->width  = width;
    box->height = height;
    box->first  = NULL;
    box->info   = NULL;

    return node;
  }
  D_ExcRet ("BoxCache: Error at AllocBox (private)",NULL);
}

BOOL  BoxCache::MoveData     (DWORD,void* from,void* to)
{
  boxdesc_t*   src   = (boxdesc_t*)from;
  boxdesc_t*   dest  = (boxdesc_t*)to;  

  src->first        += ulong (dest) - ulong (src);

  return TRUE;
}

BoxCache::BoxCache (Pool* _Pool,BinFile* file,size_t cache_size,int width,int height)
         : FloatCache  (_Pool,cache_size,FLUSH_EVENT|READ_ONLY|MOVE_DATA_EVENT),
           mFile       (file),
           mWidth      (width), mHeight (height),
           mCacheWidth (width), mCacheHeight (height),
           mXs         (0), mYs (0), mLoadWidth (0), mLoadHeight (0),
           mInfo       (NULL)
{   }

BoxCache::~BoxCache()
{   }
