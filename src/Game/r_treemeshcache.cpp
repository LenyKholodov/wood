#include "pch.h"
#include "r_treemeshcache.h"
#include "r_treerender.h"
#include "r_treeprecomp.h"

/////////////////implementation of class TreeMeshCache//////////////////////////

dcachenode_t*   TreeMeshCache::LoadData     (uint index)
{
  D_Try
  {
    size_t        size  = mPrecompile.GetIndexBufSize (index) + sizeof (multimesh_t);
    dcachenode_t* node  = AllocChunk (size);

    if (!node)    return NULL;

    multimesh_t*  mmesh = (multimesh_t*)node->buf;
    mesh_t*       mesh  = (mesh_t*)((char*)mmesh + sizeof (multimesh_t));
    
    mmesh->num          = mPrecompile.LoadIndexBuffer (index,mmesh->mesh(),size-sizeof (multimesh_t));

    if (mmesh->num <= 0)
    {
      FreeChunk (node);
      return NULL;
    } 

    for (int i=0;i<mmesh->num;i++,mesh=(mesh_t*)((char*)mesh+mesh->size))
      mesh->material += mFirstText;

    return node;
  }
  D_ExcRet ("TreeMeshCache: Error at LoadData",NULL);
}

void            TreeMeshCache::Flush        ()
{
  if (mRender) mRender->Draw ();
}

TreeMeshCache::TreeMeshCache  (Pool* _Pool,TreePrecompile& comp,size_t size)
              : DefragCache (_Pool,size,comp.GetTreesCount (),READ_ONLY|FLUSH_EVENT),
                mPrecompile (comp),
                mRender     (NULL),
                mFirstText  (0)
{   }

TreeMeshCache::~TreeMeshCache ()
{   }

///////////////////////implementation of class TreeVertexCache//////////////////

TreeVertexCache::vcnode_t*  TreeVertexCache::LoadData        (int index)
{
  D_Try
  {
    vcnode_t* node = AllocChunk (mPrecompile.GetVertexCount (index));

    if (!node)
      return NULL;

    vertexbuffer_t vb;

    vb.count       = node->vb->count;
    vb.pos         = 0;
    vb.first       = node->vb->base;
    vb.vertex      = node->vb->vertex;
    vb.normal      = node->vb->normal;
    vb.tcoord      = node->vb->tcoord;

    if (!mPrecompile.LoadVertexBuffer (index,&vb))
    {
      FreeChunk (node);
      return NULL;
    }      

    vb.count       = node->vb->count;
    vb.pos         = 0;
    vb.first       = node->vb->base;

    return node;
  }
  D_ExcRet ("TreeVertexCache: Error at LoadData",NULL);
}

TreeVertexCache::TreeVertexCache (Pool* pool,VertexBuffer* vb,TreePrecompile& comp,int vertexCount)
                : VertexCache (pool,vb,vertexCount,comp.GetTreesCount ()),
                  mPrecompile (comp)
{    }

TreeVertexCache::~TreeVertexCache ()
{   }
