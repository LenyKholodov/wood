#include "pch.h"
#include "r_hmap.h"
#include "g_level.h"
#include "g_map.h"

#include <graph\tga.h>
#include <misc.h>

bitmap_t*      HeightMap::load (const matloader_t* mat,Pool* pool)
{
  D_Try
  {
    if (!mat || !mTextXLen) return NULL;

    int       x     = mat->index % mTextXLen,
              y     = mat->index / mTextXLen;
    int       width = mTextXLen * mTextTileWidth;
    ulong     offs  = (y * mTextTileHeight * width +
                      x * mTextTileWidth) * mTextBPP + sizeof (TGAHeader);
    ulong     lsize = mTextBPP * mTextTileWidth,
              step  = mTextBPP * width,
              size  = sizeof (bitmap_t) + mTextTileWidth * mTextTileHeight * mTextBPP;
    bitmap_t* bmp   = (bitmap_t*)pool->alloc (size);

    if (!bmp)
      D_NoMemoryRet ("HeightMap",size,NULL);   

    char* data = bmp->data;

    for (int i=0;i<mTextTileHeight;i++,offs+=step,data+=lsize)
    {
      mTextFile->seek (offs);
      mTextFile->read (data,lsize);
    }

    bmp->width  = mTextTileWidth;
    bmp->height = mTextTileHeight;
    bmp->bpp    = mTextBPP << 3;

    if (bmp->bpp >= 24)
    {
      int   size   = bmp->width * bmp->height;
      char* data   = bmp->data;

      for (uint index=0;index<size;index++,data+=mTextBPP)
        swap (*data,*(data+2));
    }

    return bmp;
  }
  D_ExcRet ("HeightMap: Error at load (virtual) texture chunk",NULL);
}

void           HeightMap::resize (uint size)
{
  if (mMap)  free (mMap);
  if (mInfo) free (mInfo);

  mInfo      = (hmtile_t*)alloc (sizeof (hmtile_t) * size);
  mMap       = (uchar*)alloc    (mTileWidth * mTileHeight * size);

  if (!mMap || !mInfo)
  {
    if (!mMap)  D_NoMemory ("HeightMap",size * sizeof (hmtile_t));
    if (!mInfo) D_NoMemory ("LandscapeMap",size * mTileWidth * mTileHeight);
    return;
  }
}

void           HeightMap::reload  (const CacheMap::box_t& box)
{
  dout<<"HeightMap: reload (tile): "<<box.i.x1<<" "<<box.i.y1<<" "<<box.i.x2<<" "<<box.i.y2<<endl;

  mBox             = box;  
  mX1              = box.i.x1 * mTileWidth;
  mY1              = box.i.y1 * mTileHeight;
  mLoadWidth       = (box.i.x2-box.i.x1+1) * mTileWidth;
  mLoadHeight      = (box.i.y2-box.i.y1+1) * mTileHeight;

  hmtile_t* tptr   = mInfo;
  ulong     trcnt  = box.i.x2-box.i.x1+1;
  size_t    trsize = trcnt * sizeof (hmtile_t);
  size_t    tlsize = mInfoWidth * sizeof (hmtile_t);
  ulong     tpos   = sizeof (hmapheader_t) + 
                     box.i.y1 * mInfoWidth * sizeof (hmtile_t) + 
                     sizeof (hmtile_t) * box.i.x1;

  uchar*    mptr   = mMap;
  ulong     mrsize = trcnt * mTileWidth;
  ulong     mpos   = box.i.y1 * mWidth + box.i.x1;
  ulong     bsize  = mLoadWidth*mTileHeight;

  for (int y=box.i.y1;y<=box.i.y2;y++,tptr+=trcnt,tpos+=tlsize)
  {
    mInfoFile->seek (tpos);
    mInfoFile->read (tptr,trsize);

    for (int y1=y;y1<y+mTileHeight;y1++,mptr+=mrsize,mpos+=mWidth)
    {
      mMapFile->seek (mpos);
      mMapFile->read (mptr,mrsize);
    }  

    ulong offs  = (y-box.i.y1)*bsize;

    for (int x=0;x<trcnt;x++,offs+=mTileWidth)
    {
      tptr [x].sOffs = offs;
      tptr [x].eOffs = offs + mTileWidth + bsize - 1 - mLoadWidth;
    }
  }  
}

float          HeightMap::height        (const vertex_t& p)
{
  D_Try
  {
    int    x = p.x * mKx,
           y = p.z * mKz;
    uchar* h = mMap + (y-mY1) * mLoadWidth + (x-mX1);
    return *h;    
  }
  D_ExcRet ("HeightMap: Error at height (const vertex_t&)",0);
}

hmtile_t&      HeightMap::tile  (int x,int y)
{
  #ifdef DEBUG
    if (x < mBox.i.x1 || y < mBox.i.y1 || 
        x > mBox.i.x2 || y > mBox.i.y2)
    {
      static hmtile_t err;

      mInfoFile->seek (sizeof (hmapheader_t) + 
                    y * mInfoWidth * sizeof (hmtile_t) + 
                    sizeof (hmtile_t) * x
                  );
      mInfoFile->read (&err,sizeof (hmtile_t));

      return err;
    }      
  #endif

  return mInfo [(y-mBox.i.y1) * (mBox.i.x2-mBox.i.x1+1) + x - mBox.i.x1];
}

void    HeightMap::LoadTextureInfo (BinFile* file)
{
  TGAHeader header;

  file->read (&header,sizeof (TGAHeader));

  mTextBPP  = header.PixelDepth >> 3;
  mTextXLen = header.Width / mTextTileWidth;
  mTextYLen = header.Height / mTextTileHeight;
}

HeightMap::HeightMap  (Pool* pool,GameLevel& level,LandscapeMap* lmap)
          : CacheMap  (pool),
            mMapFile  (NULL), mInfoFile (NULL), mTextFile (NULL),
            mMap      (NULL), mInfo (NULL),
            mWidth    (0), mHeight (0), mTileWidth (0), mTileHeight (0),
            mInfoWidth(0), mInfoHeight (0), mX1 (0), mY1 (0),
            mLoadWidth (0), mLoadHeight (0),
            mKx       (1), mKy (1), mKz (1)
{
  D_Try
  {
    mInfoFile = level.CreateLump (LL_RAW_INFO,BF_OPEN_RO);

    if (!IsValid (mInfoFile))
    {
      D_Message ("HeightMap: Error at open height map info lump");
      return;
    }

    hmapheader_t hdr;
    
    mInfoFile->read (&hdr,sizeof (hmapheader_t));

    if (!hdr.mname [0]) mMapFile = level.CreateLump (LL_RAW_DATA,BF_OPEN_RO);
    else                mMapFile = F_OpenFile (hdr.mname,BF_OPEN_RO);

    if (!IsValid (mMapFile))
    {
      D_Message ("HeightMap: Error at open height map data lump");
      return;
    }

    mWidth          = hdr.width;
    mHeight         = hdr.height;
    mTileWidth      = hdr.twidth;
    mTileHeight     = hdr.theight;
    mInfoWidth      = mWidth / mTileWidth;
    mInfoHeight     = mHeight / mTileHeight;    
    mTextTileWidth  = hdr.text_twidth;
    mTextTileHeight = hdr.text_theight;
    mKx             = 1.0f / hdr.kx; //умножение для переведения из real->map
    mKy             = 1.0f / hdr.ky;
    mKz             = 1.0f / hdr.kz; 

    Init (float (lmap->width)*lmap->step,float (lmap->height)*lmap->step,
          mInfoWidth,mInfoHeight,
          2.0f * lmap->step
         );

    if (!hdr.tname [0]) mTextFile = level.CreateLump (LL_RAW_TEXT,BF_OPEN_RO);
    else                mTextFile = F_OpenFile (hdr.tname,BF_OPEN_RO);

    if (!IsValid (mTextFile))
    {
      D_Message ("HeightMap: Error at open texture data");
      if (mTextFile) delete mTextFile;
      mTextFile = NULL;
    }
    else  
      LoadTextureInfo (mTextFile);     
  }
  D_Exc ("HeightMap: Error at construct");
}

HeightMap::~HeightMap ()
{
  if (mMap)      free (mMap);
  if (mInfo)     free (mInfo);
  if (mMapFile)  delete mMapFile;
  if (mInfoFile) delete mInfoFile;
  if (mTextFile) delete mTextFile;
}
