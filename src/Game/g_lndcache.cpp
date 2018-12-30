#include "pch.h"
#include "g_lndcache.h"
#include "g_level.h"
#include "g_loader.h"
#include "r_landscape.h"

struct vblump_t
{
  int           vbuffer;
  int           first;
  int           count;
  vertex_t      data [];
};

void      LandscapeCache::FreeChunk       (dcachenode_t* node)
{
  boxdesc_t* box = (boxdesc_t*)node->buf;

  if (!box->first)  //vertex buffer
  {
    vblump_t* lump          = (vblump_t*)box->data;
    mVBLump [lump->vbuffer] = NULL;    
  }

  FloatCache::FreeChunk (node);
}

vertex_t* LandscapeCache::vbLock          (int vbIndex)
{
  D_Try
  {
    if (!mVBLump [vbIndex])
    {
      vertexbuffer_t temp;
      ulong          offs = 0;

      mVBFile->seek (mVBOffs + sizeof (ulong) * vbIndex);
      mVBFile->read (&offs,sizeof (ulong));      
      mVBFile->seek (offs);
      mVBFile->read (&temp,sizeof (vertexbuffer_t));

      dcachenode_t*  node = AllocChunk (sizeof (boxdesc_t) + sizeof (vblump_t) + 
                                        temp.count * sizeof (vertex_t)
                                       );
      if (!node)
        return NULL;

      boxdesc_t* box = (boxdesc_t*)node->buf;

      box->x         = box->y = box->width = box->height = 0;
      box->first     = NULL;
      box->info      = NULL;

      vblump_t* lump = (vblump_t*)box->data;

      lump->vbuffer  = vbIndex;
      lump->count    = temp.count;
      lump->first    = temp.first;

      mVBFile->read (lump->data,temp.count * sizeof (vertex_t));

      mVBLump [vbIndex] = node;
    }

    FloatCache::lock (mVBLump [vbIndex]);

    return ((vblump_t*)((boxdesc_t*)mVBLump [vbIndex]->buf)->data)->data;
  }
  D_ExcRet ("LandscapeCache: Error at vbLock",NULL);
}

void      LandscapeCache::vbUnlock        (int vbIndex)
{
  D_Try
  {
    if (mVBLump [vbIndex])
      FloatCache::unlock (mVBLump [vbIndex]);
  }
  D_Exc ("LandscapeCache: Error at vbUnlock");
}

void*     LandscapeCache::lock            (const grtile_t& tile,int chunk_id,LandMeshRender* render)
{
  mRender    = render;
  char* data = (char*)BoxCache::lock (tile.tile.coord.x,tile.tile.coord.y);
  mRender    = NULL;
                
  return data ? data + tile.node.chunk [chunk_id].offs : NULL;
}

void*     LandscapeCache::get             (const grtile_t& tile,int chunk_id,LandMeshRender* render)
{
  mRender    = render;
  char* data = (char*)BoxCache::get (tile.tile.coord.x,tile.tile.coord.y);
  mRender    = NULL;

  return data ? data + tile.node.chunk [chunk_id].offs : NULL;
}

void      LandscapeCache::LoadInfo        (int x,int y,int width,int height)
{
  mXs = x; mYs = y; mLoadWidth = width; mLoadHeight = height;

  ulong       rsize = width * sizeof (tileinfo_t);
  ulong       pos   = mTablesOffs + (y * mLandMap.width + x) * sizeof (tileinfo_t);
  ulong       step  = mLandMap.width * sizeof (tileinfo_t);
  tileinfo_t* ptr   = mInfo; 

  for (int i=0;i<height;i++,pos+=step,ptr+=width)
  {
    file()->seek (pos);
    file()->read (ptr,rsize);
  }
}

void      LandscapeCache::Flush           ()
{
  if (mRender) mRender->Draw ();
  else         reset         ();
}

BinFile*  LandscapeCache::OpenPack     (GameLevel& level)
{
  return level.CreateLump (LL_LAND_MESH,GameLevel::OPEN_RO);
}

LandscapeCache::LandscapeCache  
 ( Pool*           _Pool,
   LevelLoader&    _Loader,
   LandscapeMap*   _Map,
   size_t          _CacheSize
 ) : BoxCache  (_Pool,
                OpenPack (_Loader.level),
                _CacheSize,
                _Map->width,
                _Map->height
               ),
     mRender   (NULL),
     mLandMap  (*_Map),
     mTablesOffs (0),
     mVBFile     (_Loader.level.CreateLump (LL_VERTEX_BUFFER,GameLevel::OPEN_RO)),
     mVBCount   (0), mVBLump (NULL), mVBOffs (0)
{
  D_Try
  {
    GameLevel* level = &_Loader.level;

    grcacheheader_t hdr;

    file ()->read (&hdr,sizeof (hdr));

    mMapSize          = hdr.mapSize;
    mMeshNum          = hdr.meshNum;
    mInfo             = (tileinfo_t*)alloc  (_Map->cacheWidth * _Map->cacheHeight * sizeof (tileinfo_t));

    if (!mInfo)
    {
      D_Message ("LandscapeCache: No enough memory");
      return;
    }

    if (!IsValid (mVBFile))
    {
      D_Message ("LandscapeCache: Error at create lump LL_VERTEX_BUFFER");
      return;
    }

    mTablesOffs = hdr.tablesOffs;

    SetCacheBox   (_Map->cacheWidth,_Map->cacheHeight);

    vbheader_t vbHdr;

    mVBFile->read (&vbHdr,sizeof (vbHdr));

    mVBOffs      = vbHdr.offs;
    mVBCount     = vbHdr.count;
    mVBLump      = (dcachenode_t**)alloc (mVBCount * sizeof (dcachenode_t*));

    memset (mVBLump,0,mVBCount * sizeof (dcachenode_t*));

    dout<<"LandscapeCache: Vertex buffers count: "<<mVBCount<<endl;
  }
  D_Exc ("LandscapeCache: Error at construct");
}

LandscapeCache::~LandscapeCache ()
{  
  D_Try
  {
    if (mInfo)         free (mInfo);
    if (file ())       delete file ();
    if (mVBFile)       delete mVBFile;
    if (mVBLump)       free (mVBLump);
  }
  D_Exc ("LandscapeCache: Error at destruct");
}
