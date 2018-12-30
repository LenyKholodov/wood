#ifndef __RENDER_TREE_MAP__
#define __RENDER_TREE_MAP__

#include <flagbit.h>
#include <pool.h>
#include "cv_treecomp.h"

class    TreeKeeper;

struct   treemapnode_t
{
  treemapnode_t*   next;
  int              index;
  tree_t           tree;
};

////////////////////////////////////////////////////////////////////////////////
///Карта деревьев
////////////////////////////////////////////////////////////////////////////////
class    TreeMap: public MemObject
{
  public:
                        TreeMap  (Pool*,TreeKeeper*,int _GradShiftKoef = 2);
             virtual    ~TreeMap ();

////////////////////////////////////////////////////////////////////////////////
///Посадка деревьев
////////////////////////////////////////////////////////////////////////////////
        void            recalc   ();

////////////////////////////////////////////////////////////////////////////////
///Работа с тайлами
////////////////////////////////////////////////////////////////////////////////
        treemapnode_t*   get          (int x,int y);
        void             reset        ();

  protected:
        void             LoadTreeMap  (TreeCompiler&);
        void             AddTree      (const tree_t&,const vector3d_t&,const boundbox_t&);
        void             CalcHeight   (vector3d_t&);

  private:
        TreeKeeper*     mKeeper;
        FlagBit         mFlag;
        treemapnode_t** mMap;
        int*            mOffs;
        int             mShift, mWidth, mHeight;

        ClPool<treemapnode_t> mNodePool;
};

#endif