#include "pch.h"
#include "cv_mem.h"
#include "g_map.h"
#include "r_mdlmulti.h"
#include "g_level.h"
//#include "cv_treecomp.h"

#include <binfile.h>

void    BuildLandInfo   (GameLevel& level,memavail_t& info)
{
  grcacheheader_t hdr;

  BinFile*        file = level.CreateLump (LL_LAND_MESH,GameLevel::OPEN_RO);

  file->read (&hdr,sizeof (hdr));  

  uint        mapSize      = hdr.mapSize;
  uint        meshNum      = hdr.meshNum;
  size_t      size         = 0;
  tileinfo_t* ni           = new tileinfo_t [mapSize]; 

  file->seek (hdr.tablesOffs);
  file->read (ni,mapSize * sizeof (tileinfo_t));   

  for (int i=0;i<mapSize;size+=ni [i++].size);

  info.mem_min         = hdr.maxMeshSize + 0x10000;
  info.mem_max         = size + 0x10000;

  delete [] ni;
  delete file;
}  

void    BuildVBInfo             (GameLevel& level,memavail_t& info)
{
  BinFile*  file = level.CreateLump (LL_VERTEX_BUFFER,GameLevel::OPEN_RO);  

  vbheader_t vb;  

  file->read  (&vb,sizeof (vbheader_t));

  info.mem_min   = vb.vbSize + 0x1000;
  info.mem_max   = vb.vbSize * vb.count + 0x1000;

  delete file;
} 

multimodeldata_t*  LoadModel     (const char* modelname,size_t& size)
{
  D_Try
  {
    m3dheader_t         hdr;
    multimodeldata_t*   model = NULL;
    size                      = 0;

    BinFile* src = F_OpenFile (modelname,BF_OPEN_RO);

    if (!IsValid (src))
      return NULL;

    src->read (&hdr,sizeof (hdr));

    if (hdr.id != MULTI_MODEL_ID)
    {
      D_Message ("::Wrong model format '"<<modelname<<"'");
      delete src;
      return FALSE;
    }

    size = hdr.size;

    D_Try
    {
      model = (multimodeldata_t*)(new char [hdr.size]);

      if (!model)
      {
        delete src;
        return model;
      }

      src->read (model,hdr.size);
    }
    D_Exception (EXCEPTION_EXECUTE_HANDLER)
    {           
      D_Message ("::Error at copy model data");
      D_Try { if (model) delete [] (char*)(model); } D_Exc ("");
      delete src;
      return FALSE;
    }

    delete src;

    return model;
  }
  D_ExcRet ("TreePrecompile: Error at LoadModel",NULL);
}

/*void    BuildTreesInfo        (GameLevel& level,memavail_t& vertex,memavail_t& mesh)
{
  TreeCompiler     comp (M_GetPool(),&level);
  char             modelname [256] = "";
  int              count           = comp.treecount ();

  uint             vcount          = 0;
  size_t           msize           = 0;

  uint             vmin            = 1024*1024*128,
                   vmax            = 0;
  size_t           mmin            = 1024*1024*128,
                   mmax            = 0;
    
  for (int i=0;i<count;i++)
  {
    comp.GetTree (i,modelname);

    if (!strlen (modelname))
      continue;

    size_t            size  = 0;
    multimodeldata_t* model = LoadModel (modelname,size);

    vcount                 += model->vertex.num;
    msize                  += size;

    if (size < mmin) mmin = size;
    if (size > mmax) mmax = size;
    if (model->vertex.num < vmin) vmin = model->vertex.num;
    if (model->vertex.num > vmax) vmax = model->vertex.num;

    delete [] (char*)model;
  }

  vertex.mem_min   = vmin * (sizeof (vertex_t) + sizeof (vector3d_t) + sizeof (textcoord_t) + 256);
  vertex.mem_max   = vcount * (sizeof (vertex_t) + sizeof (vector3d_t) + sizeof (textcoord_t) + 256);
  mesh.mem_min     = mmin  + 4096;
  mesh.mem_max     = msize + 4096;
} */

void    BuildTotalInfo          (levelmemavail_t& mem)
{
  mem.total_info = mem.land_cache + mem.land_vertex_buffer +
                   mem.trees_vertex_buffer + mem.trees_data;
}

void    BuildMemoryInfo         (GameLevel& level)
{
  levelmemavail_t       mem;

  FloatPool             pool (M_GetPool(),32*1024*1024);

  M_PushPool            (&pool);

  BuildLandInfo         (level,mem.land_cache);
  BuildVBInfo           (level,mem.land_vertex_buffer);
//  BuildTreesInfo        (level,mem.trees_vertex_buffer,mem.trees_data);
  BuildTotalInfo        (mem);  

  BinFile* file  = level.CreateLump (LL_MEM_INFO,GameLevel::OPEN_RW);

  file->write (&mem,sizeof (mem));  

  delete file;

  M_PopPool             ();
}

BOOL    LoadMemoryInfo          (GameLevel& level,levelmemavail_t& info)
{
  BinFile* file  = level.CreateLump (LL_MEM_INFO,GameLevel::OPEN_RO);  

  if (!IsValid (file))
    return FALSE;

  file->read (&info,sizeof (levelmemavail_t));

  delete file;

  return TRUE;
}