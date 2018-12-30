#ifndef __RENDER_TREE_KEEPER__
#define __RENDER_TREE_KEEPER__

#include <pool.h>

#include "cv_treecomp.h"
#include "r_treemap.h"
#include "r_treeprecomp.h"
#include "r_treemeshcache.h"

class    LandscapeMap;
class    LandscapeCache;
class    LevelLoader;

////////////////////////////////////////////////////////////////////////////////
///Класс отвечающий за работу деревьями
////////////////////////////////////////////////////////////////////////////////
class    TreeKeeper: public MemObject
{
  friend class TreeMap;
  friend class TreeRender;
  friend class TreeEnv;
  public:
                        TreeKeeper  (Pool*,
                                     LevelLoader&,
                                     size_t _MeshCacheSize,
                                     uint   _VertexCacheSize
                                    );
          virtual       ~TreeKeeper ();          

  protected:
          const boundbox_t& GetTreeBound    (TREE);

  private:        
          LandscapeMap*         mLandMap;
          LandscapeCache*       mLandCache;
          TreeCompiler          mTreeComp;
          TreeMap               mTreeMap;
          TreePrecompile        mTreePrecompile;
          TreeVertexCache       mTreeVertexCache;
          TreeMeshCache         mTreeMeshCache;
};

#endif