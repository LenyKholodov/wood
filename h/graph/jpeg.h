#ifndef __JPEG_LOADER__
#define __JPEG_LOADER__

#include <graph\grtypes.h>
#include <binfile.h>
#include <debug.h>

////////////////////////////////////////////////////////////////////////////////
///Поддерживается загрузка только непалитрованых изображений
///Считая палитровые ихображения устаревшими и неудобными
////////////////////////////////////////////////////////////////////////////////

bitmap_t*       LoadJPEG  (BinFile*,Pool* _TempPool,ostream& _ErrStream = dout);
bitmap_t*       LoadJPEG  (BinFile*,Pool* _TempPool,ostream& _ErrStream,void* self_data);

#endif