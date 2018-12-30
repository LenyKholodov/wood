#include "pch.h"
#include "r_alphamesh.h"
#include <gl\gl.h>

void  AlphaMeshRender::AddAlpha  (float dist,DWORD param,Render* render)
{
  if (mPos >= mCount)
    Draw ();

  if (mFirst == -1)
  {
    int index             = mPos++;

    mTable [index].next   = -1;
    mTable [index].prev   = -1;
    mTable [index].dist   = dist;
    mTable [index].param  = param;
    mTable [index].render = render;

    mFirst                = index;
  }
  else
  {
    int next = mTable [mFirst].next;

    for (int i=mFirst;next!=-1 && mTable [i].dist > dist;i=next,next=next!=-1?mTable [next].next:-1);

    if (mTable [i].dist < dist)
    {
      int index             = mPos++;

      mTable [index].next   = i;
      mTable [index].prev   = mTable [i].prev;

      mTable [index].dist   = dist;
      mTable [index].param  = param; 
      mTable [index].render = render;

      if (mTable [i].prev != -1) mTable [mTable [i].prev].next = index;

      mTable [i].prev       = index;

      if (i == mFirst) mFirst = index;
    }
    else
    {
      int index             = mPos++;

      mTable [index].next   = mTable [i].next;
      mTable [index].prev   = i;

      if (mTable [i].next != -1) mTable [mTable [i].next].prev = index;

      mTable [index].dist   = dist;
      mTable [index].param  = param; 
      mTable [index].render = render;

      mTable [i].next       = index;
    }
  }
}

void  AlphaMeshRender::Draw  ()
{
  D_Try
  {
    glEnable    (GL_BLEND);
    glDepthMask (GL_FALSE);
    glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

//    dout<<"begin alpha"<<endl;

    for (int i=mFirst;i!=-1;i=mTable [i].next)
    {
      mTable [i].render->DrawAlpha (mTable [i].param,mCamera);
//      dout<<"..."<<mTable [i].dist<<endl;
    }  

//    dout<<"end alpha"<<endl;

    glDepthMask (GL_TRUE);
    glDisable   (GL_BLEND);  
  }
  D_Exc ("AlphaMeshRender: Error at Draw");

  mFirst = -1;
  mPos   = 0;
}

AlphaMeshRender::AlphaMeshRender  (Pool* _Pool,int _Num)
                : MemObject (_Pool),
                  mCount    (_Num), mPos (0), mFirst (-1),
                  mTable    ((alphanode_t*)_Pool->alloc (_Num * sizeof (alphanode_t)))
{
  if (!mTable)
  {
    D_Message ("AlphaMeshRender: No enough memory for sys data");
    return;
  }
}

AlphaMeshRender::~AlphaMeshRender ()
{
  if (mTable) free (mTable);
}
