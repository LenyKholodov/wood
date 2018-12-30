#include "pch.h"
#include "r_treekeeper.h"
#include "r_treemap.h"
#include "r_treeprecomp.h"
#include "r_treemeshcache.h"
#include "g_loader.h"

const boundbox_t& TreeKeeper::GetTreeBound (TREE tree)
{
  return mTreePrecompile.GetBound (tree);
}

TreeKeeper::TreeKeeper  (Pool* _Pool,LevelLoader& loader,size_t cacheSize,uint vcount)
           : MemObject (_Pool),
             mLandMap  (loader->mLandMap),
             mLandCache(loader->mLandMap->cache ()),
             mTreeComp (_Pool,&loader.level),
             mTreePrecompile (_Pool,mTreeComp),
             mTreeMap  (_Pool,this,RENDER_TREE_MAP_SHIFT),
             mTreeVertexCache (_Pool,loader->mVertexBuffer,mTreePrecompile,vcount),
             mTreeMeshCache   (_Pool,mTreePrecompile,cacheSize)
{
  if (!IsValid (&mTreeComp) || !IsValid (&mTreeMap) || !IsValid (&mTreePrecompile) ||
      !IsValid (&mTreeVertexCache) || !IsValid (&mTreeMeshCache)
     )
  {
    D_Message ("TreeKeeper: No enough memory for sys data");
    return;
  }

  mTreeMap.recalc           ();
}

TreeKeeper::~TreeKeeper ()
{   }