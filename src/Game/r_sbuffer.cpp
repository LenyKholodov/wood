#include "pch.h"
#include "r_sbuffer.h"
#include <misc.h>

struct   span_t
{
  int   x1, x2;
  float minh, maxh;
};

void            SBuffer::reset   ()
{
  mVisPos    = 0;

  float* buf = mSBuffer;
  float  x   = -mPos.y;
  
  for (int i=0;i<mSegSize;i++,*buf++=x);
}

void            SBuffer::set     (const vector3d_t& pos,float angle,const float* seg)
{
  mPos       = pos;

  mPrjLine.a = -(seg [3] - seg [1]);
  mPrjLine.b = seg [2] - seg [0];
  mPrjLine.c = -(seg [0] * mPrjLine.a + seg [1] * mPrjLine.b);

  normalize (mPrjLine);

  mCos       = cos (-angle);
  mSin       = sin (-angle);

  float k    = float (mSegSize) / fabs (mCos * (seg [2] - seg [0]) + mSin * (seg [1] - seg [3]));
  mCos      *= k;
  mSin      *= k;

  reset ();
}

BOOL            SBuffer::insert  (int x,int y,int index,int detail,const boundbox_t& box)
{
  span_t span;

  mVisible = FALSE;

  if (!calc (box,span))    mVisible = TRUE;
  else                     insert (span);

  if (mVisible)
  {
    if (mVisPos == mAllocSize)
    {    
      if (mAllocSize) 
        dout<<"SBuffer: Overflow at insert visnode "<<this<<endl;

      return TRUE;
    }

    visnode_t& node = mVisNode [mVisPos++];

    node.index      = index;
    node.detail     = detail;
    node.x          = x;
    node.y          = y;

    return TRUE;
  }

  return FALSE;
}

BOOL            SBuffer::insert  (const boundbox_t& box)
{
  span_t span;

  mVisible = FALSE;

  if (!calc (box,span))    mVisible = TRUE;  //??????????
  else                     insert (span);

  return mVisible;
}

BOOL            SBuffer::check   (const boundbox_t& box)
{
  span_t span;

  mVisible = FALSE;

  if (!calc (box,span))    
   return  mVisible = TRUE; 
  else                     
  {
    if (span.x2 < 0 || span.x1 >= mSegSize)
      return mVisible = TRUE;

    span.minh = -2.0f * mPos.y;

    insert (span);
  }

  return mVisible;
}

visnode_t*      SBuffer::get     (int& count)
{
  count = mVisPos;
  return mVisNode;
}

BOOL            SBuffer::cross   (const line_t& l,float& x,float& y)
{
  if (fabs (mPrjLine.a) > EPS && !(fabs (l.b) < EPS && fabs (mPrjLine.b) < EPS))
  {
    float k = -l.a / mPrjLine.a;
    
    y       = -(l.c + mPrjLine.c * k) / (l.b + mPrjLine.b * k);
    x       = -(mPrjLine.c + mPrjLine.b * y) / mPrjLine.a;
  }
  else if (fabs (mPrjLine.b) > EPS && !(fabs (l.a) < EPS && fabs (mPrjLine.a) < EPS))
  {
    float k = -l.b / mPrjLine.b;

    x       = -(l.c + mPrjLine.c * k) / (l.a + mPrjLine.a * k);
    y       = -(mPrjLine.c + mPrjLine.a * x) / mPrjLine.b;
  }
  else 
    return FALSE;

  return TRUE;
}

BOOL            SBuffer::calc    (const boundbox_t& box,span_t& span)
{
  line_t line [2];
  float  v [2][2];
  float  x,y;
  char   code = 0;

  if (mPos.x < box.min.x) code |= 1;
  if (mPos.z < box.min.z) code |= 2;
  if (mPos.x > box.max.x) code |= 4;
  if (mPos.z > box.max.z) code |= 8;

  switch (code)
  {
    case 1: //+
      v [0][0] = box.min.x;
      v [0][1] = box.min.z;      
      v [1][0] = box.min.x;
      v [1][1] = box.max.z;      
      break;  
    case 8:  //+
      v [0][0] = box.min.x;
      v [0][1] = box.max.z;      
      v [1][0] = box.max.x;
      v [1][1] = box.max.z;      
      break;  
    case 9:  //+
      v [0][0] = box.min.x;
      v [0][1] = box.min.z;      
      v [1][0] = box.max.x;
      v [1][1] = box.max.z;      
      break;  
    case 4:  //+
      v [0][0] = box.max.x;
      v [0][1] = box.min.z;      
      v [1][0] = box.max.x;
      v [1][1] = box.max.z;      
      break;  
    case 12:  //+
      v [0][0] = box.min.x;
      v [0][1] = box.max.z;      
      v [1][0] = box.max.x;
      v [1][1] = box.min.z;      
      break;  
    case 2:  //+
      v [0][0] = box.min.x;
      v [0][1] = box.min.z;      
      v [1][0] = box.max.x;
      v [1][1] = box.min.z;      
      break;  
    case 3:   //+
      v [1][0] = box.min.x;
      v [1][1] = box.max.z;      
      v [0][0] = box.max.x;
      v [0][1] = box.min.z;      
      break;  
    case 6:  //+
      v [1][0] = box.min.x;
      v [1][1] = box.min.z;      
      v [0][0] = box.max.x;
      v [0][1] = box.max.z;      
      break;  
    default:
      return FALSE;
  }

  float dx1 = v [0][0] - mPos.x,
        dy1 = v [0][1] - mPos.z,
        dx2 = mPos.x   - v [1][0],
        dy2 = mPos.z   - v [1][1];

  line [0].a = -dy1;
  line [0].b = dx1;
  line [0].c = -(mPos.x*line [0].a+mPos.z*line [0].b);
  line [1].a = -dy2;
  line [1].b = dx2;
  line [1].c = -(mPos.x*line [1].a+mPos.z*line [1].b);

  if (!cross (line [0],x,y)) 
    return FALSE;

  span.x1    = (x-mPos.x) * mCos - (y-mPos.z) * mSin + (mSegSize>>1);

  if (!cross (line [1],x,y))
    return FALSE;

  span.x2    = (x-mPos.x) * mCos - (y-mPos.z) * mSin + (mSegSize>>1);

  if (span.x2 < span.x1) 
    swap (span.x2,span.x1);

  span.minh  = box.min.y - mPos.y;
  span.maxh  = box.max.y - mPos.y;
  
  return TRUE;
}

void            SBuffer::insert  (span_t& span)
{
  if (span.x2 < 0 || span.x1 >= mSegSize)
    return ;
   
   //ВВести проверку на минимальную высоту буфера

  if (span.x1 <  0)                 span.x1 = 0;
  if (span.x2 >= mSegSize)          span.x2 = mSegSize - 1;

  int     xs   = span.x1, 
          xe   = span.x2;
  float   minh = span.minh,
          maxh = span.maxh;
  float*  buf  = mSBuffer + xs;

  for (int x=xs;x<=xe;x++,buf++)
    if (maxh >= *buf) 
    {
      mVisible = TRUE;
      
      if (minh > *buf && minh >= 0) *buf = minh;
//      if (minh > *buf) *buf = minh;
    }        
}

SBuffer::SBuffer  (Pool* _Pool,int _NodeCount,int _SegmentSize)
        : MemObject  (_Pool),
          mSegSize   (_SegmentSize),
          mAllocSize (_NodeCount),
          mSBuffer   (NULL),
          mVisNode   (NULL)          
{
  D_Try
  {
    mSBuffer = (float*)GetPool()->alloc (sizeof (float) * _SegmentSize);
    mVisNode = (visnode_t*)GetPool()->alloc (sizeof (visnode_t) * _NodeCount);

    if ((!mVisNode && mAllocSize) || !mSBuffer)
    {
      #ifdef DEBUG
        dout<<"SBUffer: No enough memory "<<this<<endl;
      #endif
//      D_Error (EXCEPTION_ERROR);
      return; //throw  
    }

    reset ();
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"SBuffer: Error at construct "<<this<<endl;
    #endif

    this->~SBuffer ();
  }
}

SBuffer::~SBuffer ()
{
  D_Try
  {
    if (mVisNode) GetPool ()->free (mVisNode);
    if (mSBuffer) GetPool ()->free (mSBuffer);

    mSBuffer = NULL;
    mVisNode = NULL;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"SBuffer: Error at destruct "<<this<<endl;
    #endif
  }
}

