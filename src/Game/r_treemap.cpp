#include "pch.h"
#include "r_treemap.h"
#include "r_treekeeper.h"
#include "g_map.h"
#include "g_lndcache.h"

#include <graph\utils.h>

void   TreeMap::CalcHeight   (vector3d_t& pos)
{
  D_Try
  {
    pos.y = mKeeper->mLandMap->CalcMinHeight (pos);
  }
  D_Exc ("TreeMap: Error at CalcHeight");
}

void   TreeMap::AddTree      (const tree_t& _tree,const vector3d_t& _pos,const boundbox_t& _box)
{
  boundbox_t box  = _box;  
  vector3d_t pos  = _pos;
  tree_t     tree = _tree;

  CalcHeight (pos);

  pos.y          += (box.max.y - box.min.y) / 2.0f;

  tree.pos        = pos;

  box.min        += pos;
  box.max        += pos;

  int width  = mKeeper->mLandMap->width,
      height = mKeeper->mLandMap->height;
  int x1     = int (box.min.x / mKeeper->mLandMap->step),
      y1     = int (box.min.z / mKeeper->mLandMap->step),
      x2     = int (box.max.x / mKeeper->mLandMap->step),
      y2     = int (box.max.z / mKeeper->mLandMap->step);

  if (x1 >= width || y1 >= height || x2 < 0 || y2 < 0) return ;
  if (x1 < 0)       x1 = 0;
  if (y1 < 0)       y1 = 0;
  if (x2 >= width)  x2 = width - 1;
  if (y2 >= height) y2 = height - 1;

  int xt1   = x1 >> mShift,
      xt2   = x2 >> mShift,
      yt1   = y1 >> mShift,
      yt2   = y2 >> mShift;    

  if (xt1 == xt2 && yt1 == yt2)
  {
    int            index = mOffs [y1] + xt1;
    treemapnode_t* node  = mNodePool.alloc ();

    if (node)
    {
      node->next         = mMap [index];
      node->index        = index;
      node->tree         = tree;
      mMap [index]       = node;   
    }  
  }
  else
  {
    int index = mOffs [y1] + xt1;
    int step  = mWidth - xt2 + xt1 - 1;
        
    for (int y = yt1;y<=yt2;y++,index += step)
      for (int x = xt1;x<=xt2;x++,index++)
      {
        treemapnode_t* node = mNodePool.alloc ();

        if (node)
        {
          node->next         = mMap [index];
          node->index        = index;
          node->tree         = tree;
          mMap [index]       = node;
        }  
      }  
  }
}

void             TreeMap::LoadTreeMap  (TreeCompiler& comp)
{
  D_Try
  {
    RingTmpl<treegroup_t>  ring (pool());

    comp.GetMap (ring);

    RingTmpl<treegroup_t>::iterator iter  = ring;
    int                             count = ring.count ();

    for (int i=0;i<count;i++,iter++)
    {
      treegroup_t group = iter.data ();

      RingTmpl<tree_t> ring1 (pool());

      if (!comp.GetTreeGroup (group.group,ring1))
        continue;

      RingTmpl<tree_t>::iterator iter1  = ring1;
      int                        count1 = ring1.count ();

      for (int j=0;j<count1;j++,iter1++)
      {
        tree_t tree = iter1.data ();
        AddTree (tree,tree.pos+group.pos,mKeeper->GetTreeBound (tree.tree));
      }
    }
  }
  D_Exc ("TreeMap: Error at LoadTreeMap");
}

treemapnode_t*   TreeMap::get          (int x,int y)
{
  D_Try
  {
    int index = mOffs [y] + (x >> mShift);

    if (mFlag [index]) return NULL;
    else
    {
      mFlag.set (index);
      return mMap [index];
    }  
  }
  D_ExcRet ("TreeMap: Error at get",NULL);
}

void             TreeMap::reset        ()
{
  mFlag.reset ();
}

void   TreeMap::recalc   ()
{
  LoadTreeMap (mKeeper->mTreeComp);
}

TreeMap::TreeMap  (Pool* _Pool,TreeKeeper* loader,int _GradShiftKoef)
        : MemObject (_Pool),
          mMap      (NULL), mOffs (NULL),
          mShift    (_GradShiftKoef),
          mWidth    ((loader->mLandMap->width >> _GradShiftKoef) + 1),
          mHeight   ((loader->mLandMap->height >> _GradShiftKoef) + 1),
          mFlag     (_Pool,((loader->mLandMap->width >> _GradShiftKoef) + 1) * 
                            ((loader->mLandMap->height >> _GradShiftKoef) + 1)),
          mNodePool (_Pool,1024),
          mKeeper (loader)                            
{
  D_Try
  {
    if (!IsValid (&mFlag) || !IsValid (&mNodePool))
    {
      #ifdef DEBUG
        dout<<"ModelMap: No enough memory for mFlags "<<endl;
      #endif
      return;
    }

    int size = mWidth * mHeight;

    mMap     = (treemapnode_t**)pool()->alloc (sizeof (treemapnode_t*) * size);
    mOffs    = (int*)pool()->alloc (sizeof (int) * loader->mLandMap->height);

    if (!mMap || !mOffs)
    {
      D_Message ("ModelMap: No enough memory for sys data ");
      return;
    }

    memset (mMap,0,sizeof (treemapnode_t*) * size);

    for (int y=0;y<loader->mLandMap->height;y++)
      mOffs [y] = (y >> mShift) * mWidth;
  }
  D_Exc ("TreeMap: Error at construct");
}

TreeMap::~TreeMap ()
{
  D_Try
  {
    if (mMap)      pool()->free (mMap);
    if (mOffs)     pool()->free (mOffs);
  }
  D_Exc ("TreeMap: Error at destruct");
}
