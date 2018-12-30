#include <pch.h>
#include <binfile\PackStream.h>
#include <memory\pool.h>
#include <zip\zutil.h>

BinFile::filepos_t  PackStream::_seek (filepos_t offset)
{
  if (canread ())
  {
    if (offset < 0) 
      return -1L;

    if (offset >= stream.total_out) 
    {
        offset -= stream.total_out;
    } 
    else if (!reset ()) 
        return -1L;

    if (offset && outbuf == Z_NULL)
        outbuf = ext_new Byte [buf_size];

    while (offset > 0)  
    {
      int size = buf_size;

      if (offset < buf_size) 
        size = offset;

      size = read (outbuf,size);

      if (size <= 0) 
        return -1L;

      offset -= size;
    }

    return (z_off_t)stream.total_out;
  }
  else if (canwrite ())
  {
    if (offset < 0) 
      return -1L;

    if (!inbuf) 
    {
      inbuf = ext_new Byte [buf_size];
      
      if (!inbuf)
        return mFilePos;

      zmemzero (inbuf,buf_size);
    }

    while (offset > 0)  
    {
      size_t size = buf_size;

      if (offset < buf_size) 
        size = offset;

      size = write (inbuf,size);

      if (!size)
        return -1L;

      offset -= size;
    }

    return (z_off_t)stream.total_in;  
  }
  else return mFilePos;
}

bool       PackStream::_reset  ()
{
  if (canread ())
  {
    z_err = Z_OK;
    z_eof = 0;
    stream.avail_in = 0;
    stream.next_in  = inbuf;
    crc = crc32(0L, Z_NULL, 0);
        
    if (!file->reset ())
       return false;

    inflateReset(&stream);

    BinFile::_reset ();

    return true;
  }

  if (canwrite ())
  {
    z_err = Z_OK;
    z_eof = 0;
    stream.avail_out = buf_size;
    stream.next_out  = outbuf;
    crc = crc32(0L, Z_NULL, 0);
        
    flush ();

    if (!file->reset ())
       return false;

    deflateReset(&stream);

    BinFile::_reset ();

    return true;
  }

  return false;
}

void       PackStream::_flush  ()
{
  uInt len;
  int done = 0;

  stream.avail_in = 0; 

  for (;;) 
  {
    len = buf_size - stream.avail_out;

    if (len) 
    {
      if ((uInt)file->write(outbuf,len) != len) 
      {
        z_err = Z_ERRNO;
        return;
  //      return Z_ERRNO;
      }

      stream.next_out = outbuf;
      stream.avail_out = buf_size;
    }

    if (done) break;

    z_err = deflate(&(stream),Z_FINISH); //???????

    if (!len && z_err == Z_BUF_ERROR) z_err = Z_OK;

    done = (stream.avail_out || z_err == Z_STREAM_END);

    if (z_err != Z_OK && z_err != Z_STREAM_END) break;
  }

//  return  z_err == Z_STREAM_END ? Z_OK : z_err;
}

bool       PackStream::valid  () const
{
  return IsValid (file) ; //???
}

size_t  PackStream::_read  (void* buf,size_t len,filepos_t fpos)
{
  Bytef* start = (Bytef*)buf;
  Byte*  next_out; 

  if (z_err == Z_DATA_ERROR || z_err == Z_ERRNO) return 0; //ret - 1
  if (z_err == Z_STREAM_END) return 0;  /* EOF */

  next_out            = (Byte*)buf;
  stream.next_out  = (Bytef*)buf;
  stream.avail_out = len;

  while (stream.avail_out) 
  {
    if (!stream.avail_in && !z_eof) 
    {
      stream.avail_in = file->read (inbuf,buf_size); //??

      if (!stream.avail_in) 
        z_eof = 1;

      stream.next_in = inbuf;
    }

    z_err = inflate(&(stream), Z_NO_FLUSH);

    if (z_err != Z_OK || z_eof) break;
  }

  crc = crc32(crc, start, (uInt)(stream.next_out - start));

  return (int)(len - stream.avail_out);
}

size_t  PackStream::_write (const void* buf,size_t len,filepos_t fpos)
{
  stream.next_in  = (Bytef*)buf;
  stream.avail_in = len;

  while (stream.avail_in) 
  {
    if (!stream.avail_out) 
    {
      stream.next_out = outbuf;

      if (file->write (outbuf,buf_size) != buf_size) 
      {
        z_err = Z_ERRNO;
        break;          
      }

      stream.avail_out = buf_size;
    }

    z_err = deflate (&stream,Z_NO_FLUSH);

    if (z_err != Z_OK) 
      break;
  }

  crc = crc32(crc, (const Bytef *)buf, len);

  return (size_t)(len - stream.avail_in);
}

void    PackStream::destroy ()
{
  if (stream.state != NULL) 
  {
    if (canwrite ())     deflateEnd (&stream);
    else if (canread ()) inflateEnd (&stream);
  }

  if (inbuf)  delete [] inbuf;
  if (outbuf) delete [] outbuf;
}

void*   PackStream::AllocData (void* ptr,uInt items,uInt size)
{
  return ((Pool*)ptr)->alloc (size*items);
}

void    PackStream::FreeData  (void* ptr,void* buf)
{
  ((Pool*)ptr)->free (buf);
}

PackStream::PackStream  (BinFile* f,int mode,int packMode,Pool* pool,size_t size,bool del)
          : MemObject (pool),
            file      (f),
            autodel   (del),
            buf_size  (size)
{
  if (mode == -1)
    mode = file->getmode ();

  if (mode & can_write) mode &= ~can_read;
//  if (mode & can_seek)  mode &= ~can_seek;

  mMode    = (file->getmode () & mode) | can_resize | can_read_resize;
  mFileLen = 0;
//  mFileLen = file->size (); 

  int err;

  static int strateges [5] = {Z_DEFAULT_STRATEGY,
                              Z_HUFFMAN_ONLY,
                              Z_HUFFMAN_ONLY,
                              Z_FILTERED,
                              Z_FILTERED
                             };
  static int levels [5]    = {Z_NO_COMPRESSION,
                              Z_BEST_SPEED,
                              Z_BEST_COMPRESSION,
                              Z_BEST_SPEED,
                              Z_BEST_COMPRESSION
                             }; 

  int level    = levels [packMode]; 
  int strategy = strateges [packMode];

  stream.zalloc = AllocData;
  stream.zfree  = FreeData;
  stream.opaque = GetPool ();

  stream.next_in  = inbuf  = Z_NULL;
  stream.next_out = outbuf = Z_NULL;
  stream.avail_in = stream.avail_out = 0;

  z_err = Z_OK;
  z_eof = 0;
  crc   = crc32(0L, Z_NULL, 0);
//  transparent = 0; 

  if (canwrite ()) 
  {
    err = deflateInit2(&(stream), level,
                       Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, strategy);

    stream.next_out = outbuf = ext_new Byte [buf_size];

    if (err != Z_OK || outbuf == Z_NULL)
    {
      destroy ();
      return;
    }   
  } 
  else 
  {
    stream.next_in  = inbuf = ext_new Byte [buf_size];

    err = inflateInit2(&(stream), -MAX_WBITS);

    if (err != Z_OK || inbuf == Z_NULL) 
    {
      destroy ();
      return;
    }
  }

  stream.avail_out = buf_size;
}

PackStream::~PackStream ()
{
  if (canwrite ())
    flush ();

  destroy (); 

  if (autodel)
    delete file;
}
