#ifndef __CONVERT_HEIGHT_MAP_RAW_FILE__
#define __CONVERT_HEIGHT_MAP_RAW_FILE__

#include "g_level.h"
#include <binfile.h>

////////////////////////////////////////////////////////////////////////////////
///Конвертер из карты высот
////////////////////////////////////////////////////////////////////////////////
class    RawConvert: public MemObject
{
  public:
                RawConvert  (Pool*        pool,
                             const char*  raw_file_name,
                             const char*  text_file_name,
                             GameLevel&   level,
                             BOOL         include = FALSE
                            );
       virtual  ~RawConvert ();

////////////////////////////////////////////////////////////////////////////////
///Установки
////////////////////////////////////////////////////////////////////////////////
       void     SetSize         (int width,int height);
       void     SetTileSize     (int width,int height);
       void     SetTextTileSize (int width,int height);
       void     SetMaxHeight    (float height);

////////////////////////////////////////////////////////////////////////////////
///Сохранение
////////////////////////////////////////////////////////////////////////////////
       void     Save        (); 

////////////////////////////////////////////////////////////////////////////////
///Конвертация
////////////////////////////////////////////////////////////////////////////////
       void     convert     (); 
       void     prepare     ();

  protected:
       void     SaveInfo    (GameLevel&);
       void     SaveRAW     (GameLevel&);
       void     SaveTexture (GameLevel&);

  private:      
       void     CalcBound   (const int x,const int y,boundbox_t&) const;

  private:
        char            mFileName [128], mTextName [128];
        BinFile*        mFile, *mTextFile;
        hmtile_t*       mInfo;
        uchar*          mMap;
        float           mXKoef, mYKoef, mZKoef;
        int             mWidth, mHeight, mTileWidth, mTileHeight, 
                        mTextTileWidth, mTextTileHeight,
                        mTextWidth,mTextHeight, mTextBPP;
        BOOL            mInclude;
        GameLevel&      mLevel;
        grmapheader_t   mMapHeader;
};

#endif