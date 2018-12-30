#ifndef __PAK_STREAM_BIN_FILE__
#define __PAK_STREAM_BIN_FILE__

#include <binfile\streamfile.h>
#include <zip\zlib.h>

struct   gzStream;

const size_t BF_DEFAULT_STREAM_CACHE_SIZE = 16384;

/////////////////////////////////////////////////////////////////////////////////////////////
///Уровни паковки
/////////////////////////////////////////////////////////////////////////////////////////////
enum PackLevels
{
   BF_PACK_NO_COMPRESSION = 0,
   BF_PACK_FAST,
   BF_PACK_NORMAL,
   BF_PACK_GOOD,
   BF_PACK_BEST
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Zip-поток ///внимание! размер (даже при чтении) меняется динамически
/////////////////////////////////////////////////////////////////////////////////////////////
class    PackStream: public StreamBinFile, public MemObject
{
  public:

                PackStream (BinFile*  file,                            
                            int       mode    = -1, //mode=-1 - default to owner mode
                            int       level   = BF_PACK_FAST,
                            Pool*     pool    = Memory::M_GetPool (),
                            size_t    bufsize = BF_DEFAULT_STREAM_CACHE_SIZE,
                            bool      autoDel = false
                           ); 
      virtual   ~PackStream ();

      virtual   bool       valid  () const;

                MemObject::operator new;
                MemObject::operator delete;
                StreamBinFile::reset;

                void                SetAutodel (bool del) { autodel = del; }

  protected:
      virtual   bool       _reset  (); 
      virtual   void       _flush  ();
      virtual   filepos_t  _seek   (filepos_t);
      virtual   size_t     _read   (void*,size_t,filepos_t);
      virtual   size_t     _write  (const void*,size_t,filepos_t);

  private:
                void    destroy   ();
      static    void*   AllocData (void*,uInt,uInt);
      static    void    FreeData  (void*,void*);

  private:
      z_stream  stream;

      int       z_err, z_eof; 
      Byte      *inbuf, *outbuf;
      uLong     crc;   
      long      startpos; 
      size_t    buf_size;

      BinFile*  file;
      bool      autodel;
};

#endif