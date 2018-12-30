#ifndef __RENDER_HEIGHT_MAP__
#define __RENDER_HEIGHT_MAP__

#include "g_cachemap.h"
#include "r_matenvx.h"

#include <binfile.h>

class    GameLevel;
class    LandscapeMap;

////////////////////////////////////////////////////////////////////////////////
///Карта кэширования карты высот
////////////////////////////////////////////////////////////////////////////////
class    HeightMap: public CacheMap, public UserMaterialLoader
{
  friend class HMapRender;
  friend class HMapRenderList;
  public:
                                HeightMap  (Pool*,GameLevel&,LandscapeMap*);
          virtual               ~HeightMap ();

////////////////////////////////////////////////////////////////////////////////
///Корректность
////////////////////////////////////////////////////////////////////////////////
       virtual bool valid      () const { return mMap && mInfo && IsValid (mMapFile) && IsValid (mInfoFile); }

////////////////////////////////////////////////////////////////////////////////
///Кэширование карты
////////////////////////////////////////////////////////////////////////////////             
            void                SetPos         (const vector3d_t& p) { CacheMap::SetPos (p.x,p.z); }

////////////////////////////////////////////////////////////////////////////////
///Определение высоты
////////////////////////////////////////////////////////////////////////////////
            float               height        (const vertex_t&);

////////////////////////////////////////////////////////////////////////////////
///Работа с картой
////////////////////////////////////////////////////////////////////////////////
            int                 width         () const { return mWidth;          }    
            int                 height        () const { return mHeight;         }            

  protected:
            hmtile_t&           tile  (int,int);
    inline  int                 index (int x,int y) { return (y-mBox.i.y1)*(mBox.i.x2-mBox.i.x1+1) + x-mBox.i.x1; }
    inline  hmtile_t&           tile  (int index)   { return mInfo [index]; }

    virtual void                reload        (const box_t&);
    virtual void                resize        (uint count);

    virtual bitmap_t*           load (const matloader_t*,Pool*);

  private:
            void                LoadTextureInfo (BinFile*);    

  private:
            uchar*              mMap;
            hmtile_t*           mInfo;
            BinFile*            mMapFile, *mInfoFile, *mTextFile;
            int                 mWidth, mHeight, mTileWidth, mTileHeight;
            int                 mInfoWidth, mInfoHeight;
            float               mKx, mKy, mKz;
            int                 mX1,mY1,mLoadWidth,mLoadHeight;

            int                 mTextTileWidth, mTextTileHeight, mTextBPP,
                                mTextXLen, mTextYLen;
};

#endif
