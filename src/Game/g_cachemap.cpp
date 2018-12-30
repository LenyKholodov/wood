#include "pch.h"
#include "g_cachemap.h"
#include <misc.h>

void  CacheMap::SetPos         (float x,float y)
{
  box_t box;
  
  create (x,y,box,mCViewDepth);

  if (box.i.x1 >= mBox.i.x1 && box.i.y1 >= mBox.i.y1 && 
      box.i.x2 <= mBox.i.x2 && box.i.y2 <= mBox.i.y2)
    return;
  else
  {
    create (x,y,box,mCViewDepth*2.0f);

    uint size = (box.i.y2-box.i.y1+1) * (box.i.x2-box.i.x1+1);

    if (size > mTSize) 
    {
      D_Message ("CacheMap: resize "<<size);
      resize (mTSize=size);
    }

    reload (box);
  }    
}

void  CacheMap::SetViewDepth   (float view_depth)
{
  if (view_depth != mCViewDepth)
  {
    mCViewDepth  = mCMaxViewDepth >= view_depth     ? view_depth     : mCMaxViewDepth;
    mCViewDepth  = mCViewDepth    <  mCMinViewDepth ? mCMinViewDepth : mCViewDepth;

    mBox.i.x1    = mBox.i.x2  = mBox.i.y1  = mBox.i.y2  = 0;
    mBox.fl.x1   = mBox.fl.x2 = mBox.fl.y1 = mBox.fl.y2 = 0;

    float depth  = mCViewDepth * mTMaxViewDepth / mCMaxViewDepth;
    depth        = mTMaxViewDepth >= depth          ? depth          : mTMaxViewDepth;
    depth        = depth          <  mTMinViewDepth ? mTMinViewDepth : depth;

    if (depth != mTViewDepth)
    {
      mTViewDepth  = depth;
      uint  count  = sqr (4.0f*depth+2.0f);

      if (depth == mTMaxViewDepth)
        count  = mTWidth * mTHeight + 4;

      dout<<"CacheMap: resize: "<<count<<endl;

      resize (mTSize = count);
    }
  }          
}

void  CacheMap::clip          (CacheMap::box_t& box)
{
  if (box.i.x1 <  0)         box.i.x1  = 0;
  if (box.i.y1 <  0)         box.i.y1  = 0;
  if (box.i.x2 >= mTWidth)   box.i.x2  = mTWidth-1;
  if (box.i.y2 >= mTHeight)  box.i.y2  = mTHeight-1;

  if (box.fl.x1 <  0)        box.fl.x1 = 0;
  if (box.fl.y1 <  0)        box.fl.y1 = 0;
  if (box.fl.x2 >= mCWidth)  box.fl.x2 = mCWidth-1;
  if (box.fl.y2 >= mCHeight) box.fl.y2 = mCHeight-1;
}

void  CacheMap::create        (float x,float y,CacheMap::box_t& box,float d)
{
  float xdiv = mTWidth  / mCWidth,
        ydiv = mTHeight / mCHeight;  

  box.i.x1   = (x-d) * xdiv;
  box.i.y1   = (y-d) * ydiv;
  box.i.x2   = (x+d) * xdiv + 1;
  box.i.y2   = (y+d) * ydiv + 1;  

  box.fl.x1  = x - d,
  box.fl.y1  = y - d,
  box.fl.x2  = x + d,
  box.fl.y2  = y + d;

  clip (box);
}

void  CacheMap::Init (float cwidth,float cheight,float twidth,float theight,float mdepth)
{
  mCWidth        = cwidth;
  mCHeight       = cheight;
  mTWidth        = twidth;
  mTHeight       = theight;

  mCMaxViewDepth = sqrt (sqr (cwidth) + sqr (cheight));
  mTMaxViewDepth = sqrt (sqr (twidth) + sqr (theight));
//  mTMaxViewDepth = sqrt (twidth * theight) / 4.0f;
  mCMinViewDepth = mdepth;
  mTMinViewDepth = mdepth / mCMaxViewDepth * mTMaxViewDepth;
  mTMinViewDepth = mTMinViewDepth < 1 ? 1 : mTMinViewDepth;

  mBox.i.x1      = mBox.i.x2  = mBox.i.y1  = mBox.i.y2  = 0;
  mBox.fl.x1     = mBox.fl.x2 = mBox.fl.y1 = mBox.fl.y2 = 0;
}

CacheMap::CacheMap   (Pool* pool)
         : MemObject (pool),
           mCWidth (0), mCHeight (0), mTWidth (0), mTHeight (0),
           mCMaxViewDepth (0), mCMinViewDepth (0), 
           mTMaxViewDepth (0), mTMinViewDepth (0),
           mCViewDepth    (0), mTViewDepth (0),
           mTSize         (0)
{
  mBox.i.x1      = mBox.i.x2  = mBox.i.y1  = mBox.i.y2  = 0;
  mBox.fl.x1     = mBox.fl.x2 = mBox.fl.y1 = mBox.fl.y2 = 0;
}

CacheMap::CacheMap (
  Pool*       pool,
  float       cwidth,
  float       cheight,
  float       twidth,
  float       theight,
  float       mdepth )
         : MemObject (pool),
           mCWidth (0), mCHeight (0), mTWidth (0), mTHeight (0),
           mCMaxViewDepth (0), mCMinViewDepth (0), 
           mTMaxViewDepth (0), mTMinViewDepth (0),
           mCViewDepth    (0), mTViewDepth (0),
           mTSize         (0)
{
  Init (cwidth,cheight,twidth,theight,mdepth);
}

