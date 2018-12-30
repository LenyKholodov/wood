#include "zlib\zlib.h"
#include <iostream.h>

#pragma comment(lib,"zlib.lib")

z_stream stream;
bool     init = false;

int _compress2 (z_stream& stream,Bytef* dest,uLongf* destLen,const Bytef* source,uLong sourceLen,int level)
{
  int err;

  stream.next_in = (Bytef*)source;
  stream.avail_in = (uInt)sourceLen;
  stream.next_out = dest;
  stream.avail_out = (uInt)*destLen;
  if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;

  stream.zalloc = (alloc_func)0;
  stream.zfree = (free_func)0;
  stream.opaque = (voidpf)0;

  if (!init)
  {
    init = true;
    err = deflateInit(&stream, level);    
    if (err != Z_OK) return err;
  }

//  err = deflate(&stream, Z_FINISH);
  err = deflate(&stream, Z_NO_FLUSH);
//  if (err != Z_STREAM_END) {
//      deflateEnd(&stream);
//      return err == Z_OK ? Z_BUF_ERROR : err;
//  }
  *destLen = stream.total_out;

//  err = deflateEnd(&stream);
  return err;
}

void    main ()
{
  int err;
//  if (err != Z_OK) 
//  {
//    cout<<err<<endl;
//    return;
//  }  

  static unsigned char src [1024*64];
  static unsigned char dst [1024*64*1024];
  unsigned long    dstSize = 64*1024*1024;

  cout<<"compress in"<<endl;

//  err = compress2 (dst,&dstSize,src,64*1024,Z_NO_COMPRESSION);
  cout<<_compress2 (stream,dst,&dstSize,src,64*1024,Z_NO_COMPRESSION)<<endl;
  cout<<_compress2 (stream,dst,&dstSize,src,64*1024,Z_NO_COMPRESSION)<<endl;

  cout<<"compress out"<<endl;

  err = deflateEnd(&stream);  

  cout<<err<<endl;
}