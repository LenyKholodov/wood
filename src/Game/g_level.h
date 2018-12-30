#ifndef __GAME_LEVEL__
#define __GAME_LEVEL__

#include "g_map.h"
#include "r_landscape.h"

////////////////////////////////////////////////////////////////////////////////
///GameLevel constants
////////////////////////////////////////////////////////////////////////////////

const  int       WOOD_HEADER_ID               = *((int*)"WOOD");
const  int       END_TAG                      = *((int*)"END!");
const  uint      LL_NUM                       = 32;

#define  NEW_LUMP_ID(NAME,POS) const uint NAME ## _BIT = 1l<<(POS); \
                               const uint NAME = POS;

NEW_LUMP_ID (LL_LANDSCAPE,     0);
NEW_LUMP_ID (LL_TEXTURES,      1);
NEW_LUMP_ID (LL_LAND_MESH,     2);
NEW_LUMP_ID (LL_VERTEX_BUFFER, 3);
NEW_LUMP_ID (LL_SKY_BOX,       4);
NEW_LUMP_ID (LL_TREE_MAP,      5);
NEW_LUMP_ID (LL_LAND_MAT,      6);
NEW_LUMP_ID (LL_MEM_INFO,      7);
NEW_LUMP_ID (LL_RAW_DATA,      8);
NEW_LUMP_ID (LL_RAW_INFO,      9);
NEW_LUMP_ID (LL_RAW_TEXT,      10);

////////////////////////////////////////////////////////////////////////////////
///Загрузчик уровня
////////////////////////////////////////////////////////////////////////////////
class   GameLevel: public MemObject
{  
  friend class LumpFile;
  public:
                GameLevel  (Pool*       _Pool,
                            const char* _FileName
                           ); //for read
                GameLevel  (Pool*,BinFile*); //for write
      virtual   ~GameLevel ();    

      virtual   bool valid () const { return IsValid (mFile); }

                enum {
                  OPEN_RO = BF_OPEN_RO,
                  OPEN_RW = BF_OPEN_RW
                };

////////////////////////////////////////////////////////////////////////////////
///Запись в GameLevel 
////////////////////////////////////////////////////////////////////////////////
      BinFile*  CreateLump      (uint _LoaderId,int mode,size_t _LumpSize = 0);
      void      DeleteLump      (BinFile* file) { if (file) delete file; }

      struct lumpx_t
      {
        size_t     size;
        ulong      pos;
      };       

      const 
      lumpx_t&   GetLump         (uint _LoaderId) const;

////////////////////////////////////////////////////////////////////////////////
///Сохранение уровня (рекомендуется только для утилит построения уровня)
////////////////////////////////////////////////////////////////////////////////
      BOOL      Save            (const char* _LevelName = NULL) const;

  protected:
      BinFile*          mFile;
      lumpx_t           mLump     [LL_NUM];
      BinFile*          mLumpFile [LL_NUM];
      BOOL              mWait, mWrite, mChange;
      long              mFilePos;      
};

#endif