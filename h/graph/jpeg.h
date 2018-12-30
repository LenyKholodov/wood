#ifndef __JPEG_LOADER__
#define __JPEG_LOADER__

#include <graph\grtypes.h>
#include <binfile.h>
#include <debug.h>

////////////////////////////////////////////////////////////////////////////////
///�����ন������ ����㧪� ⮫쪮 �������஢���� ����ࠦ����
///���� �����஢� �审ࠦ���� ���ॢ訬� � ��㤮��묨
////////////////////////////////////////////////////////////////////////////////

bitmap_t*       LoadJPEG  (BinFile*,Pool* _TempPool,ostream& _ErrStream = dout);
bitmap_t*       LoadJPEG  (BinFile*,Pool* _TempPool,ostream& _ErrStream,void* self_data);

#endif