#include <pch.h>
#include <screen\fscreen.h>
#include <string.h>
#include <misc.h>
#include <debug.h>

///////////////implementation of class Flat Screen//////////////////////////////

template <class T> 
FlatScreen<T>::FlatScreen (int w,int h,void* ptr)
              : memScreen  (w,h,sizeof (T) * 8,ptr==NULL),
                mData      ((T*)(ptr ? ptr : mPicture->data))
//                mData      ((T*)ptr)
{   
  if (ptr)
    mPicture->data = (uchar*)ptr;  
}

template <class T> bool FlatScreen<T>::IsValid(void) const
{
  return Screen::IsValid() && mData;
}

template <class T> long FlatScreen<T>::lo_GetPixel(int x,int y)
{
  return mData[x+y*mWidth];
}

template <class T> void FlatScreen<T>::lo_PutPixel(int x,int y,long color)
{
  mData[x+y*mWidth]=color;
}

template <class T> void FlatScreen<T>::lo_vline(int x,int y,int h,long color)
{
        for(T* d=mData+x+y*mWidth;h--;d+=mWidth)
                   *d=color;
}

template <class T> void FlatScreen<T>::lo_line(int xs,int ys,int xe,int ye,long color)
{
// rem: xs<=xe
        T* ptr;
        *(ptr=mData+xs+ys*mWidth)=color;
        int dx=xe-xs;
        int dy=abs(ye-ys);
        if(dy<=dx)
        {
                int d,d1,d2=(dy-dx)<<1;
                d=(d1=dy<<1)-dx;
                if(ye>ys)
                        for(;dx--;*++ptr=color)
                        {
                                if(d>0)
                                {
                                        d+=d2;
                                        ptr+=mWidth;
                                }
                                else
                                        d+=d1;
                        }
                else
                        for(;dx--;*++ptr=color)
                        {
                                if(d>0)
                                {
                                        d+=d2;
                                        ptr-=mWidth;
                                }
                                else
                                        d+=d1;
                        }
        }
        else
        {
                int d,d1,d2=(dx-dy)<<1;
                d=(d1=dx<<1)-dy;
                if(ye>ys)
                        for(;dy--;*(ptr+=mWidth)=color)
                        {
                                if(d>0)
                                {
                                        d+=d2;
                                        ptr++;
                                }
                                else
                                        d+=d1;
                        }
                else
                        for(;dy--;*(ptr-=mWidth)=color)
                        {
                                if(d>0)
                                {
                                        d+=d2;
                                        ptr++;
                                }
                                else
                                        d+=d1;
                        }
        }
}

template <class T> void FlatScreen<T>::lo_PutImage(int x,int y,const image_t* image,const crect_t& rect)
{
        long w=image->width,
             h=image->height,
             rw=w;
        const T* src=(const T*)image->data;
        // vertical truncate
        if(rect.bottom<y+h)
                if((h=rect.bottom-y+1)<=0)
                        return;
        if(rect.top>y)
        {
                int d=rect.top-y;
                if((h-=d)<=0)
                        return;
                src+=w*d;               
                y=rect.top;
        }
        T* dest=mData+x+y*mWidth;
        if(rect.right<x+w)
                if((rw=rect.right-x+1)<=0)
                        return;
        if(x<rect.left)
        {
                int d=rect.left-x;
                src+=d;
                dest+=d;
                if((rw-=d)<0)
                        return;
        }
        for(rw*=sizeof(T);h--;src+=w)
        {
                memcpy(dest,src,rw);
                dest+=mWidth;
        }
} 

template <class T> void FlatScreen<T>::lo_GetImage(int x,int y,int w,int h,image_t* image,const crect_t& rect)
{
        long rw=w;
        image->width=w;
        image->height=h;
        T* dest=(T*)image->data;
        // vertical truncate
        if(rect.bottom<y+h)
                if((h=rect.bottom-y+1)<=0)
                        return;
        if(rect.top>y)
        {
                int d=rect.top-y;
                if((h-=d)<=0)
                        return;
                dest+=w*d;              
                y=rect.top;
        }
        const T* src=mData+x+y*mWidth;
        if(rect.right<x+w)
                if((rw=rect.right-x+1)<=0)
                        return;
        if(x<rect.left)
        {
                int d=rect.left-x;
                src+=d;
                dest+=d;
                if((rw-=d)<0)
                        return;
        }
        for(rw*=sizeof(T);h--;src+=mWidth)
        {
                memcpy(dest,src,rw);
                dest+=w;
        }
}

template <class T> 
void FlatScreen<T>::lo_PutSprite(int x,int y,const sprite_t* image,const crect_t& rect)
{
  D_Try
  {
  //  Screen::lo_PutSprite (x,y,image,rect);

  //  return;

    if((x-=image->x)>rect.right)
            return;  

    y-=image->y;

    sprite_t::strip_t* strip = (sprite_t::strip_t*)image->data;

    int      bpp    = sizeof (T);
    ulong    shift  = sizeof (T) >> 1;
    int      swidth = image->width;

    for(int h=image->height;y<rect.top;y++)
    {
      for (int x=0;x<swidth;x+=(strip->offs+strip->len)>>shift,strip=strip->next ());

      if(--h<=0)
          return;
    }

    int    app = 0;  

    if (x < rect.left)  
    {
      app = (rect.left - x) << shift;
      x   = rect.left;
    }

    if (y+h>rect.bottom)
      if ((h=rect.bottom-y+1)<=0)
         return;

    int w;
    if (x+(w=swidth)<rect.left)
        return;
    if (x+w>rect.right)
      if((w=rect.right-x+1)<=0)
        return;  

    T*      base = mData + x + y * mWidth;
    size_t  ws   = w << shift,
            ss   = swidth << shift;

    for (;h>0;h--,y++,base += mWidth)
    {
      T*   dest = base;
      uint x    = 0;

      for (int j=0;j<app;j+=strip->len+strip->offs,strip=strip->next ())
      {
        if (j + strip->len > app)
        {
          size_t len = j + strip->len - app;
          len        = len < ws ? len : ws;
          memcpy (dest,strip->data+app-j,len);
        } 

        if (j + strip->len + strip->offs > app)
          x = j + strip->len + strip->offs - app;                  
      }
          
      for (;x<ws && j<ss;j+=strip->len+strip->offs,x+=strip->len+strip->offs,strip=strip->next ())
        memcpy (dest+(x>>shift),strip->data,strip->len + x < ws ? strip->len : ws - x);

      for (;j<ss;j+=strip->offs+strip->len,strip=strip->next ());
    }
  }
  D_Exc ("FlatScreen: Error at lo_PutSprite");
}

/////////////////////////implementation of class fScreen32bit///////////////////

void fScreen32bit::lo_hline(int x,int y,int w,long color)
{
        memsetd(mData+x+y*mWidth,color,w);
}

void fScreen32bit::lo_bar(int xs,int ys,int xe,int ye,long color)
{
        int l;
        uint32* ptr=mData+xs+ys*mWidth;
        if((l=xe-xs+1)==mWidth)
                memsetd(ptr,color,mWidth*(ye-ys+1));
        else    
                for(;ys++<=ye;ptr+=mWidth)
                        memsetd(ptr,color,l);
}

long fScreen32bit::rgb(uchar red,uchar green,uchar blue)
{
        return ulong(blue)|(ulong(green)<<8)|(ulong(red)<<16);
}

void fScreen32bit::rgb(long color,uchar& red,uchar& green,uchar& blue)
{
  red   = color>>16;
  green = color>>8;
  blue  = color;
}

//////////////////////platette screen///////////////////////////////////////////

void        fScreen8bit::refresh         ()
{
  memScreen* scr = (memScreen*)mRefreshScr; 

  if (::IsValid (scr) && scr->ptr ())
  {
    crect_t& rect = mReallyRefreshRect;
    ulong*  dest = (ulong*)scr->ptr () + rect.top * scr->width ();
    uchar*  src  = mData + rect.top * mWidth;
    ulong   app  = mWidth-rect.right-1;

    for (int y=rect.top;y<=rect.bottom;y++,dest+=app,src+=app)
      for (int x=rect.left;x<=rect.right;x++,dest++,src++)
        *dest = *(ulong*)(mPalette + *src); 
  }
}

void        fScreen8bit::lo_PutPalette   (int i,int c,const rgba_t* pal)
{
  if (check (i) && check (i+c-1))
  {
    memcpy (mPalette+i,pal,c*sizeof (rgba_t));
  }
}

void        fScreen8bit::lo_GetPalette   (int i,int c,rgba_t* pal)
{
  if (check (i) && check (i+c-1))
  {
    memcpy (pal,mPalette+i,c*sizeof (rgba_t));
  }
}

void        fScreen8bit::lo_hline(int x,int y,int w,color_t color)
{
  memset (mData+x+y*mWidth,color,w);
}

void        fScreen8bit::lo_bar(int xs,int ys,int xe,int ye,long color)
{
  int l;

  uchar* ptr=mData+xs+ys*mWidth;

  if((l=xe-xs+1)==mWidth)  memset(ptr,color,mWidth*(ye-ys+1));
  else                     for(;ys++<=ye;ptr+=mWidth) memset(ptr,color,l);
}

// other
long fScreen8bit::rgb(uchar red,uchar green,uchar blue)
{
#define COLORLEN(c,r,g,b)       (sqr(long(c.red)-long(r))+sqr(long(c.green)-long(g))+sqr(long(c.blue)-long(b)))
        int imin  = 0;
        long lmin = COLORLEN(mPalette [0],red,green,blue),l;
        for(int i=1;i<256;i++)
        {
          if((l=COLORLEN(mPalette [i],red,green,blue))<lmin)
          {
            lmin=l;
            imin=i;
          }
        }
#undef COLORLEN
        return imin;
}

void fScreen8bit::rgb(color_t color,uchar& red,uchar& green,uchar& blue)
{
  if (check (color))
  {
    red   = mPalette [color].red;
    green = mPalette [color].green;
    blue  = mPalette [color].blue;   
  }
}

fScreen8bit::fScreen8bit   (int w,int h,void* ptr)
            : FlatScreen<uchar> (w,h,ptr) 
{
  uchar rgIntensity [8] = {0,32,64,128,160,192,224,255};
  uchar bIntensity  [4] = {0,64,128,255};

  for (int i=0;i<256;i++)
  {
    mPalette [i].alpha = 0;
    mPalette [i].red   = rgIntensity [i & 7];
    mPalette [i].green = rgIntensity [(i >> 3) & 7];
    mPalette [i].blue  = bIntensity [i >> 6];
  }
}

static fScreen32bit    bug (0,0,NULL);
static FlatScreen<ulong>  bug1 (0,0,NULL);