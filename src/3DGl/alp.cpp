#include <pch.h>
#include <graph\alp.h>
#include <graph\bmp.h>
#include <graph\tga.h>
#include <graph\jpeg.h>

const long ALP_ID = *((ulong*)"ALP ");

void SaveALP  (BinFile* dest,BinFile* src,Pool* pool,const rgb_t& col1,const rgb_t& col2,uint koeftrue,uint koeffalse,int fmt,ostream& os)
{
  D_Try
  {
    if (!IsValid (dest) || !IsValid (src))
    {
      os<<"Wrong file(s) ptr"<<endl;
      return;
    }

    alpheader_t  hdr;

    hdr.Id               = ALP_ID;
    hdr.Offs [0]         = sizeof (hdr);
    hdr.Size [0]         = src->size ();
    hdr.Format [0]       = fmt;
    hdr.Offs [1]         = 0;
    hdr.Size [1]         = 0;
    hdr.Format [1]       = FMT_UNKNOWN;
    hdr.KoefTrue         = koeftrue;
    hdr.KoefFalse        = koeffalse;

    memcpy (&hdr.FirstCol,&col1,sizeof (rgb_t));
    memcpy (&hdr.LastCol,&col2,sizeof (rgb_t));

    void* buf = pool->alloc (src->size ());

    if (!buf)
    {
      os<<"Error at SaveALP: No enough memory "<<src->size ()<<endl;
      return;
    }

    src->read   (buf,src->size ());
    dest->write (&hdr,sizeof (hdr));
    dest->write (buf,src->size ());

    pool->free  (buf);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    os<<"Error at SaveALP: Unknown error "<<endl;
    return ;
  }
}

void  SaveALP  (BinFile* dest,BinFile* srcText,BinFile* srcAlp,Pool* pool,int* fmt,ostream& os)
{
  D_Try
  {
    if (!IsValid (dest) || !IsValid (srcText) || !IsValid (srcAlp))
    {
      os<<"Wrong file(s) ptr"<<endl;
      return;
    }

    alpheader_t  hdr;

    hdr.Id               = ALP_ID;
    hdr.Offs [0]         = sizeof (hdr);
    hdr.Size [0]         = srcText->size ();
    hdr.Offs [1]         = srcText->size () + sizeof (hdr);
    hdr.Size [1]         = srcAlp->size ();
    hdr.KoefTrue         = 1;
    hdr.KoefFalse        = 1;

    memcpy (hdr.Format,fmt,sizeof (uint) * 2);

    size_t size = srcText->size () > srcAlp->size () ? srcText->size () : srcAlp->size ();
    void*   buf = pool->alloc (size);

    if (!buf)
    {
      os<<"Error at SaveALP: No enough memory "<<size<<endl;
      return;
    }

    srcText->read   (buf,srcText->size ());
    dest->write     (&hdr,sizeof (hdr));
    dest->write     (buf,srcText->size ());
    srcText->read   (buf,srcAlp->size ());
    dest->write     (buf,srcAlp->size ());

    pool->free  (buf);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    os<<"Error at SaveALP: Unknown error "<<endl;
    return ;
  }
}

#define CHECK(COL,FIRST,LAST) ((COL.red>=FIRST.red&&COL.red<=LAST.red)&&(COL.green>=FIRST.green&&COL.green<=LAST.green)&&(COL.blue>=FIRST.blue&&COL.blue<=LAST.blue))
#define CHECK8(COL,FIRST,LAST) ((COL>=FIRST.red&&COL<=LAST.red)||(COL>=FIRST.green&&COL<=LAST.green)||(COL>=FIRST.blue&&COL<=LAST.blue))

void      Convert32bit (void* _src,void* _dest,const alpheader_t& hdr,ulong size)
{
  rgba_t* src  = (rgba_t*)_src;
  rgba_t* dest = (rgba_t*)_dest;

  rgb_t   fst  = hdr.FirstCol,
          lst  = hdr.LastCol;

  for (int i=0;i<size;i++,src++,dest++)
  {
    dest->red   = src->red;
    dest->green = src->green;
    dest->blue  = src->blue;
    dest->alpha = CHECK ((*src),fst,lst) ? hdr.KoefTrue : hdr.KoefFalse;
  }  
}

void      Convert24bit (void* _src,void* _dest,const alpheader_t& hdr,ulong size)
{
  rgb_t* src   = (rgb_t*)_src;
  rgba_t* dest = (rgba_t*)_dest;

  rgb_t   fst  = hdr.FirstCol,
          lst  = hdr.LastCol;

  for (int i=0;i<size;i++,src++,dest++)
  {
    dest->red   = src->red;
    dest->green = src->green;
    dest->blue  = src->blue;
    dest->alpha = CHECK ((*src),fst,lst) ? hdr.KoefTrue : hdr.KoefFalse;
  }  
}

void      Convert8bit (void* _src,void* _dest,const alpheader_t& hdr,ulong size)
{
  uchar * src   = (uchar*)_src;
  rgba_t* dest  = (rgba_t*)_dest;

  rgb_t   fst  = hdr.FirstCol,
          lst  = hdr.LastCol;

  for (int i=0;i<size;i++,src++,dest++)
  {
    dest->red   = *src;
    dest->green = *src;
    dest->blue  = *src;
    dest->alpha = CHECK8 ((*src),fst,lst) ? hdr.KoefTrue : hdr.KoefFalse;
  }  
}

void      Mix32bit (void* _src,void* _alp,void* _dest,ulong size,ulong step)
{
  rgba_t* src  = (rgba_t*)_src;
  uchar*  alp  = (uchar*)_alp;
  rgba_t* dest = (rgba_t*)_dest;

  for (int i=0;i<size;i++,src++,dest++,alp+=step)
  {
    dest->red   = src->red;
    dest->green = src->green;
    dest->blue  = src->blue;
    dest->alpha = *alp ? 255 : 0;
  }  
}

void      Mix24bit (void* _src,void* _alp,void* _dest,ulong size,ulong step)
{
  rgb_t*  src  = (rgb_t*)_src;
  uchar*  alp  = (uchar*)_alp;
  rgba_t* dest = (rgba_t*)_dest;

  for (int i=0;i<size;i++,src++,dest++,alp+=step)
  {
    dest->red   = src->red;
    dest->green = src->green;
    dest->blue  = src->blue;
    dest->alpha = *alp ? 255 : 0;
  }  
}

void      Mix8bit (void* _src,void* _alp,void* _dest,ulong size,ulong step)
{
  uchar * src   = (uchar*)_src;
  uchar*  alp   = (uchar*)_alp;
  rgba_t* dest  = (rgba_t*)_dest;

  for (int i=0;i<size;i++,src++,dest++,alp+=step)
  {
    dest->red   = *src;
    dest->green = *src;
    dest->blue  = *src;
    dest->alpha = *alp ? 255 : 0;
  }  
}

bitmap_t* LoadALP  (BinFile* file,Pool* pool,ostream& os)
{
  bitmap_t* bitmap  = NULL, 
          * bmp [2] = {NULL};

//  D_Try
//  {  
    if (!file)
    {
      os<<"Error at LoadALP: Wrong file ptr"<<endl;
      return NULL;
    }

    alpheader_t hdr;

    file->read (&hdr,sizeof (hdr));

    if (hdr.Id != ALP_ID)
    {
      os<<"LoadALP: Wrong file format"<<endl;
      return NULL;
    }

    for (int i=0;i<2;i++)
    {
      if (!hdr.Size [i])
        break;

      FrameBinFile abf (file,hdr.Offs [i],hdr.Size [i]);
      
      switch (hdr.Format [i])
      {
        case FMT_BMP:  bmp [i] = LoadBMP  (&abf,pool,os); break;
        case FMT_JPEG: bmp [i] = LoadJPEG (&abf,pool,os); break;
        case FMT_TGA:  bmp [i] = LoadTGA  (&abf,pool,os); break;
        default:
          if (!(bmp [i] = LoadBMP (&abf,pool,os)))
          {
            abf.reset ();

            if (!(bmp [i] = LoadJPEG (&abf,pool,os)))
            {
              abf.reset ();
              bmp [i] = LoadTGA (&abf,pool,os);
            }
          }    
          break;                          
      }
      
      if (!bmp [i])
      {
        os<<"LoadALP: Error load bitmap from file "<<endl;
        
        if (bmp [0]) pool->free (bmp [0]);
        if (bmp [1]) pool->free (bmp [1]);

        return NULL;
      }
    }

    bitmap = (bitmap_t*)pool->alloc (bmp [0]->width*bmp [0]->height*4+sizeof (bitmap_t));

    if (!bitmap)
    {
      os<<"LoadALP: Error at alloc memory for bitmap"<<endl;

      if (bmp [0]) pool->free (bmp [0]);
      if (bmp [1]) pool->free (bmp [1]);

      return NULL;
    }    

    if (!bmp [1])
    {
      bitmap->width  = bmp [0]->width;
      bitmap->height = bmp [0]->height;
      bitmap->bpp    = 32;

      switch (bmp [0]->bpp)
      {      
        case 32: Convert32bit (bmp [0]->data,bitmap->data,hdr,bmp [0]->width*bmp [0]->height); break;
        case 24: Convert24bit (bmp [0]->data,bitmap->data,hdr,bmp [0]->width*bmp [0]->height); break;
        case 8:  Convert8bit  (bmp [0]->data,bitmap->data,hdr,bmp [0]->width*bmp [0]->height); break;
        default:
          os<<"LoadALP: Wrong ALP bpp: "<<bmp [0]->bpp<<endl;
          pool->free (bitmap);  bitmap  = NULL;
          pool->free (bmp [0]); bmp [0] = NULL;
          return NULL;        
      }

      pool->free (bmp [0]);
    }
    else
    {
      if ((bmp [0]->width != bmp [1]->width) || (bmp [0]->height != bmp [1]->height))
      {
        if (bmp [0]) pool->free (bmp [0]);
        if (bmp [1]) pool->free (bmp [1]); 
        if (bitmap)  pool->free (bitmap);
        return NULL;
      }

      bitmap->width  = bmp [0]->width;
      bitmap->height = bmp [0]->height;
      bitmap->bpp    = 32;

      switch (bmp [0]->bpp)
      {      
        case 32: Mix32bit (bmp [0]->data,bmp [1]->data,bitmap->data,bmp [0]->width*bmp [0]->height,bmp [1]->bpp>>3); break;
        case 24: Mix24bit (bmp [0]->data,bmp [1]->data,bitmap->data,bmp [0]->width*bmp [0]->height,bmp [1]->bpp>>3); break;
        case 8:  Mix8bit  (bmp [0]->data,bmp [1]->data,bitmap->data,bmp [0]->width*bmp [0]->height,bmp [1]->bpp>>3); break;
        default:
          os<<"LoadALP: Wrong ALP bpp: "<<bmp [0]->bpp<<endl;
          pool->free (bitmap);  bitmap  = NULL;
          pool->free (bmp [0]); bmp [0] = NULL;
          pool->free (bmp [1]); bmp [1] = NULL;
          return NULL;        
      }

      pool->free (bmp [0]);
      pool->free (bmp [1]);
    }

    return bitmap;
//  }
//  D_Exception (EXCEPTION_EXECUTE_HANDLER)
//  {
//    os<<"LoadALP: Unknow error"<<endl;
//    
//    D_Try
//    {
//      if (bmp [0])    pool->free (bmp [0]);
//      if (bmp [1])    pool->free (bmp [1]);
//      if (bitmap)     pool->free (bitmap);
//    }
//    D_Exception (EXCEPTION_EXECUTE_HANDLER)
//    {
//    }
//
//    return NULL;
//  }
}

bitmap_t*      ConvALP  (bitmap_t* img,bitmap_t* alp,Pool* pool,ostream& os)
{
  bitmap_t* bitmap = (bitmap_t*)pool->alloc (img->width*img->height*4+sizeof (bitmap_t));

  if (!bitmap)
  {
    os<<"ConvALP: Error at alloc memory for bitmap"<<endl;
    return NULL;
  }    

  if ((img->width != alp->width) || (img->height != alp->height))
  {
    if (bitmap)  pool->free (bitmap);
    return NULL;
  }  

  bitmap->width  = img->width;
  bitmap->height = img->height;
  bitmap->bpp    = 32;

  switch (img->bpp)
  {      
    case 32: Mix32bit (img->data,alp->data,bitmap->data,img->width*img->height,alp->bpp>>3); break;
    case 24: Mix24bit (img->data,alp->data,bitmap->data,img->width*img->height,alp->bpp>>3); break;
    case 8:  Mix8bit  (img->data,alp->data,bitmap->data,img->width*img->height,alp->bpp>>3); break;
    default:
      os<<"ConvALP: Wrong ALP bpp: "<<img->bpp<<endl;
      pool->free (bitmap);  bitmap  = NULL;
      return NULL;        
  }

  return bitmap;    
}
