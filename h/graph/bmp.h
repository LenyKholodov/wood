#ifndef __BMP_LOADER__
#define __BMP_LOADER__

#include <graph\grtypes.h>
#include <binfile.h>
#include <debug.h>

#pragma pack(push,1)

struct   WinBMPFileHeader
{
  WORD          FileType;
  DWORD         FileSize;
  WORD          Reserved [2];
  DWORD         BitmapOffset;
};

struct  WinBMPHeader
{
  DWORD         Size;
  LONG          Width;
  LONG          Height;
  WORD          Planes;
  WORD          BPP;
  DWORD         Compression;
  DWORD         SizeOfBitmap;
  LONG          HorzResolution;
  LONG          VertResolution;
  DWORD         ColorUsed;
  DWORD         ColorsImportant;

  DWORD         RedMask;
  DWORD         GreenMask;
  DWORD         BlueMask;
  DWORD         AlphaMask;
  DWORD         CSType;  
  LONG          RedX;  
  LONG          RedY;  
  LONG          RedZ;  
  LONG          GreenX;  
  LONG          GreenY;
  LONG          GreenZ;    
  LONG          BlueX;
  LONG          BlueY;
  LONG          BlueZ;
  DWORD         GammaRed;
  DWORD         GammaGreen;
  DWORD         GammaBlue;
};

#pragma pack(pop)

bitmap_t*       LoadBMP  (BinFile*,Pool* _TempPool,ostream& _ErrStream = dout);
bitmap_t*       LoadBMP  (BinFile*,Pool* _TempPool,ostream& _ErrStream,void* self_data);

#endif