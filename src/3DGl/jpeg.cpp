#include <pch.h>
#include <graph\jpeg.h>
#include <graph\jpeglib.h>

#pragma comment (lib,"jpeg.lib")

bitmap_t*  LoadJPEG  (BinFile* file,Pool* pool,ostream& os)
{
  return LoadJPEG (file,pool,os,NULL);
}

bitmap_t*  LoadJPEG  (BinFile* file,Pool* pool,ostream& os,void*)
{
  char*         tempBuf  = NULL;
  bitmap_t*     bmp      = NULL;
  uchar**       rptr     = NULL;   

  D_Try
  {
    jpeg_decompress_struct info;
    jpeg_error_mgr         jerr;
    FILE                   fbuf;

    tempBuf = (char*)pool->alloc (file->size());
  
    if (tempBuf == NULL)
    {
      os<<"LoadJPEG: Error at alloc memory for temp file buffer "<<endl;
      return NULL;
    }

    file->read (tempBuf,file->size ());

    info.err        = jpeg_std_error(&jerr);
    jerr.error_exit = NULL;

    fbuf._base      = tempBuf;
    fbuf._ptr       = tempBuf;
    fbuf._bufsiz    = file->size ();
    fbuf._cnt       = file->size ();
    fbuf._file      = -1;
    fbuf._charbuf   = 0;
    fbuf._flag      = _IOMYBUF|_IORW;
    fbuf._tmpfname  = NULL;

    jpeg_create_decompress  (&info);        
    jpeg_stdio_src          (&info,&fbuf);
    jpeg_read_header        (&info,TRUE);
    jpeg_start_decompress   (&info);

    bmp            = (bitmap_t*)pool->alloc (sizeof (bitmap_t) + 
                      info.image_width * info.num_components * info.image_height);

    if (bmp == NULL)
    {
      os<<"LoadJPEG: Error at alloc memory for bitmap size: "
        <<sizeof (bitmap_t) + info.image_width * info.num_components * info.image_height
        <<endl;
      pool->free (tempBuf);
      return NULL;
    }

    bmp->width      = info.image_width;
    bmp->height     = info.image_height;
    bmp->bpp        = info.num_components << 3;

    if (bmp->bpp == 24)
    {  
      rptr            = (uchar**)pool->alloc (sizeof (uchar*)*bmp->height);
      size_t  sline   = info.image_width * info.num_components;
      int     rread   = 0;
  
      if (rptr == NULL)
      {
        os<<"LoadJPEG: Error at alloc memory for row pointers temp buffer "<<endl;
        pool->free (bmp);
        pool->free (tempBuf);
        return NULL;
      }

      for (int i=0;i<bmp->height;i++)
          rptr [i] = (uchar*)(bmp->data + i*sline);
  
      while (info.output_scanline < info.output_height) 
        rread += jpeg_read_scanlines(&info,&rptr[rread],info.output_height - rread);

      pool->free (rptr);
    }
    else
    {
      os<<"LoadJPEG: Wrong image bpp: "<<bmp->bpp<<endl;
      pool->free (bmp);
      bmp = NULL;
    }
    
    jpeg_destroy_decompress(&info);
         
    pool->free (tempBuf);  
  
    return bmp;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    os<<"LoadJPEG: Error at load image: Undefined "<<endl;
    
    D_Try
    {
    }
    D_Exception (EXCEPTION_EXECUTE_HANDLER)
    {
      if (rptr)       pool->free (rptr);
      if (bmp)        pool->free (bmp);
      if (tempBuf)    pool->free (tempBuf);
    }
    
    return NULL;    
  }
}
