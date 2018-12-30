#include <pch.h>
#include <graph\bmp.h>

DWORD    BMP_SIGN  = 0x4D42;

#pragma pack(1)

struct   palrgb_t
{
  uchar  red;
  uchar  green;
  uchar  blue;
  uchar  reserved;
};

struct  rgb16_t
{
  uint16        red   : 5;   
  uint16        green : 6;
  uint16        blue  : 5;
};

struct  rgb15_t
{
  uint16        red   : 5;   
  uint16        green : 5;
  uint16        blue  : 5;
};

template <class Src>
static void LoadRGBImage (BinFile* file,bitmap_t* bmp,WinBMPHeader& header,long _BMPOffs,Src* src)
{
  uchar temp [4];
  uint     width     = header.Width;
  uint     height    = header.Height;
  size_t   Appendix  = (4 - width * sizeof (Src)) & 3;
  size_t   sline     = width*sizeof (Src);
  rgb_t*   dest      = (rgb_t*)bmp->data + width * (height-1);

  file->seek (_BMPOffs);

  while (height-- > 0)
  {
    file->read (src,sline);
    file->read (temp,Appendix); 

    for (int i=0;i<width;i++,src++,dest++)
    {
      dest->red   = src->blue;
      dest->green = src->green;
      dest->blue  = src->red;
    }     

    dest -= width << 1;
    src  -= width;
  }
}

static void    LoadBMP8bit (BinFile* file,bitmap_t* bmp,palrgb_t* pal,WinBMPHeader& header,long _BMPOffs,void* buf)
{
  char   temp [4];
  long   width     = header.Width;
  long   height    = header.Height;
  long   Appendix  = (4-width) & 3;  
  uchar* src       = (uchar*)buf;   
  rgb_t* dest      = (rgb_t*)bmp->data + width * (height - 1);

  file->seek (sizeof (WinBMPFileHeader)+header.Size);

  file->read (pal,_BMPOffs-file->getpos());
  file->seek (_BMPOffs);
             
  while (height-- > 0)
  {
    file->read (src=(uchar*)buf,width);
    file->read (temp,Appendix);   

    for (int i=0;i<width;i++,src++,dest++)
    {
      int index   = *src;

      dest->red   = pal [index].blue;
      dest->green = pal [index].green;
      dest->blue  = pal [index].red;
    }      

    dest -= width << 1;
  }               
}

static void  LoadBMP8bitComp (BinFile* file,bitmap_t* bmp,palrgb_t* pal,WinBMPHeader& header,long _BMPOffs,void* buf)
{
  char   temp;
  long   width     = header.Width;
  long   height    = header.Height;
  long   Appendix  = (4-width) & 3;  
  uchar* src       = (uchar*)buf;
  rgb_t* dest      = (rgb_t*)bmp->data;  
  int    lump      = 0 ;

  size_t LOAD_CHUNK = width * 4;
  
  file->seek (sizeof (WinBMPFileHeader)+header.Size);

  file->read (pal,_BMPOffs-file->getpos());
  file->seek (_BMPOffs);

  size_t pos = 0;

  for (int done=false;!done;)
  {   
    uchar code;

    file->read (&code,1);

    if (code)
    {
      int index = 0;

      file->read (&index,1);

      for (int i=0;i<code;i++,dest++)
      {
        dest->red   = pal [index].blue;
        dest->green = pal [index].green;
        dest->blue  = pal [index].red;        
      }

      pos  += code;
    }  
    else
    {
      file->read (&code,1);

      size_t miss = 0;  //пропуск

      switch (code)
      {
        case 0:
          miss = (width - pos % width) % width;
          break;
        case 1:
          done = true;
          miss = width * height - pos;
          break;
        case 2:
        {
          uchar pos [2];

          file->read (pos,2);

          miss = long(pos[1])*width+pos [0];

          break;  
        }  
        default:
        {
          while (code)
          {
            size_t s = code < LOAD_CHUNK ? code : LOAD_CHUNK;

            file->read (src=(uchar*)buf,s);
            file->read (&temp,code&1); //for last chunk

            for (int i=0;i<s;i++,dest++,src++)
            {
              int index = *src;

              dest->red   = pal [index].blue;
              dest->green = pal [index].green;
              dest->blue  = pal [index].red;
            }

            code -= s;
            pos  += s;
          }

          break;
        }
      }

      for (int i=0;i<miss;i++,dest++)
      {
        dest->red   = pal [0].blue;
        dest->green = pal [0].green;
        dest->blue  = pal [0].red;
      }

      pos += miss;
    }    
  }  

  size_t line = width * 3;
  uchar* up   = (uchar*)bmp->data;
  uchar* down = (uchar*)bmp->data + line * (height - 1);

  for (int i=0;i<height>>1;i++,up+=line,down-=line)
  {
    memcpy (buf,down,line);
    memcpy (down,up,line);
    memcpy (up,buf,line);
  }
} 

bitmap_t*       LoadBMP  (BinFile* file,Pool* pool,ostream& os)
{
  return LoadBMP (file,pool,os,NULL);
}

bitmap_t*       LoadBMP  (BinFile* file,Pool* pool,ostream& os,void*)
{
  bitmap_t* bmp = NULL;
  void*     buf = NULL;
 
  D_Try
  {
    WinBMPFileHeader      fileHeader;
    WinBMPHeader          header;

    file->read (&fileHeader,sizeof (fileHeader));
 
    if (fileHeader.FileType != BMP_SIGN)
    {
      os<<"LoadBMP: Wrong file signature "<<endl;
      return NULL;
    }

    file->read (&header,sizeof (header));  

    bmp = (bitmap_t*)pool->alloc (sizeof (bitmap_t) + header.Width*header.Height*3);
    buf = pool->alloc (header.Width*4);
 
    if (!bmp || !buf)
    {
      os<<"LoadBMP: Error at alloc bitmap Width: "<<header.Width
        <<" Height: "<<header.Height<<" BPP: "<<header.BPP<<endl;
      
      if (bmp) pool->free (bmp);
      if (buf) pool->free (buf);

      return NULL;  
    }

    bmp->width    = header.Width;
    bmp->height   = header.Height;
    bmp->bpp      = header.BPP;

    palrgb_t pal  [256];
    BOOL     Error = 0;

    switch (header.BPP)
    {
      case 32: 
        LoadRGBImage (file,bmp,header,fileHeader.BitmapOffset,(rgba_t*)buf);
        break;
      case 24:
        LoadRGBImage (file,bmp,header,fileHeader.BitmapOffset,(rgb_t*)buf);
        break;  
      case 15:
        LoadRGBImage (file,bmp,header,fileHeader.BitmapOffset,(rgb15_t*)buf);
        break;  
      case 16:
        LoadRGBImage (file,bmp,header,fileHeader.BitmapOffset,(rgb16_t*)buf);
        break;  
      case 8:
        if (header.Compression == 0) 
          LoadBMP8bit  (file,bmp,pal,header,fileHeader.BitmapOffset,buf);
        else if (header.Compression == 1) 
          LoadBMP8bitComp (file,bmp,pal,header,fileHeader.BitmapOffset,buf);
        else  
          Error = 1;
        break;
      default:
        Error = 1;
    }

    if (Error)
    {
      os<<"LoadBMP: Error at load body "<<endl;

      pool->free (buf);
      pool->free (bmp);
      
      return NULL;
    }    

    pool->free (buf);
    
    bmp->bpp = 24; 

    return bmp;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    os<<"LoadBMP: Undefined error "<<endl;

    D_Try
    {
      if (buf) pool->free (buf);
      if (bmp) pool->free (bmp);
    }
    D_Exception (EXCEPTION_EXECUTE_HANDLER)
    {
    }

    return NULL;
  }
}
