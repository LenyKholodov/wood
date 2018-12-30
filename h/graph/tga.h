#ifndef __TGA_LOADER__
#define __TGA_LOADER__

#include <graph\grtypes.h>
#include <binfile.h>
#include <debug.h>

#pragma pack(push,1)

struct  TGAHeader
{
  BYTE  IDLength;
  BYTE  ColorMapType;
  BYTE  ImageType;
  WORD  CMapStart;
  WORD  CMapLength;
  BYTE  CMapDepth;
  WORD  XOffset;
  WORD  YOffset;
  WORD  Width;
  WORD  Height;
  BYTE  PixelDepth;
  BYTE  ImageDescriptor;
};

#pragma pack(pop)

bitmap_t*       LoadTGA  (BinFile*,Pool* _TempPool,ostream& _ErrStream = dout);
bitmap_t*       LoadTGA  (BinFile*,Pool* _TempPool,ostream& _ErrStream,void* self_data);

#endif