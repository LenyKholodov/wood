#include "pch.h"
#include "r_hmlist.h"

BOOL      HMapRenderList::AddFAN    (ulong s,ulong c,ulong e,uint x,uint y,uint w,uint h,hmedge_t edge) 
{
  if (mIPos + 6 <= mICount)
  {
    uint*   list = mIndex + mIPos;
    uchar*  map  = mMap.mMap; 

    uchar   mid  = (map [c] + map [c+1] + 
                    map [c-mMap.mLoadWidth] + map [c-mMap.mLoadWidth+1]
                   ) >> 2;

    list [0]     = AddVertex (x+(w>>1), y+(h>>1), &mid,       FALSE,-0.5f,-0.5f);
    list [1]     = AddVertex (x,        y,        map+s,      edge.flag & HM_EDGE_X1_Y1);
    list [2]     = AddVertex (x,        y+h,      map+e-w+1,  edge.flag & HM_EDGE_X1_Y2);
    list [3]     = AddVertex (x+w,      y+h,      map+e,      edge.flag & HM_EDGE_X2_Y2);
    list [4]     = AddVertex (x+w,      y,        map+s+w-1,  edge.flag & HM_EDGE_X2_Y1);
    list [5]     = list [1];

    if (list [0] == -1 || list [1] == -1 || list [2] == -1 || list [3] == -1 ||
        list [4] == -1) return FALSE;

    mIPos += 6;

    return TRUE;
  }
  else
    return FALSE;      
}

void      HMapRenderList::NextTile  (uint x,uint y)
{
  mXTile = x;
  mYTile = y;

  int width   = mMap.mTileWidth  >> 1,
      height  = mMap.mTileHeight >> 1,
      size    = (width+1) * (height+1),
      *ptr    = mData;

  for (int i=0;i<size;i++,*ptr++=-1);
}

void      HMapRenderList::reset     ()
{
  int width   = mMap.mLoadWidth  >> 1,
      height  = mMap.mLoadHeight >> 1;

  for (int x=0;x<=mMap.mInfoWidth;x++)
  {
    int* line = mYLine [x];
    for (int i=0;i<=height;*line++=-1);
  }  

  for (int y=0;y<=mMap.mInfoHeight;y++)
  {
    int* line = mXLine [y];
    for (int i=0;i<=width;*line++=-1);
  }  
}

int       HMapRenderList::AddVertex (uint x,uint y,uchar* h,uint outside,float dx,float dy)
{
  if (outside)
  {
    uint ymask = (1l<<mYShift)-1,
         xmask = (1l<<mXShift)-1;

    if (!(y & ymask))
    {
      int* xl = mXLine [(y-mMap.mY1)>>mYShift];
      int  xi = (x-mMap.mX1) >> mXShift;
      
      if (xl [xi] != -1)    return xl [xi];
      if (mVPos >= mVCount) return -1;

      vertex_t& v = mVertex [mVPos++];

      v.x         = (float)x + dx;
      v.z         = (float)y + dy;
      v.y         = *h;

      return xl [xi] = mVPos - 1;
    }
    else
    {   
      int* yl = mYLine [(x-mMap.mX1)>>mXShift];
      int  yi = (y-mMap.mY1) >> mYShift;
      
      if (yl [yi] != -1)    return yl [yi];
      if (mVPos >= mVCount) return -1;

      vertex_t& v = mVertex [mVPos++];

      v.x         = (float)x + dx;
      v.z         = (float)y + dy;
      v.y         = *h;

      return yl [yi] = mVPos - 1;      
    }
  }
  else
  {
    uint xi = (x - mXTile) >> 1,
         yi = (y - mYTile) >> 1;    

    if (mTile [yi][xi] != -1) return mTile [yi][xi];

      if (mVPos >= mVCount) return -1;

      vertex_t& v = mVertex [mVPos++];

      v.x         = x;
      v.z         = y;
      v.y         = *h;

      return mTile [yi][xi] = mVPos - 1;          
  }  
}

HMapRenderList::HMapRenderList  (Pool* pool,HeightMap& map,uint vcount,uint icount)
               : MemObject (pool),
                 mMap      (map), 
                 mXTile    (0), mYTile (0),
                 mVCount   (vcount),
                 mICount   (icount),
                 mVPos     (0), mIPos (0)
{
  uint w = map.mTileWidth,
       h = map.mTileHeight;

  for (int i=0;w;w>>=1,i++); mXShift = i - 1;
  for (    i=0;h;h>>=1,i++); mYShift = i - 1;

  mData     = (int*)alloc      (sizeof (int) * (map.mTileWidth+1) * (map.mTileHeight+1) +
                                (map.mInfoHeight + 1) * (map.mLoadWidth) / 2 * sizeof (int) + 
                                (map.mInfoWidth + 1) * (map.mLoadHeight) / 2 * sizeof (int)
                               );
  mTile     = (int**)alloc     (sizeof (int*) * (map.mTileHeight+1));
  mVertex   = (vertex_t*)alloc (sizeof (vertex_t) * mVCount);
  mIndex    = (uint*)alloc     (sizeof (uint) * mICount);
  mXLine    = (int**)alloc     (sizeof (int*) * (map.mInfoHeight + 1));
  mYLine    = (int**)alloc     (sizeof (int*) * (map.mInfoWidth + 1));

  if (!mData || !mTile || !mVertex || !mIndex || !mXLine || !mYLine)
  {  
    D_Message ("HMapRenderList: No enough memory for sys data");
    return;
  }

  int* data    = mData;
  int  width   = mMap.mTileWidth  >> 1,
       height  = mMap.mTileHeight >> 1;

  for (i=0;i<=height;i++,data+=width+1) mTile [i] = data;
  for (i=0;i<=map.mInfoHeight;i++,data+=map.mLoadWidth>>1) mXLine [i] = data;
  for (i=0;i<=map.mInfoWidth;i++,data+=map.mLoadHeight>>1) mYLine [i] = data;

  reset ();
}

HMapRenderList::~HMapRenderList ()
{
  if (mData)    free (mData);
  if (mXLine)   free (mXLine);
  if (mYLine)   free (mYLine);
  if (mVertex)  free (mVertex);
  if (mTile)    free (mTile);
  if (mIndex)   free (mIndex);
}