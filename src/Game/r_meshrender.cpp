#include "pch.h"
#include "r_meshrender.h"
#include <gl\gl.h>

BOOL         MeshRender::DrawMesh    (int index,ulong id)
{
  if (index < 0 || index >= mListNum || !RENDER_USE_MESH_RENDER)
    return FALSE;
  
  IRing::iterator& iter = mIter [index];

  if (iter.data ().id != id)
    return FALSE;

  glCallList          (iter.data ().list + mFirstList);  
  mIndexRing.SetFirst (iter);

  return TRUE;
}

BOOL         MeshRender::BeginMesh   (ulong id,int& list)
{
  if (!RENDER_USE_MESH_RENDER)
    return TRUE;

  int             index;

  if (mIndexDict.search (id,index))
  {
    IRing::iterator iter = mIter [index];
    list                 = iter.data ().list;

    mIndexRing.SetFirst (iter);

    return FALSE;
  }
  else
  {
    IRing::iterator iter = IRing::iterator (mIndexRing); --iter;

    mIndexDict.erase  (iter.data ().id);
    mIndexDict.insert (iter.data ().id = id,iter.data ().list);
    glNewList         (iter.data ().list + mFirstList,GL_COMPILE);  
    mIndexRing.SetFirst  (mIter [list = mCurList = iter.data ().list]);

    return TRUE;
  }
}

void         MeshRender::EndMesh     ()
{
  if (mCurList < 0 || mCurList >= mListNum || !RENDER_USE_MESH_RENDER)
    return;

  glEndList ();

  mCurList = -1;
}

MeshRender::MeshRender  (Pool* _Pool,int _MeshesCacheNum)
           : MemObject   (_Pool),
             mIndexRing  (_Pool),
             mIndexDict  (_Pool),
             mListNum    (_MeshesCacheNum),
             mFirstList  (glGenLists (_MeshesCacheNum)),
             mIter       (NULL),
             mCurList    (-1)
{
  D_Try
  {
    if (!IsValid (&mIndexRing) || !IsValid (&mIndexDict))
    {
      #ifdef DEBUG
        dout<<"MeshRender: No enough meomory for sys data "<<endl;
      #endif
      return;
    }

    mIter = (IRing::iterator*)GetPool()->alloc (sizeof (IRing::iterator) * mListNum);

    if (!mIter)
    {
      #ifdef DEBUG
        dout<<"MeshRender: No enough meomory for sys data "<<endl;
      #endif
      return;
    }

    for (int i=0;i<mListNum;i++)
    {
      meshid_t id = {i,-1};

      mIndexRing.insert (id);

      mIter [i] = IRing::iterator (mIndexRing);      
      mIter [i]--;
    }  
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"MeshRender: Error at construct "<<endl;
    #endif
  }
}

MeshRender::~MeshRender ()
{
  D_Try
  {     
    glDeleteLists (mFirstList,mListNum);

    if (mIter) GetPool()->free (mIter);
  }          
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {           
    #ifdef DEBUG
      dout<<"MeshRender: Error at destruct "<<endl;
    #endif
  }
}
