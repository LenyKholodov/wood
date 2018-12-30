#include "pch.h"
#include "cv_mdlmulti.h"
#include "r_mdlmulti.h"

#include <gl\gl.h>

//////////////////////implementation of class MultiModelConv//////////////////////

BOOL        MultiModelConv::BuildMeshes   ()
{
  M_PushPool (GetPool());

  D_Try
  {
    facet_t* facet = mFacet;
    mMesh          = new FacetRing [mMCount + 1];

    for (int i=0;i<mFCount;i++,facet++)
      mMesh [++facet->texture].insert (i);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"MultiModelConv: Error at BuildMeshes"<<endl;
    #endif
    return FALSE;
  }        

  M_PopPool ();

  return TRUE;
}

void        MultiModelConv::Save          (const char* name,ostream* os)
{
  D_Try
  {
    if (mMCount > 0x10000)
    {
      dout<<"MultiModelConv: Multi model materials > 0x10000. Call to DadyCool"<<endl;
      return;
    }

    BinFile* file = F_OpenFile (name,BF_OPEN_CR);

    if (!IsValid (file))
    {
      D_Message ("MultiModelConv: Error at create file '"<<name<<"'");
      if (file) delete file;
      return;
    }

    m3dheader_t hdr;
    static      textmat_t   defMat (0);

    hdr.id    = MULTI_MODEL_ID;
    hdr.size  = sizeof (vertex_t) * mVCount +
                sizeof (facet_t) * mFCount +
                sizeof (vector3d_t) * mNCount +
                sizeof (multimodeldata_t) +
                mMatStrSize + strlen (defMat.name) + 1;

    multimodeldata_t   data;
    static ulong       meshOffs [0x10000];
    ulong              pos = sizeof (ulong) * (mMCount + 1);

    for (int i=0;i<mMCount + 1;i++)
    {
      meshOffs [i]  = pos;
      pos          += mMesh [i].count () * sizeof (int) + sizeof (mesh_t);
    }  

    hdr.size         += pos;

    data.bound        = mBound;
    
    data.vertex.num   = mVCount;
    data.vertex.size  = sizeof (vertex_t) * mVCount;
    data.vertex.offs  = 0;

    data.facet.num    = mFCount;
    data.facet.size   = sizeof (facet_t) * mFCount;
    data.facet.offs   = data.vertex.size;

    data.normal.num   = mNCount;
    data.normal.size  = sizeof (vector3d_t) * mNCount;
    data.normal.offs  = data.vertex.size + data.facet.size;

    mMCount++;

    data.matname.num  = mMCount;
    data.matname.size = mMatStrSize + strlen (defMat.name) + 1;
    data.matname.offs = data.vertex.size + data.facet.size + data.normal.size;

    data.mesh.num     = mMCount;
    data.mesh.size    = pos + mMCount * sizeof (ulong);
    data.mesh.offs    = data.matname.size + data.matname.offs;

    file->write (&hdr,sizeof (hdr));
    file->write (&data,sizeof (data));

    file->write (mVertex,data.vertex.size);
    file->write (mFacet,data.facet.size);
    file->write (mNormal,data.normal.size);
    file->write (defMat.name,strlen (defMat.name) + 1);
    file->write (mMatStr,mMatStrSize);

    for (i=0;i<mMCount;i++)
      meshOffs [i] += data.mesh.offs;

    file->write (meshOffs,sizeof (ulong) * mMCount); 

    static  char   temp  [1024*1024*4 + 4096];

    for (i=0;i<mMCount;i++)
    {   
      FacetRing::iterator iter  = mMesh [i];
      int                 count = mMesh [i].count ();
      mesh_t*             mesh  = (mesh_t*)temp;      

      if (count < 1024*1024)
      {
        int* index      = (int*)mesh->data;
        int  mini       = *index,
             maxi       = *index;        

        for (int j=0;j<count;j++,iter++,index++)
        {
          *index = iter.data ();

          if (*index < mini) mini = *index;
          if (*index > maxi) maxi = *index;
        }  

        mesh->type      = GL_TRIANGLES;
        mesh->elType    = GL_UNSIGNED_INT;
        mesh->vbuffer   = -1;
        mesh->material  = i;
        mesh->count     = count;
        mesh->start     = mini;
        mesh->end       = maxi;
        mesh->index     = i;
        mesh->size      = sizeof (mesh_t) + count * sizeof (int);

        file->write (mesh,mesh->size);
      }
      else      
      {
        cout<<"MultiModelConv: Error too many facets: "<<count<<". Call to DadyCool"<<endl;
        return;
      }  
    }

    delete file;

    (*os)<<"Process model: '"<<name<<"'"<<endl;
    (*os)<<"\t\tVertexes:\t"<<mVCount<<endl;
    (*os)<<"\t\tFacets:\t\t"<<mFCount<<endl;
    (*os)<<"\t\tMaterials:\t"<<mMCount<<endl;
  }
  D_Exc ("MultiModelConv: Error at Save");
}

MultiModelConv::MultiModelConv  (Pool* _Pool,Object3D& obj,const vector3d_t& axis)
             : M3DModelConv (_Pool,obj,axis),
               mMesh        (NULL)

{   
  BuildMeshes ();
}

MultiModelConv::~MultiModelConv ()
{
  D_Try
  {
    if (mMesh)   delete [] mMesh; //GetPool()->free ???
  }     
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"MultiModelConv: Error at destruct "<<endl;
    #endif
  }
}
