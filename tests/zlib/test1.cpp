#include <kernel.h>
#include "zlib\zlib.h"
#include "zlib\zutil.h"

struct zipStream 
{
  z_stream stream;
  int      z_err;   /* error code for last stream operation */
  int      z_eof;   /* set if end of input file */
  BinFile  *file;  
  Byte     *inbuf;  /* input buffer */
  Byte     *outbuf; /* output buffer */
  uLong    crc;     /* crc32 of uncompressed data */
  char     *msg;    /* error message */
  char     *path;   /* path name for debugging only */
  int      transparent; /* 1 if input file is not a .gz file */
  char     mode;    /* 'w' or 'r' */
  long     startpos; /* start of compressed data in file (header skipped) */
};

const size_t Z_BUFSIZE = 1024*16;

int destroy (zipStream* s);

int get_byte(zipStream* s)
{
    if (s->z_eof) return EOF;
    if (s->stream.avail_in == 0) {
        errno = 0;
//        s->stream.avail_in = fread(s->inbuf, 1, Z_BUFSIZE, s->file);
        s->stream.avail_in = s->file->read(s->inbuf,Z_BUFSIZE);
        if (s->stream.avail_in == 0) {
            s->z_eof = 1;
//            if (ferror(s->file)) s->z_err = Z_ERRNO;
            return EOF;
        }
        s->stream.next_in = s->inbuf;
    }
    s->stream.avail_in--;
    return *(s->stream.next_in)++;
}


uLong getLong (zipStream* s)
{
    uLong x = (uLong)get_byte(s);
    int c;

    x += ((uLong)get_byte(s))<<8;
    x += ((uLong)get_byte(s))<<16;
    c = get_byte(s);
    if (c == EOF) s->z_err = Z_DATA_ERROR;
    x += ((uLong)c)<<24;
    return x;
}


size_t zipRead (zipStream* s,void* buf,size_t len)
{
    Bytef *start = (Bytef*)buf; /* starting point for crc computation */
    Byte  *next_out; /* == stream.next_out but not forced far (for MSDOS) */

//    if (s == NULL || s->mode != 'r') return Z_STREAM_ERROR;

    if (s->z_err == Z_DATA_ERROR || s->z_err == Z_ERRNO) return -1;
    if (s->z_err == Z_STREAM_END) return 0;  /* EOF */

    next_out = (Byte*)buf;
    s->stream.next_out = (Bytef*)buf;
    s->stream.avail_out = len;

    while (s->stream.avail_out != 0) {

        if (s->transparent) {
            /* Copy first the lookahead bytes: */
            uInt n = s->stream.avail_in;
            if (n > s->stream.avail_out) n = s->stream.avail_out;
            if (n > 0) {
                zmemcpy(s->stream.next_out, s->stream.next_in, n);
                next_out += n;
                s->stream.next_out = next_out;
                s->stream.next_in   += n;
                s->stream.avail_out -= n;
                s->stream.avail_in  -= n;
            }
            if (s->stream.avail_out > 0) {
                s->stream.avail_out -= s->file->read (next_out,s->stream.avail_out);
            
//                s->stream.avail_out -= fread(next_out, 1, s->stream.avail_out,
//                                             s->file);
            }
            len -= s->stream.avail_out;
            s->stream.total_in  += (uLong)len;
            s->stream.total_out += (uLong)len;
            if (len == 0) s->z_eof = 1;
            return (int)len;
        }
        if (s->stream.avail_in == 0 && !s->z_eof) {

            errno = 0;
//            s->stream.avail_in = fread(s->inbuf, 1, Z_BUFSIZE, s->file);
            s->stream.avail_in = s->file->read (s->inbuf,Z_BUFSIZE);
            if (s->stream.avail_in == 0) {
                s->z_eof = 1;
//                if (ferror(s->file)) {
//                    s->z_err = Z_ERRNO;
//                    break;
//                }
            }
            s->stream.next_in = s->inbuf;
        }
        s->z_err = inflate(&(s->stream), Z_NO_FLUSH);

/*        if (s->z_err == Z_STREAM_END) {
//             Check CRC and original size 
            s->crc = crc32(s->crc, start, (uInt)(s->stream.next_out - start));
            start = s->stream.next_out;

            if (getLong(s) != s->crc) {
                s->z_err = Z_DATA_ERROR;
            } else {
                (void)getLong(s);
                // The uncompressed length returned by above getlong() may
                // be different from s->stream.total_out) in case of
                 //concatenated .gz files. Check for such files:
                ///
                check_header(s);
                if (s->z_err == Z_OK) {
                    uLong total_in = s->stream.total_in;
                    uLong total_out = s->stream.total_out;

                    inflateReset(&(s->stream));
                    s->stream.total_in = total_in;
                    s->stream.total_out = total_out;
                    s->crc = crc32(0L, Z_NULL, 0);
                }
            }

        }*/
        if (s->z_err != Z_OK || s->z_eof) break;
    }
    s->crc = crc32(s->crc, start, (uInt)(s->stream.next_out - start));

    return (int)(len - s->stream.avail_out);
}

size_t  zipWrite (zipStream* s,const void* buf,size_t len)
{
//  if (!s || s->mode != 'w') return Z_STREAM_ERROR;

  s->stream.next_in  = (Bytef*)buf;
  s->stream.avail_in = len;

  while (s->stream.avail_in) 
  {
    if (s->stream.avail_out == 0) 
    {
      s->stream.next_out = s->outbuf;

      if (s->file->write (s->outbuf,Z_BUFSIZE) != Z_BUFSIZE) 
      {
        s->z_err = Z_ERRNO;
        break;          
      }

      s->stream.avail_out = Z_BUFSIZE;
    }

    s->z_err = deflate(&(s->stream), Z_NO_FLUSH);

    if (s->z_err != Z_OK) 
      break;
  }

  s->crc = crc32(s->crc, (const Bytef *)buf, len);

  return (size_t)(len - s->stream.avail_in);
}

zipStream* zipOpen (const char* path,const char* mode)
{
    int err;
    int level = Z_DEFAULT_COMPRESSION; /* compression level */
    int strategy = Z_DEFAULT_STRATEGY; /* compression strategy */
    char *p = (char*)mode;
    zipStream* s;
    char fmode[80]; /* copy of mode, without the compression level */
    char *m = fmode;

//    if (!path || !mode) return Z_NULL;

  //  s = (gz_stream *)ALLOC(sizeof(gz_stream));
    s = ext_new zipStream;

    if (!s) return Z_NULL;

    s->stream.zalloc = (alloc_func)0; //??
    s->stream.zfree  = (free_func)0; //??
    s->stream.opaque = (voidpf)0;  //??

    s->stream.next_in  = s->inbuf  = Z_NULL;
    s->stream.next_out = s->outbuf = Z_NULL;
    s->stream.avail_in = s->stream.avail_out = 0;
    s->file = NULL;
    s->z_err = Z_OK;
    s->z_eof = 0;
    s->crc = crc32(0L, Z_NULL, 0);
    s->msg = NULL;
    s->transparent = 0;

//    s->path = (char*)ALLOC(strlen(path)+1);
    s->path = ext_new char [strlen (path) + 1];

    if (!s->path) {
        return destroy(s), (zipStream*)Z_NULL;
    }

    strcpy(s->path, path); /* do this early for debugging */

    s->mode = '\0';

    do {
        if (*p == 'r') s->mode = 'r';
        if (*p == 'w' || *p == 'a') s->mode = 'w';
        if (*p >= '0' && *p <= '9') {
            level = *p - '0';
        } else if (*p == 'f') {
          strategy = Z_FILTERED;
        } else if (*p == 'h') {
          strategy = Z_HUFFMAN_ONLY;
        } else {
            *m++ = *p; /* copy the mode */
        }
    } while (*p++ && m != fmode + sizeof(fmode));

    if (s->mode == '\0') return destroy(s), (zipStream*)Z_NULL;
    
    if (s->mode == 'w') {
#ifdef NO_DEFLATE
        err = Z_STREAM_ERROR;
#else
        err = deflateInit2(&(s->stream), level,
                           Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, strategy);
        /* windowBits is passed < 0 to suppress zlib header */

//        s->stream.next_out = s->outbuf = (Byte*)ALLOC(Z_BUFSIZE);
        s->stream.next_out = s->outbuf = ext_new Byte [Z_BUFSIZE];
#endif
        if (err != Z_OK || s->outbuf == Z_NULL) {
            return destroy(s), (zipStream*)Z_NULL;
        }
    } else {
//        s->stream.next_in  = s->inbuf = (Byte*)ALLOC(Z_BUFSIZE);
        s->stream.next_in  = s->inbuf = ext_new Byte [Z_BUFSIZE];

        err = inflateInit2(&(s->stream), -MAX_WBITS);
        /* windowBits is passed < 0 to tell that there is no zlib header.
         * Note that in this case inflate *requires* an extra "dummy" byte
         * after the compressed stream in order to complete decompression and
         * return Z_STREAM_END. Here the gzip CRC32 ensures that 4 bytes are
         * present after the compressed stream.
         */
        if (err != Z_OK || s->inbuf == Z_NULL) {
            return destroy(s), (zipStream*)Z_NULL;
        }
    }

    s->stream.avail_out = Z_BUFSIZE;

    errno = 0;
//    s->file = fd < 0 ? F_OPEN(path, fmode) : (FILE*)fdopen(fd, fmode);
//    s->file = F_OpenFile (path,BF_OPEN_CR);   ///!!!ATTENT!!
    switch (s->mode)
    {
      case 'w':  s->file = F_OpenFile (path,BF_OPEN_CR);   break;
      case 'a':  s->file = F_OpenFile (path,BF_OPEN_RW);   break;
      case 'r':  s->file = F_OpenFile (path,BF_OPEN_RO);   break;
      default:   s->file = NULL;
    }  

    if (!s->file) {
        return destroy(s), (zipStream*)Z_NULL;
    }
    
    return s;
}

int do_flush (zipStream* s,int flush)
{
    uInt len;
    int done = 0;

//    if (s == NULL || s->mode != 'w') return Z_STREAM_ERROR;

    s->stream.avail_in = 0; /* should be zero already anyway */

    for (;;) {
        len = Z_BUFSIZE - s->stream.avail_out;

        if (len != 0) {
            if ((uInt)s->file->write(s->outbuf,len) != len) {
                s->z_err = Z_ERRNO;
                return Z_ERRNO;
            }
            s->stream.next_out = s->outbuf;
            s->stream.avail_out = Z_BUFSIZE;
        }
        if (done) break;
        s->z_err = deflate(&(s->stream), flush);

        /* Ignore the second of two consecutive flushes: */
        if (len == 0 && s->z_err == Z_BUF_ERROR) s->z_err = Z_OK;

        /* deflate has finished flushing only when it hasn't used up
         * all the available space in the output buffer: 
         */
        done = (s->stream.avail_out != 0 || s->z_err == Z_STREAM_END);
 
        if (s->z_err != Z_OK && s->z_err != Z_STREAM_END) break;
    }
    return  s->z_err == Z_STREAM_END ? Z_OK : s->z_err;
}


int zipClose (zipStream* s)
{
    int err;

//    if (s == NULL) return Z_STREAM_ERROR;

    if (s->mode == 'w') {
#ifdef NO_DEFLATE
        return Z_STREAM_ERROR;
#else
        err = do_flush (s,Z_FINISH);
        if (err != Z_OK) return destroy(s);

//        putLong (s->file, s->crc);
//        putLong (s->file, s->stream.total_in);
#endif
    }
    return destroy(s);
}

#define TRYFREE(p) {if (p) delete p; }

int destroy (zipStream* s)
{
    int err = Z_OK;

//    if (!s) return Z_STREAM_ERROR;

    TRYFREE (s->msg);

    if (s->stream.state != NULL) {
        if (s->mode == 'w') {
#ifdef NO_DEFLATE
            err = Z_STREAM_ERROR;
#else
            err = deflateEnd(&(s->stream));
#endif
        } else if (s->mode == 'r') {
            err = inflateEnd(&(s->stream));
        }
    }

#if 0 //my
    if (s->file != NULL && fclose(s->file)) {
#ifdef ESPIPE
        if (errno != ESPIPE) /* fclose is broken for pipes in HP/UX */
#endif
            err = Z_ERRNO;
    }
#endif //my

    delete s->file;
    s->file = NULL;
    err = Z_ERRNO;

    if (s->z_err < 0) err = s->z_err;

    TRYFREE(s->inbuf);
    TRYFREE(s->outbuf);
    TRYFREE(s->path);
    TRYFREE(s);

    return err;
}

const char writeMsg [] = "This is the small message for zip compress"
                        " and now HHHHHHHHHHHHHHHHHHHHHHHH";

void    doWrite ()
{
  zipStream* s = zipOpen ("test.pk","w9");

  cout<<"stream create: "<<(s != NULL)<<endl;

  cout<<"write "<<zipWrite (s,writeMsg,strlen (writeMsg))<<" bytes"<<endl;

  zipClose (s);
}

void    doRead ()
{
  zipStream* s = zipOpen ("test.pk","r");

  cout<<"stream create: "<<(s != NULL)<<endl;

  char msg [1024] = {0};

  cout<<"read "<<zipRead (s,msg,strlen (writeMsg))<<" bytes"<<endl;
  cout<<"read message: '"<<msg<<"'"<<endl;

  zipClose (s);
}

void    main ()
{
  K_FastInit ("test1.map");

  doWrite ();
  doRead ();
}
