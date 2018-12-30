#include "cv_mdlsmp.h"
#include "cv_utils.h"
#include "r_mdlsmp.h"

#include <graph\striper.h>

//////////////////////implementation of class SmpModelConv//////////////////////


void        SmpModelConv::Save          (const char* name,ostream* os)
{
  D_Try
  {
    BinFile* file = F_OpenFile (name,BinFile::OPEN_RW);

    if (!IsValid (file))
    {
      D_Message ("SmpModelConv: Error at create file '"<<name<<"'");
      if (file) delete file;
      return;
    }

    if (mStripEnable)
      convArrays ();

    m3dheader_t hdr;

    hdr.id    = SMP_MODEL_ID;
    hdr.size  = sizeof (vertex_t) * mVCount +
                sizeof (facet_t) * mFCount +
                sizeof (vector3d_t) * mNCount +
                sizeof (textmat_t) * mMCount + 
                sizeof (smpmodeldata_t);

    smpmodeldata_t data;
    stripdest_t*   strip = NULL;

    if (mStripEnable)
      ComputeStrip (mFCount,mFacet,1|2|4,pool());

    data.material     = mMCount ? 0 : -1;
    data.bound        = mBound;
    data.type         = strip ? MODEL_STRIP_DATA : MODEL_TRIANGLES_DATA;
    
    data.vertex.num   = mVCount;
    data.vertex.size  = sizeof (vertex_t) * mVCount;
    data.vertex.offs  = 0;

    data.facet.num    = mFCount;
    data.facet.size   = sizeof (facet_t) * mFCount;
    data.facet.offs   = data.vertex.size;

    data.normal.num   = mNCount;
    data.normal.size  = sizeof (vector3d_t) * mNCount;
    data.normal.offs  = data.vertex.size + data.facet.size;

    data.mat.num      = mMCount;
    data.mat.size     = sizeof (textmat_t) * mMCount;
    data.mat.offs     = data.vertex.size + data.facet.size + data.normal.size;

    if (strip)
    {
      data.strip.offs  = data.vertex.size + data.facet.size + data.normal.size + data.mat.size;
      data.strip.size  = strip->strip->len * sizeof (uint);
      data.strip.num   = strip->strip->len;

      data.tcoord.offs = data.strip.offs + data.strip.size;
      data.tcoord.size = sizeof (textcoord_t) * mVCount;
      data.tcoord.num  = mVCount;

      hdr.size        += data.strip.size + data.tcoord.size;
    }
    else
    {
      data.strip.num   = 0;
      data.strip.size  = 0;
      data.strip.offs  = 0;

      data.tcoord.num  = 0;
      data.tcoord.size = 0;
      data.tcoord.offs = 0;
    }

    file->write (&hdr,sizeof (hdr));
    file->write (&data,sizeof (data));

    file->write (mVertex,data.vertex.size);
    file->write (mFacet,data.facet.size);
    file->write (mNormal,data.normal.size);
    file->write (mMat,data.mat.size);
    
    if (strip)
    {
      file->write  (strip->strip->strip,data.strip.size);
      file->write  (mTextCoord,data.tcoord.size);

      pool()->free (strip);
    }    

    delete file;

    (*os)<<"Process model: '"<<name<<"'"<<endl;
    (*os)<<"\t\tVertexes:\t"<<mVCount<<endl;
    (*os)<<"\t\tFacets:\t\t"<<mFCount<<endl;
    (*os)<<"\t\tMaterials:\t"<<mMCount<<endl;
  }
  D_Exc ("SmpModelConv: Error at Save");
}

SmpModelConv::SmpModelConv  (Pool* _Pool,Object3D& obj,BOOL _Strip,const vector3d_t& axis)
             : M3DModelConv (_Pool,obj,axis), 
               mStripEnable (_Strip)

{   }

SmpModelConv::~SmpModelConv ()
{   }

