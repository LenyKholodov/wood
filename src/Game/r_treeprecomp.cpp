#include "pch.h"
#include "cv_treecomp.h"
#include "cv_utils.h"
#include "r_mdlmulti.h"
#include "r_treeprecomp.h"

#include <string.h>
#include <gl\gl.h>
#include <graph\striper.h>

struct   treedesc_t
{
  bound_t       bound;          //Ограничивающие тела                 
  ulong         offs;           //Смещение относительно начала файла
  size_t        size;           //Размер всех данных

  uint          vcount;         //Количество вершин

  struct mesh_t
  {
    uint        offs;       //Смещение до мэшей
    size_t      size;       //Размер
    uint        count;      //Количество мэшей
  }mesh;
};

struct   treechunk_t
{
  bound_t       bound;          //Ограничивающие тела                 
  vertex_t*     vertex;         //Указатель на грани
  vector3d_t*   normal;         //Указатель на нормали
  textcoord_t*  tcoord;         //Указатель на текстурные координаты
  mesh_t*       mesh;           //Указатель на меши

  size_t        meshSize;       //Общий размер мэшей
  int           mcount,         //Количество материалов (мэшей)
                vcount;         //Количество вершин
};

const  boundbox_t& TreePrecompile::GetBound  (TREE tree)
{
  D_Try
  {
    return mTreeDesc [tree].bound.box;    
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    D_Message ("TreePrecompile: Error at GetTreeBound");
    static boundbox_t error;
    memset (&error,0,sizeof (error));     
    return error;
  }
}

BOOL     TreePrecompile::BuildTreePrecompile ()
{
  D_Try 
  {
    M_PushPool    (GetPool());

    D_Try
    {
      PrecompModels (mComp);
    }                     
    D_Exc ("TreePrecompile: Error at convert");  

    M_PopPool     ();

    return TRUE;
  }
  D_ExcRet ("TreePrecompile: Error at BuildTreePrecompile",FALSE);
}

BOOL     TreePrecompile::PrecompModels (TreeCompiler& comp)
{
  char           modelname [256] = "";
  int            count           = comp.treecount ();

  if (!mTreeDesc)
    return FALSE;

  for (int i=0;i<count;i++)
  {
    comp.GetTree (i,modelname);

    memset (mTreeDesc + i,0,sizeof (treedesc_t));

    if (!strlen (modelname))
      continue;
    else
    {
      size_t       size   = 0;
      treechunk_t  tree;

      if (!AddModel (modelname,tree,i))
      {
        D_Message ("TreePrecompile: Error at AddModel");
        continue;
      }

      mTreeDesc [i].size       = tree.meshSize + tree.vcount * (sizeof (vertex_t) + sizeof (vector3d_t) + sizeof (textcoord_t));
      mTreeDesc [i].offs       = mFile->getpos ();

      mTreeDesc [i].vcount     = tree.vcount;
      mTreeDesc [i].bound      = tree.bound;

      mFile->write (tree.vertex,sizeof (vertex_t) * tree.vcount);
      mFile->write (tree.normal,sizeof (vector3d_t) * tree.vcount);
      mFile->write (tree.tcoord,sizeof (textcoord_t) * tree.vcount);

      mTreeDesc [i].mesh.offs  = mFile->getpos ();
      mTreeDesc [i].mesh.size  = tree.meshSize;
      mTreeDesc [i].mesh.count = tree.mcount;

      mFile->write (tree.mesh,tree.meshSize);

      FreeModel (tree);
    }
  }    

  delete [] mTreeDesc;

  return TRUE;
}

void  TreePrecompile::CalcBound (multimodeldata_t* model,bound_t& bound)
{
  D_Try
  {
    vertex_t*    mVertex = (vertex_t*)(model->data + model->vertex.offs);
    int          mVCount = model->vertex.num;

    vector3d_t c (.0f);
    vector3d_t dim (.0f);

    for (int i=0;i<mVCount;i++)
      c += mVertex [i];

    c /= float (mVCount);

    for (i=0;i<mVCount;i++)
    {
      vector3d_t d = abs (c - mVertex [i]);

      if (d.x > dim.x) dim.x = d.x;
      if (d.y > dim.y) dim.y = d.y;
      if (d.z > dim.z) dim.z = d.z;
    }    

    bound.box.min        = c - dim;
    bound.box.max        = c + dim;
    bound.sphere.center  = c;
    bound.sphere.r       = length (dim);
  }
  D_Exc ("TreePrecompile: Error at CalcBound");
}

BOOL  TreePrecompile::AddModel      (const char* modelname,treechunk_t& tree,int treeIndex)
{
  multimodeldata_t* model = LoadModel (modelname);

  if (!model)
  {
    D_Message ("TreePrecompile: Model not load '"<<modelname<<"'");
    return FALSE;
  }  

    //calc bound 

  CalcBound (model,tree.bound);

    //build material tree and recalc material indexes    

  static int matReindex [4096];

  for (int i=0;i<4096;i++)
    matReindex [i] = -1;

  char* str = model->data + model->matname.offs;

  for (i=0;i<model->matname.num;i++,str+=strlen(str)+1)
    matReindex [i] = mMatDict.AddMaterial (str);

    //reindex vertex buffers 

  facet_t*     mFacet  = (facet_t*)(model->data + model->facet.offs);

  tree.vertex = (vertex_t*)(model->data + model->vertex.offs);
  tree.normal = (vector3d_t*)(model->data + model->normal.offs);
  tree.tcoord = NULL;
  tree.vcount = ConvertArrays (model->vertex.num,
                               &tree.vertex,
                               &tree.normal,
                               &tree.tcoord,
                               mFacet,
                               model->facet.num,
                               M_GetPool(),
                               FALSE
                              );

  if (!tree.tcoord || !tree.vertex || !tree.normal)
  {
    D_Message ("TreePrecompile: No enough memory for temp data");
    return FALSE;
  }  

         //compute strip/triangle meshes

  static mesh_t* meshArray [4096];

  memset (meshArray,0,sizeof (mesh_t*) * 4096);

  int*    offs     = (int*)(model->data + model->mesh.offs);
  size_t  meshSize = 0;
  int     meshNum  = 0;

  for (i=0;i<model->mesh.num;i++)
  {
    RingTmpl<int> iring (M_GetPool());
    mesh_t*       mesh  = (mesh_t*)(model->data + offs [i]);
    int*          index = (int*)mesh->data;
    int           mtr   = mesh->material;

    for (int j=0;j<mesh->count;j++,index++)
      iring.insert (*index);

    mesh                = CreateMesh (iring,mFacet,16,TRUE,TRUE,M_GetPool(),M_GetPool()); 

    if (!mesh)
    {
 //     D_Message ("TreePrecompile: Error at create mesh");
      meshArray [i] = NULL;
      continue;
    }

    meshNum++;
    mesh->material      = matReindex [mtr];
    mesh->vbuffer       = treeIndex;

    meshArray [i]       = mesh;
    meshSize           += mesh->size;
  }

  tree.mesh     = (mesh_t*)(new char [meshSize]);
  tree.meshSize = meshSize;
  tree.mcount   = meshNum;

  mesh_t* mesh  = tree.mesh;

  if (!tree.mesh)
  {
    D_Message ("TreePrecompile: No enough memory for temp data");
    return FALSE;  
  }  

  for (i=0;i<model->mesh.num;i++)
    if (meshArray [i])
    {
      memcpy (mesh,meshArray [i],meshArray [i]->size);
      M_GetPool()->free (meshArray [i]);
      mesh=(mesh_t*)((char*)mesh+mesh->size);
    }

  delete [] (char*)(model);

  return TRUE;
}

void    TreePrecompile::FreeModel (treechunk_t& tree)
{
  if (tree.mesh)   delete [] (char*)(tree.mesh); 

  if (tree.vertex) M_GetPool()->free (tree.vertex);
  if (tree.normal) M_GetPool()->free (tree.normal);
  if (tree.tcoord) M_GetPool()->free (tree.tcoord);

  tree.vertex = NULL;
  tree.normal = NULL;
  tree.tcoord = NULL;
  tree.mesh   = NULL;

  tree.meshSize = 0;
  tree.mcount   = tree.vcount = 0;  
}

multimodeldata_t*  TreePrecompile::LoadModel     (const char* modelname)
{
  D_Try
  {
    m3dheader_t         hdr;
    multimodeldata_t*   model = NULL;

    BinFile* src = F_OpenFile (modelname,BF_OPEN_RO);

    if (!IsValid (src))
      return NULL;

    src->read (&hdr,sizeof (hdr));

    if (hdr.id != MULTI_MODEL_ID)
    {
      D_Message ("TreePrecompile: Wrong model format '"<<modelname<<"'");
      delete src;
      return FALSE;
    }

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
      D_Message ("TreePrecompile:: Error at copy model data");
      D_Try { if (model) delete [] (char*)(model); } D_Exc ("");
      delete src;
      return FALSE;
    }

    delete src;

    return model;
  }
  D_ExcRet ("TreePrecompile: Error at LoadModel",NULL);
}

BOOL     TreePrecompile::LoadVertexBuffer        (TREE tree,vertexbuffer_t* vb)
{
  D_Try
  {
    vb->count = mTreeDesc [tree].vcount;
    vb->pos   = vb->count;
    vb->first = 0;

    mFile->seek  (mTreeDesc [tree].offs);    
    mFile->read  (vb->vertex,sizeof (vertex_t) * vb->count);
    mFile->read  (vb->normal,sizeof (vector3d_t) * vb->count);
    mFile->read  (vb->tcoord,sizeof (textcoord_t) * vb->count);    

    return TRUE;
  }
  D_ExcRet ("TreePrecompile: Error at LoadVertexBuffer",FALSE);
}

int      TreePrecompile::LoadIndexBuffer         (TREE tree,mesh_t* mesh,size_t size)
{
  D_Try
  {
    size      = size >= mTreeDesc [tree].mesh.size ? mTreeDesc [tree].mesh.size :
                size;

    mFile->seek  (mTreeDesc [tree].mesh.offs);
    mFile->read  (mesh,size);

    return mTreeDesc [tree].mesh.count;
  }
  D_ExcRet ("TreePrecompile: Error at LoadIndexBuffer",0);
}

size_t   TreePrecompile::GetIndexBufSize         (TREE tree)
{
  return mTreeDesc [tree].mesh.size;
}

uint     TreePrecompile::GetVertexCount          (TREE tree)
{
  return mTreeDesc [tree].vcount;
}

TreePrecompile::TreePrecompile  (Pool* _Pool,TreeCompiler& comp,const char* name)
               : MemObject (_Pool),
                 mMatDict  (_Pool,RENDER_TREE_MAX_MATERIALS_COUNT),
                 mComp     (comp),
                 mFile     (NULL),
                 mTreeDesc (NULL)
{
  D_Try
  {
    mFile     = F_OpenFile (name,BF_OPEN_RW);

    mTreeDesc = (treedesc_t*)alloc (sizeof (treedesc_t) * comp.treecount ());

    if (!mTreeDesc)
    {
      D_Message ("TreePrecompile: No enough memory for sys data");      
      return;
    }

    BuildTreePrecompile ();
  }
  D_Exc ("TreePrecompile: Error at construct");
}

TreePrecompile::~TreePrecompile ()
{
  D_Try
  {
    if (mFile)          delete mFile;
    if (mTreeDesc)      GetPool()->free (mTreeDesc);
  }
  D_Exc ("TreePrecompile: Error at destruct")
}
