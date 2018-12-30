#ifndef __LAND_MESH_CACHE__
#define __LAND_MESH_CACHE__

#include <pool.h>
#include <binfile.h>

#include "g_boxcache.h"

class  LandMeshRender;
class  GameLevel;
class  LevelLoader;

////////////////////////////////////////////////////////////////////////////////
///Кэш земли
////////////////////////////////////////////////////////////////////////////////
class  LandscapeCache: public BoxCache
{
  public:
                        LandscapeCache  (Pool*           _Pool,
                                         LevelLoader&    _Loader,
                                         LandscapeMap*   _Map,
                                         size_t          _CacheSize
                                        );
         virtual        ~LandscapeCache ();

////////////////////////////////////////////////////////////////////////////////
///Кэширование
////////////////////////////////////////////////////////////////////////////////
     void*      lock            (const grtile_t&,int chunk_id,LandMeshRender* = NULL); 
     void*      get             (const grtile_t&,int chunk_id,LandMeshRender* = NULL);

     vertex_t*  vbLock          (int vertex_buffer_index);
     void       vbUnlock        (int vertex_buffer_index);

     virtual bool   valid       () const { return mVBLump && IsValid (mVBFile) &&
                                                  BoxCache::valid ();
                                         }

  protected:
         virtual void   Flush           ();
         virtual void   LoadInfo        (int,int,int,int);
         virtual void   FreeChunk       (dcachenode_t*);

  private:
         static  BinFile*  OpenPack     (GameLevel&);

  private:
        LandMeshRender* mRender;
        LandscapeMap&   mLandMap;
        uint            mMapSize, mMeshNum, mTablesOffs;

        BinFile*        mVBFile;
        ulong           mVBOffs, mVBCount;
        dcachenode_t**  mVBLump;        
};

#endif