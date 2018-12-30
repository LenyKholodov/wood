#include "pch.h"
#include "r_mdlmap.h"
#include "r_mdlenv.h"

#include <string.h>

BOOL            ModelMap::InsertModel  (model_t* mdl)
{
  D_Try
  {
    if (mdl->pos.node)
      ExtractModel (mdl);

    int width  = mModelEnv->mLandMap->width,
        height = mModelEnv->mLandMap->height;
    int x1     = int (mdl->pos.box.min.x / mModelEnv->mLandMap->step),
        y1     = int (mdl->pos.box.min.z / mModelEnv->mLandMap->step),
        x2     = int (mdl->pos.box.max.x / mModelEnv->mLandMap->step),
        y2     = int (mdl->pos.box.max.z / mModelEnv->mLandMap->step);

    if (x1 >= width || y1 >= height || x2 < 0 || y2 < 0) return TRUE;    
    if (x1 < 0)       x1 = 0;
    if (y1 < 0)       y1 = 0;
    if (x2 >= width)  x2 = width - 1;
    if (y2 >= height) y2 = height - 1;

    int xt1   = x1 >> mShift,
        xt2   = x2 >> mShift,
        yt1   = y1 >> mShift,
        yt2   = y2 >> mShift;    
   
    mdl->pos.tile = (mdl->pos.pos.z / mModelEnv->mLandMap->step) * width +
                    (mdl->pos.pos.x / mModelEnv->mLandMap->step);

    if (xt1 == xt2 && yt1 == yt2)
    {
      int           index = mOffs [y1] + xt1;
      mdlmapnode_t* node  = mNodePool.alloc ();

      if (!node)
        return FALSE;

      node->prev         = NULL;
      node->nextTile     = NULL;
      node->next         = mMap [index];
      node->index        = index;
      node->model        = mdl;
      
      if (mMap [index])   mMap [index]->prev = node;

      mMap [index]       = node;
      mdl->pos.node      = node;
    }
    else
    {
      int index = mOffs [y1] + xt1;
      int step  = mWidth - xt2 + xt1 - 1;
          
      for (int y = yt1;y<=yt2;y++,index += step)
        for (int x = xt1;x<=xt2;x++,index++)
        {
          mdlmapnode_t* node = mNodePool.alloc ();

          if (!node)
          {
            ExtractModel (mdl);
            return FALSE;
          }  

          node->prev         = NULL;
          node->nextTile     = NULL;
          node->next         = mMap [index];
          node->index        = index;
          node->model        = mdl;

          if (mMap [index]) mMap [index]->prev = node;

          mMap [index]       = node;

          if (mdl->pos.node)
          {
            node->nextTile   = mdl->pos.node;
            mdl->pos.node    = node;            
          }
          else
            mdl->pos.node = node;
        }  
    }            

    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelMap: Error at InsertModel "<<endl;
    #endif
    return FALSE;
  }
}

void            ModelMap::ExtractModel (model_t* mdl)
{
  D_Try
  {
    if (mdl->pos.node)
    {
      mdlmapnode_t* next = mdl->pos.node->nextTile;
      for (mdlmapnode_t* node = mdl->pos.node;node;node=next,next=next?next->nextTile:NULL)
      {
        if (node->prev) node->prev->next   = node->next;
        else            mMap [node->index] = node->next;
        if (node->next) node->next->prev   = node->prev;
 
        mNodePool.free (node);
      }

      mdl->pos.node = NULL;      
    }  
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelMap: Error at ExtractModel "<<endl;
    #endif    
  }
}

mdlmapnode_t*   ModelMap::get          (int x,int y)
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
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelMap: Error at get () ";
      dout<<this<<endl;
    #endif
    return NULL;
  }
}

void            ModelMap::reset ()
{
  mFlag.reset ();
}

ModelMap::ModelMap  (Pool* _Pool,ModelEnv* _ModelEnv,int _GradShiftKoef)
         : MemObject (_Pool),
           mFlag     (_Pool,((_ModelEnv->mLandMap->width >> _GradShiftKoef) + 1) * 
                            ((_ModelEnv->mLandMap->height >> _GradShiftKoef) + 1)),
           mNodePool (_Pool,((_ModelEnv->mLandMap->width >> _GradShiftKoef) + 1) * 
                            ((_ModelEnv->mLandMap->height >> _GradShiftKoef) + 1)),
           mModelEnv (_ModelEnv),
           mShift    (_GradShiftKoef),
           mMap      (NULL),
           mOffs     (NULL),
           mWidth    ((_ModelEnv->mLandMap->width >> _GradShiftKoef) + 1),
           mHeight   ((_ModelEnv->mLandMap->height >> _GradShiftKoef) + 1)
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

    mMap     = (mdlmapnode_t**)GetPool()->alloc (sizeof (mdlmapnode_t*) * size);
    mOffs    = (int*)GetPool()->alloc (sizeof (int) * mModelEnv->mLandMap->height);

    if (!mMap || !mOffs)
    {
      #ifdef DEBUG
        dout<<"ModelMap: No enough memory for sys data "<<endl;
      #endif
      return;
    }

    memset (mMap,0,sizeof (mdlmapnode_t*) * size);

    for (int y=0;y<mModelEnv->mLandMap->height;y++)
      mOffs [y] = (y >> mShift) * mWidth;
  }     
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelMap: Error at construct "<<endl;
    #endif
  }
}

ModelMap::~ModelMap ()
{
  D_Try
  {
    if (mOffs)          GetPool()->free (mOffs);
    if (mMap)           GetPool()->free (mMap);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelMap: Error at destruct "<<endl;
    #endif
  }
}
