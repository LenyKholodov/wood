#include <pch.h>
#include <graph\tga.h>

const    int    TGA_TYPE_COLOR  = 2;
const    int    TGA_TYPE_GRAY   = 3;

bitmap_t*       LoadTGA  (BinFile* file,Pool* pool,ostream& os)
{
  return LoadTGA (file,pool,os,NULL);
}

bitmap_t*       LoadTGA  (BinFile* file,Pool* pool,ostream& os,void*)
{
  bitmap_t* bmp = NULL;

  D_Try
  {
    TGAHeader header;
    int       width, height, bpp;
    static    char     GrayMask [12] = {0, 0, TGA_TYPE_GRAY, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    static    char     RGBMask  [12] = {0, 0, TGA_TYPE_COLOR, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    file->read (&header,sizeof (TGAHeader));    

    if (!memcmp(RGBMask,&header,sizeof(RGBMask)) && !memcmp(GrayMask,&header,sizeof(GrayMask)))
    {
      os<<"LoadTGA: Unsupported format: "<<header.ImageType<<endl;
      return NULL;    
    }    

    width       = header.Width;    
    height      = header.Height;
    bpp         = header.PixelDepth;

    if (bpp != 32 && bpp != 24 && bpp != 8) 
    {
      os<<"LoadTGA: Wrong image bpp: "<<bpp<<endl;
      return NULL;
    }

    bmp = (bitmap_t*)pool->alloc (sizeof (bitmap_t) + 
           width * height * (bpp>>3));

    if (bmp == NULL)
    {
      os<<"LoadTGA: Error at alloc bitmap Width: "<<width
        <<" Height: "<<height<<" BPP: "<<bpp<<endl;
      return NULL;  
    }              

    bmp->width    = width;
    bmp->height   = height;
    bmp->bpp      = bpp;
    
    file->read (bmp->data,(bpp>>3) * width * height);

    if (bpp >= 24)
    {
      int   size   = width * height;
      char* data   = bmp->data;
      bpp        >>= 3;

      for (uint index=0;index<size;index++,data+=bpp)
        swap (*data,*(data+2));
    }

    return bmp;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    os<<"LoadTGA: Undefined error "<<endl;

    D_Try
    {
      if (bmp) pool->free (bmp);
    }
    D_Exception (EXCEPTION_EXECUTE_HANDLER)
    {
    }

    return NULL;
  }        
}
