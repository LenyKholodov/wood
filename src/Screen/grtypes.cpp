#include <pch.h>
#include <screen\grtypes.h>
#include <graph\bmp.h>
#include <graph\tga.h>
#include <graph\jpeg.h>
#include <misc.h>

/////////////////////implementation of class image_t////////////////////////////

image_t* image_t::create  (int w,int h,int bpp)
{  
  image_t* img = (image_t*)(new char [w*h*(bpp>>3)+sizeof (image_t)]);

  if (!img)
    return NULL;

  img->width  = w;
  img->height = h;
  img->bpp    = bpp;
  img->data   = (uchar*)img->mData;

  return img;
}

image_t* image_t::create  (int w,int h,int bpp,void* data)
{
  image_t* img = (image_t*)(new char [sizeof (image_t)]);

  if (!img)
    return NULL;

  img->width  = w;
  img->height = h;
  img->bpp    = bpp;
  img->data   = (uchar*)data;

  return img;
}

void     image_t::dispose (image_t* img)
{
  if (img)
    delete [] (char*)img;
}

bool     image_t::save    (BinFile* file)    const
{
  if (bpp < 15)  //only rgb images
    return false;

  size_t app;
  uchar  tmp [4] = {0};
  uchar* ptr     = data + (height - 1) * width * (bpp>>3);
  size_t size    = width * (bpp>>3);

  switch (bpp)
  {
    case 8:  app = (4-width) & 3;   break;
    case 15: 
    case 16: app = width&1?2:0;     break;
    case 24: app = (4-width*3) & 3; break;
    case 32: app = 0;               break;
  }

  WinBMPFileHeader    fh;
  WinBMPHeader        bh;

  memset (&bh,0,sizeof (bh));

  bh.Size             = sizeof (bh);
  bh.Width            = width;
  bh.Height           = height;
  bh.BPP              = bpp;  
  bh.Planes           = 1;
  bh.HorzResolution   = 32000;
  bh.VertResolution   = 32000;
  bh.SizeOfBitmap     = (width * (bpp>>3) + app) * height;

  fh.FileType         = 0x4D42;
  fh.FileSize         = sizeof (fh) + sizeof (bh) + bh.SizeOfBitmap;
  fh.Reserved [0]     = fh.Reserved [1] = 0;
  fh.BitmapOffset     = sizeof (fh) + sizeof (fh);

  file->write (&fh,sizeof (fh));
  file->write (&bh,sizeof (bh));
  
  for (int h=height;h;h--,ptr-=size)
  {
    file->write (ptr,size);
    file->write (tmp,app);
  }

  return true;
}

bool     image_t::save    (const char* name) const
{
  BinFile* file = F_OpenFile (name,BF_OPEN_CR);

  if (!IsValid (file))
    return false;

  if (!save (file))
    return false;

  delete file;

  return true;
}

void*    image_t::operator new       (size_t size)
{
  return ::operator new (size);
}

void*    image_t::operator new []    (size_t size)
{
  return NULL;
}

void     image_t::operator delete [] (void* x)
{  }

void     image_t::operator delete (void* x)
{
  image_t* image = (image_t*)x;

  if (image) dispose (image);
}

image_t::image_t  (): width (0), height (0), bpp (0), data (NULL) {}
image_t::image_t  (int w,int h,int _bpp):width (w), height (h), bpp (_bpp),data (mData) {}
image_t::~image_t () {}


///////////////////////implementation of class sprite_t/////////////////////////

void*    sprite_t::operator new []    (size_t size)
{
  return NULL;
}

void     sprite_t::operator delete [] (void* x)
{  }

void     sprite_t::operator delete (void* x)
{
  sprite_t* image = (sprite_t*)x;

  if (image) dispose (image);
}

sprite_t::sprite_t  (): width (0), height (0), bpp (0),x (0), y (0) {}
sprite_t::~sprite_t () {}

sprite_t* sprite_t::create   (const image_t* image,color_t zc)
{
  if (!image)
    return 0;

  sprite_t* sprite = (sprite_t*)(new char [image->size () * 2 + image->height * 4]);

  if (!sprite)
    return NULL;

  sprite->width   = image->width;
  sprite->height  = image->height;
  sprite->bpp     = image->bpp == 24 ? 32 : image->bpp;
  sprite->x       = 0;
  sprite->y       = 0;

  strip_t* strip  = (strip_t*)sprite->data;
  int      height = image->height,
           width  = image->width;
  int      lsize  = image->width * (image->bpp >> 3);
  uchar*   src    = image->data;

  ulong    mask;
  int      bpp, dBpp;
  int      shift;

  switch (image->bpp)
  {
    case 8:  shift = 24; bpp = 1; mask = 0xFF;       break;
    case 15:
    case 16: shift = 16; bpp = 2; mask = 0xFFFF;     break;
    case 24: shift = 8;  bpp = 3; mask = 0xFFFFFF;   break;
    case 32: shift = 0;  bpp = 4; mask = 0xFFFFFFFF; break;
    default: return NULL;   
  }

  dBpp = bpp == 3 ? 4 : bpp; 

  for (int y=0;y<height;y++)
  {
    for (int x=0;x<width;strip=strip->next ())
    {
      strip->len  = 0;
      strip->offs = 0;      

      uchar* dest = strip->data;

      while (((*(ulong*)src) & mask) >> shift != zc && x < width)
      {
        memcpy (dest,src,bpp);
        src   += bpp;
        dest  += dBpp;
        strip->len++;
        x++;
      }

      if (x == width)
      {
        strip->len *= dBpp;
        strip       = strip->next ();
        break;
      }

      while (((*(ulong*)src) & mask) >> shift == zc && x < width)
      {
        src += bpp;
        strip->offs++;
        x++;
      }

      strip->len  *= dBpp;
      strip->offs *= dBpp;
    }  
  }

  return sprite;
}

void      sprite_t::dispose  (sprite_t* img)
{
  if (img)
    delete [] (char*)img;
}

struct  spriteheader_t
{
  ulong         id;
  size_t        size;  
};

const ulong SPRITE_FILE_ID = *(ulong*)"SPR ";

bool      sprite_t::save     (BinFile* file)    const
{
  spriteheader_t hdr;

  hdr.id   = SPRITE_FILE_ID;
  hdr.size = size ();

  file->write (&hdr,sizeof (hdr));
  file->write (this,hdr.size);

  return true;
}

bool      sprite_t::save     (const char* name) const
{
  BinFile* file = F_OpenFile (name,BF_OPEN_CR);

  if (!IsValid (file))
    return false;

  if (!save (file))
    return false;

  delete file;

  return true;
}

size_t     sprite_t::size () const 
{
  size_t    size  = sizeof (sprite_t);
  strip_t*  strip = (strip_t*)data;
  uint      w     = width * (bpp >> 3);

  for (int y=0;y<height;y++)
    for (int x=0;x<w;x+=strip->len+strip->offs,strip=strip->next())
      size += strip->len + sizeof (strip_t);

  return size;
}

sprite_t*  sprite_t::create (BinFile* file)
{
  spriteheader_t hdr;

  file->read (&hdr,sizeof (hdr));

  if (hdr.id != SPRITE_FILE_ID)
    return NULL;

  sprite_t* sprite = (sprite_t*)(new char [hdr.size]);  

  if (!sprite)
    return NULL;

  file->read (sprite,hdr.size);

  return sprite;
}

sprite_t*   sprite_t::create (const char* name)
{
  BinFile* file = F_OpenFile (name,BF_OPEN_RO);

  if (!IsValid (file))
    return NULL;

  sprite_t* image = create (file);

  delete file;

  return image;
}

////////////////////////implementation of class imagelist_t/////////////////////

void*    imagelist_t::operator new []    (size_t size)
{
  return NULL;
}

void     imagelist_t::operator delete [] (void* x)
{  }

void     imagelist_t::operator delete (void* x)
{
  imagelist_t* image = (imagelist_t*)x;

  if (image) dispose (image);
}

imagelist_t::imagelist_t  (): count (0), bpp (0) {}
imagelist_t::~imagelist_t () {}

void      imagelist_t::dispose  (imagelist_t* img)
{
  if (img)
    delete [] (char*)img;
}

imagelist_t* imagelist_t::create (int c,image_t** imgs)
{
  if (!imgs || !imgs [0])
    return NULL;

  int bitpp = imgs [0]->bpp;

  if (bitpp != 8 && bitpp != 15 && bitpp != 16 && bitpp != 24 && bitpp != 32)
    return NULL;

  size_t size = sizeof (imagelist_t) + sizeof (long) * c;

  for(int i=0;i<c;size+=imgs [i]->size (),i++)
    if (!imgs [i] || imgs [i]->bpp != bitpp)   
      return NULL;

  imagelist_t* list = (imagelist_t*)(new char [size]);

  if (!list)
    return NULL;

  list->bpp   = bitpp;
  list->count = c;     

  long* index = (long*)list->data;

  index [0]   = sizeof(long) * c;

  memcpy (list->GetImage (0),imgs [0],imgs[0]->size());

  for (i=1;i<c;i++)
  {
    index [i] = index[i-1] + imgs[i-1]->size ();
    memcpy(list->GetImage (i),imgs [i],imgs [i]->size());
  }

  return list;
}

imagelist_t* imagelist_t::create   (int c,int bitpp,crect_t* rect)
{
  if (!rect)
    return NULL;

  size_t  size = sizeof(imagelist_t) + sizeof(long) * c;

  for (int i=0;i<c;size+=image_t::size (rect [i].width(),rect [i].height(),bitpp),i++);

  imagelist_t* list = (imagelist_t*)(new char [size]);

  if (!list)
    return NULL;

  list->bpp   = bitpp;
  list->count = c;
    
  long* index = (long*)list->data;

  index [0]   = sizeof(long) * c;

  new (list->GetImage (0)) image_t (rect [0].width (),rect [0].height (),bitpp);

  for(i=1;i<c;i++)
  {
    index [i] = index[i-1] + list->GetImage(i-1)->size();
    new (list->GetImage (i)) image_t (rect [i].width (),rect [i].height (),bitpp);
  }

  return list;
}

size_t   imagelist_t::size () const
{
  return size_t (GetImage (count-1)) + GetImage (count-1)->size () - size_t (this);
}

struct ilheader_t
{
  ulong   id;  
  size_t  size;
};

const ulong IL_FILE_ID = *(ulong*)"ILST";

bool      imagelist_t::save     (BinFile* file) const
{
  ilheader_t hdr;

  hdr.id    = IL_FILE_ID;
  hdr.size  = size ();

  file->write (&hdr,sizeof (hdr));
  file->write (this,hdr.size);

  return true;
}

bool      imagelist_t::save     (const char* name) const
{
  BinFile* file = F_OpenFile (name,BF_OPEN_CR);

  if (!IsValid (file))
    return false;

  if (!save (file))
    return false;

  delete file;

  return true;
}

imagelist_t* imagelist_t::create   (BinFile* file)
{
  ilheader_t hdr;

  file->read (&hdr,sizeof (hdr));

  if (hdr.id != IL_FILE_ID)
    return NULL;

  imagelist_t* list = (imagelist_t*)(new char [hdr.size]);     

  if (!list)
    return NULL;

  file->read (list,hdr.size);

  return list;
}

imagelist_t* imagelist_t::create   (const char* name)
{
  BinFile* file = F_OpenFile (name,BF_OPEN_RO);

  if (!IsValid (file))
    return NULL;

  imagelist_t* image = create (file);

  delete file;

  return image;
}
