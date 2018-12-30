#include "pch.h"
#include "cv_mdlm3d.h"
#include "cv_utils.h"

void        M3DModelConv::convArrays    ()
{
  mVCount = mNCount = ConvertArrays (mVCount,&mVertex,&mNormal,&mTextCoord,mFacet,mFCount,GetPool());
} 

void        M3DModelConv::normalize     (const vector3d_t& v)
{
  vector3d_t d          = mBound.box.max - mBound.box.min;
  vector3d_t s          = v / d;

  mBound.box.min       *= s;
  mBound.box.max       *= s;
  mBound.sphere.center *= s;
  mBound.sphere.r       = length (d*s) / 2.0f;

  for (int i=0;i<mVCount;i++)
    mVertex [i] *= s;
}

M3DModelConv::M3DModelConv  (Pool* _Pool,Object3D& obj,const vector3d_t& axis)
             : MemObject (_Pool),
               mVertex   (NULL),  mVCount (0),
               mNormal   (NULL),  mNCount (0),               
               mFacet    (NULL),  mFCount (0),
               mMat      (NULL),  mMCount (0),
               mTextCoord (NULL), 
               mMatStr (NULL), mMatStrSize (0)

{
  D_Try
  {
    if (!ConvertObject3D (obj,
                     GetPool(),
                     mVCount,&mVertex,
                     mFCount,&mFacet,
                     mNCount,&mNormal,
                     mMCount,&mMat
                    ))
    {
      cout<<"M3DModelConv: Error at convert model"<<endl;
      return;
    }        

    ComputeBound (mFCount,mFacet,mVertex,mBound);

    vector3d_t vc = (mBound.box.min + mBound.box.max )/ 2.0f;

    for (int i=0;i<mVCount;i++)
      mVertex [i] -= vc;

    mBound.box.min -= vc;
    mBound.box.max -= vc;

    if (fabs (axis.x) > EPS && fabs (axis.y) > EPS && fabs (axis.z) > EPS)
      normalize (axis);    

    for (i=0;i<mMCount;mMatStrSize += strlen (mMat [i++].name)+1);

    char* str = mMatStr = (char*)alloc (mMatStrSize);    

    if (str)
    {
      for (i=0;i<mMCount;i++,str += strlen (str)+1)
        strcpy (str,mMat [i].name);
    }
  }
  D_Exc ("M3DModelConv: Error at construct");
}

M3DModelConv::~M3DModelConv ()
{
  D_Try
  {
    if (mMat)           free (mMat);
    if (mNormal)        free (mNormal);
    if (mFacet)         free (mFacet);
    if (mVertex)        free (mVertex);
    if (mTextCoord)     free (mTextCoord);

    if (mMatStr)        free (mMatStr);
  }     
  D_Exc ("M3DModelConv: Error at destruct");
}

