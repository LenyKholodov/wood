#include "pch.h"
#include "cv_rawmap.h"
#include <graph\tga.h>

void     RawConvert::SetSize     (int width,int height)
{
  mWidth  = width  ? width  : sqrt (mFile->size ());
  mHeight = height ? height : sqrt (mFile->size ());
  mXKoef  = mMapHeader.bbox.max.x/float (mWidth);
  mYKoef  = mMapHeader.bbox.max.z/float (mHeight);
}

void     RawConvert::SetTileSize (int width,int height)
{
  mTileWidth  = width;
  mTileHeight = height;

  if (mTileWidth  > mWidth  || !mTileWidth)  mTileWidth  = mWidth; 
  if (mTileHeight > mHeight || !mTileHeight) mTileHeight = mHeight; 
}

void     RawConvert::SetTextTileSize (int width,int height)
{
  mTextTileWidth  = width;
  mTextTileHeight = height;
}

void     RawConvert::SetMaxHeight (float height)
{
  uchar  hmax = 0;
  uchar* h    = mMap;
  int    size = mWidth * mHeight;

  for (int i=0;i<size;i++,h++)
    if (*h > hmax)
      hmax = *h;

  mZKoef = height/float (hmax);  
}

void     RawConvert::Save       ()
{
  D_Try
  {
    SaveInfo (mLevel);
  }
  D_Exc ("RawConvert: Error at Save");
}

void     RawConvert::SaveInfo    (GameLevel& level)
{
  hmapheader_t  hdr;
  
  if (!mInclude) 
  {
    strcpy (hdr.mname,mFileName);
    strcpy (hdr.tname,mTextName);
  }  
  else
  {
    SaveRAW     (level);
    SaveTexture (level);

    hdr.mname [0] = 0;
    hdr.tname [0] = 0;
  }

  hdr.width        = mWidth;
  hdr.height       = mHeight;
  hdr.twidth       = mTileWidth;
  hdr.theight      = mTileHeight;
  hdr.text_twidth  = mTextTileWidth;
  hdr.text_theight = mTextTileHeight;
  hdr.kx           = mXKoef;
  hdr.ky           = mZKoef;
  hdr.kz           = mYKoef;
  hdr.infoOffs     = sizeof (hmapheader_t);

  BinFile*      file = level.CreateLump (LL_RAW_INFO,BF_OPEN_RW);

  file->write (&hdr,sizeof (hmapheader_t));
  file->write (mInfo,sizeof (hmtile_t) * mWidth * mHeight / (mTileWidth * mTileHeight));
 
  delete file;
}

void     RawConvert::SaveRAW     (GameLevel& level)
{
  BinFile* file = level.CreateLump (LL_RAW_DATA,BF_OPEN_RW);

  file->write (mMap,mWidth*mHeight);

  delete file;
}

void     RawConvert::SaveTexture (GameLevel& level)
{
  BinFile* file  = level.CreateLump (LL_RAW_TEXT,BF_OPEN_RW);

  if (!IsValid (mTextFile) || !IsValid (file))
  {
    D_Message ("RawConvert: Error at SaveTexture '"<<mTextName<<"'");
    if (file)      delete file;
    return;
  }

  mTextFile->reset ();
  
  const  size_t BUF_SIZE = 1024*1024*16;
  static char   buffer [BUF_SIZE];

  for (size_t size = mTextFile->size ();size;size-=BUF_SIZE)
    file->write (buffer,mTextFile->read (buffer,BUF_SIZE));

  delete file;
}

void     RawConvert::prepare     ()
{
  mMap       = (uchar*)alloc (mWidth * mHeight);
  mInfo      = (hmtile_t*)alloc (sizeof (hmtile_t) * mWidth * mHeight / (mTileWidth*mTileHeight));

  if (!mInfo || !mMap)
  {
    if (!mInfo) D_NoMemory ("RawConvert",sizeof (hmtile_t) * mWidth * mHeight / (mTileWidth*mTileHeight));
    if (!mMap)  D_NoMemory ("RawConvert",mWidth*mHeight);
    return;
  }

  if (IsValid (mTextFile))
  {
    TGAHeader header;

    mTextFile->reset ();
    mTextFile->read  (&header,sizeof (TGAHeader));

    mTextWidth  = header.Width;    
    mTextHeight = header.Height;
    mTextBPP    = header.PixelDepth >> 3;

    if (!mTextTileWidth || mTextTileWidth > mTextWidth) 
      mTextTileWidth = mTextWidth;
    if (!mTextTileHeight || mTextTileHeight > mTextHeight) 
      mTextTileHeight = mTextHeight;
  }
}

void     RawConvert::convert     ()
{
  D_Try
  {
    int lwidth  = mWidth / mTileWidth;
    int twidth  = mTextWidth ? mTextWidth : mWidth,
        theight = mTextHeight ? mTextHeight : mHeight;
    int ttw     = mTextTileWidth ? mTextTileWidth : twidth,
        tth     = mTextTileHeight ? mTextTileHeight : theight;
    int xlen    = twidth  / ttw,
        ylen    = theight / tth;

    for (int y=0,ty=0;y<mHeight;y+=mTileHeight,ty++)
    {
      int index = ty * lwidth;

      for (int x=0;x<mWidth;x+=mTileWidth,index++)
      {      
        mInfo [index].index   = index;
        mInfo [index].texture = y * theight / (mHeight * tth) * xlen + 
                                x * twidth  / (mWidth  * ttw);
        mInfo [index].x       = x;
        mInfo [index].y       = y;
        mInfo [index].sOffs   = y * mWidth + x;
        mInfo [index].eOffs   = (y + mTileHeight) * mWidth + x + mTileWidth;

        CalcBound (x,y,mInfo [index].box);
      }
    }  
  }
  D_Exc ("RawConvert: Error at convert");
}

void     RawConvert::CalcBound   (const int x1,const int y1,boundbox_t& box) const
{
  uchar hmin = mMap [y1*mWidth+x1],
        hmax = hmin;

  for (int y=y1;y<=y1+mTileHeight;y++)
  {
    uchar* h = mMap + y*mWidth + x1;

    for (int x=x1;x<=x1+mTileWidth;x++,h++)
    {
      if (*h < hmin) hmin = *h;
      if (*h > hmax) hmax = *h;
    }
  }

  box.min.x = float (x1) * mXKoef;
  box.min.z = float (y1) * mYKoef;
  box.max.x = float (x1+mTileWidth) * mXKoef;
  box.max.z = float (y1+mTileHeight) * mYKoef;
  box.min.y = float (hmin) * mZKoef;
  box.max.y = float (hmax) * mZKoef;
}

RawConvert::RawConvert  (Pool* pool,const char* name,const char* tname,GameLevel& level,BOOL inc)
           : MemObject (pool),
             mFile     (NULL), mMap (NULL), mInfo (NULL), mTextFile (NULL),
             mWidth    (0), mHeight (0), mTileWidth (0), mTileHeight (0),
             mTextTileWidth (0), mTextTileHeight (0),
             mInclude  (inc), mLevel (level),
             mZKoef    (1), mXKoef (1), mYKoef (1),
             mTextWidth (0), mTextHeight (0), mTextBPP (1)
{
  D_Try
  {
    if (name)  strcpy (mFileName,name); 
    else       mFileName [0] = 0;
    if (tname) strcpy (mTextName,tname);
    else       mTextName [0] = 0;

    BinFile* file  = level.CreateLump (LL_LANDSCAPE,BF_OPEN_RO);

    if (!IsValid (file))
    {
      D_Message ("RawConvert: Error at load landscape map");
      return;
    }

    grmapheader_t hdr;

    file->read (&hdr,sizeof (grmapheader_t));  

    mMapHeader = hdr;

    delete file;

    mFile     = F_OpenFile (mFileName,BF_OPEN_RO);
//    mTextFile = F_OpenFile ("textures",mTextName,BF_OPEN_RO);
    mTextFile = F_OpenFile (mTextName,BF_OPEN_RO);

    if (!IsValid (mFile))
    {  
      D_Message ("RawConvert: Error at open file '"<<mFileName<<"'");
      return;
    }

    if (!IsValid (mTextFile))
      D_Message ("RawConvert: Error at open file (texture) '"<<mTextName<<"'");
  }
  D_Exc ("RawConvert: Error at construct");
}

RawConvert::~RawConvert ()
{
  D_Try
  {
    if (mMap)      free (mMap);
    if (mInfo)     free (mInfo);
    if (mFile)     delete mFile;
    if (mTextFile) delete mTextFile;
  }
  D_Exc ("RawConvert: Error at destruct");
}
