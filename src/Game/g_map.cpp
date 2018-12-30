#include "pch.h"
#include "g_map.h"
#include "g_lndcache.h"
#include "g_loader.h"

#include <misc.h>
#include <graph\utils.h>

//grtile_t&     LandscapeMap::index (int x,int y)
//{
//  return map [y*width+x];
//}

grtile_t&  LandscapeMap::tile  (int x,int y)
{
  #ifdef DEBUG
    if (x < mBox.i.x1 || y < mBox.i.y1 || 
        x > mBox.i.x2 || y > mBox.i.y2)
    {
      static grtile_t err;

      mFile->seek (sizeof (grmapheader_t) + 
                    y * width * sizeof (grtile_t) + 
                    sizeof (grtile_t) * x
                  );
      mFile->read (&err,sizeof (grtile_t));

      return err;
    }      
  #endif

  return map [(y-mBox.i.y1) * (mBox.i.x2-mBox.i.x1+1) + x - mBox.i.x1];
}

void       LandscapeMap::resize (uint size)
{
  if (map) free (map);

  map         = (grtile_t*)alloc (sizeof (grtile_t) * size);

  if (!map)
  {
    D_NoMemory ("LandscapeMap",size * sizeof (grtile_t));
    return;
  }
}

void       LandscapeMap::reload         (const box_t& box)
{
  dout<<"LandscapeMap: reload: "<<box.i.x1<<" "<<box.i.y1<<" "<<box.i.x2<<" "<<box.i.y2<<endl;

  grtile_t* ptr   = map;
  ulong     rcnt  = box.i.x2-box.i.x1+1;
  size_t    rsize = rcnt * sizeof (grtile_t);
  size_t    lsize = width * sizeof (grtile_t);
  ulong     pos   = sizeof (grmapheader_t) + 
                    box.i.y1 * width * sizeof (grtile_t) + 
                    sizeof (grtile_t) * box.i.x1;

  for (int y=box.i.y1;y<=box.i.y2;y++,ptr+=rcnt,pos+=lsize)
  {
    mFile->seek (pos);
    mFile->read (ptr,rsize);
  }  

  mBox = box;
}

uint   LandscapeMap::CalcHeight    (const vertex_t&,heightstage_t&)
{
  return 0;
}

float  LandscapeMap::CalcMaxHeight (const vertex_t& v)
{
  D_Try
  {
    grtile_t&     tile   = this->tile (v.x/step,v.z/step);
    facetpatch_t* patch  = (facetpatch_t*)mCache->lock (tile,GRC_FACET_PATCH);
    float         height = 0;
    int           count  = tile.node.chunk [GRC_FACET_PATCH].count;

    if (!patch) 
      return 0;

    vector3d_t line [2] = {vector3d_t (v.x,0,v.z),vector3d_t (v.x,bbox.max.y,v.z)};
    vertex_t   temp (.0f);

    for (int i=0;i<count;i++,patch=(facetpatch_t*)((char*)patch+patch->size))
    {
      vertex_t* vertex  = mCache->vbLock (patch->vbuffer);
      facet_t*  f       = patch->data;
      int       count   = patch->count;

      if (!vertex) continue;

      for (int j=0;j<count;j++,f++)
      {
        plane_t  plane    = {f->n.x,f->n.y,f->n.z,-(f->n & vertex [f->index [0]])};      
        vertex_t vrtx [3] = {vertex [f->index [0]],vertex [f->index [1]],vertex [f->index [2]]};

        IntersectionPoint  (line,plane,temp);
        
        if (IsInsidePolygon (temp,vrtx,3))
            if (temp.y > height) height = temp.y;
      }
     
      mCache->vbUnlock (patch->vbuffer);
    }

    mCache->unlock (tile.tile.coord.x,tile.tile.coord.y);

    return height;
  }
  D_ExcRet ("LandscapeMap: Error at GetMaxHeight",0);
}

float  LandscapeMap::CalcMinHeight (const vertex_t& v)
{
  D_Try
  {
    grtile_t&     tile   = this->tile (v.x/step,v.z/step);
    facetpatch_t* patch  = (facetpatch_t*)mCache->lock (tile,GRC_FACET_PATCH);
    float         height = tile.box.max.y;
    int           count  = tile.node.chunk [GRC_FACET_PATCH].count;

    if (!patch) return 0;

    vector3d_t line [2] = {vector3d_t (v.x,0,v.z),vector3d_t (v.x,bbox.max.y,v.z)};
    vertex_t   temp (.0f);

    for (int i=0;i<count;i++,patch=(facetpatch_t*)((char*)patch+patch->size))
    {
      vertex_t* vertex  = mCache->vbLock (patch->vbuffer);
      facet_t*  f       = patch->data;
      int       count   = patch->count;

      if (!vertex) continue;

      for (int j=0;j<count;j++,f++)
      {
        plane_t  plane    = {f->n.x,f->n.y,f->n.z,-(f->n & vertex [f->index [0]])};      
        vertex_t vrtx [3] = {vertex [f->index [0]],vertex [f->index [1]],vertex [f->index [2]]};

        IntersectionPoint  (line,plane,temp);
        
        if (IsInsidePolygon (temp,vrtx,3))
          if (temp.y < height) height = temp.y;
      }
     
      mCache->vbUnlock (patch->vbuffer);
    }

    mCache->unlock (tile.tile.coord.x,tile.tile.coord.y);

    return (height != tile.box.max.y) ? height : 0;
  }
  D_ExcRet ("LandscapeMap: Error at GetMinHeight",0);
}

LandscapeMap::LandscapeMap   (Pool* _Pool,LevelLoader& loader,size_t cache_size)
         : CacheMap  (_Pool),
           width     (0),
           height    (0),
           map       (NULL),
           fCount    (0),
           vCount    (0),
           mFile     (NULL),
           mCache    (NULL)
{
  D_Try
  {
    mFile = loader.level.CreateLump (LL_LANDSCAPE,GameLevel::OPEN_RO);

    if (!IsValid (mFile))
    {
      D_Message ("LandscapeMap: Error at load landscape map");
      return;
    }

    grmapheader_t hdr;

    mFile->read (&hdr,sizeof (grmapheader_t));

    width        = hdr.width;
    height       = hdr.height;    
    cacheWidth   = hdr.cacheWidth;
    cacheHeight  = hdr.cacheHeight;
    vCount       = hdr.vCount;

    fCount       = hdr.fCount;

    bbox         = hdr.bbox;
    step         = hdr.step;

    float md     = sqrt (2.0f * (sqr (width) + sqr (height))) * step;
    
    if (RENDER_VIEW_DEPTH > md)           RENDER_VIEW_DEPTH = md;
    if (RENDER_VIEW_DEPTH < step * 2.0f)  RENDER_VIEW_DEPTH = step * 2.0f;

    Init         (float (width)*step,float (height)*step,width,height,2.0f*step);
    SetViewDepth (RENDER_VIEW_DEPTH);

    mCache = ext_new LandscapeCache (GetPool(),loader,this,cache_size);

    if (!IsValid (mCache))
    {
      D_Message ("LandscapeMap: Error at create landscape cache");
      return;
    }
  }
  D_Exc ("LandscapeMap: Error at construct");
}

LandscapeMap::~LandscapeMap  ()
{
  D_Try
  {
    if (mCache)   delete mCache;
    if (map)      free (map);
    if (mFile)    delete mFile;
  }
  D_Exc ("LandscapeMap: Error at destruct");
}
            