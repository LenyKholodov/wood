#ifndef __ALPHA_BLEND_TEXTURE__
#define __ALPHA_BLEND_TEXTURE__

#include <graph\grtypes.h>
#include <binfile.h>
#include <debug.h>

#pragma pack(push,1)

enum ALPFormats
{
  FMT_UNKNOWN = 0,        
  FMT_BMP,
  FMT_JPEG,
  FMT_TGA  
};

struct  alpheader_t
{
  ulong    Id;
  ulong    Offs [2];
  ulong    Size [2];
  int      Format [2];
  rgb_t    FirstCol,
           LastCol;
  uint     KoefTrue, KoefFalse;
};

#pragma pack(pop)

bitmap_t*       LoadALP  (BinFile*,Pool* _TempPool,ostream& _ErrStream = dout);
void            SaveALP  (BinFile* _DestFile,
                          BinFile* _SrcFile,
                          Pool*    _TempPool,
                          const rgb_t&   _FromColor,
                          const rgb_t&   _ToColor,
                          uint     _KoefTrue  = 255,
                          uint     _KoefFalse = 0,
                          int      _Format = FMT_UNKNOWN,
                          ostream& _ErrStream = dout
                         );
void            SaveALP  (BinFile* _DestFile,
                          BinFile* _SrcTexture,
                          BinFile* _AlpTexture,
                          Pool*    _TempPool,
                          int*     _Format,
                          ostream& _ErrStream = dout
                         );
bitmap_t*      ConvALP  (bitmap_t* image,bitmap_t* alp,Pool*,ostream& = dout);

#endif