#include <pch.h>
#include <screen\screen.h>
#include <stdlib.h>
#include <misc.h>
#include <debug.h>

void Screen::correct (void)
{
  if(mClipRect.left   <  0)        mClipRect.left   = 0;
  if(mClipRect.right  >= mWidth)   mClipRect.right  = mWidth-1;
  if(mClipRect.top    <  0)        mClipRect.top    = 0;
  if(mClipRect.bottom >= mHeight)  mClipRect.bottom = mHeight-1;
}

void Screen::rgb(color_t,uchar&,uchar&,uchar&) { }
bool Screen::IsValid(void) const        { return !mClipRect.empty(); }
void Screen::lo_PutPixel(int,int,long)  {  }
long Screen::lo_GetPixel(int,int)       { return 0; }
void Screen::lo_vline(int x,int y,int h,color_t color)
{
  for (int ye=y+h;y<ye;lo_PutPixel(x,y++,color));
}

void Screen::lo_hline(int x,int y,int w,color_t color)
{
 for(int xe=x+w;x<xe;lo_PutPixel(x++,y,color));
}

void Screen::lo_line(int xs,int ys,int xe,int ye,color_t color)
{
// rem: xs<=xe
  lo_PutPixel(xs,ys,color);
  int dx=xe-xs;
  int dy=abs(ye-ys);
  if(dy<=dx)
  {
    int d,d1,d2=(dy-dx)<<1;
    d=(d1=dy<<1)-dx;
    if(ye>ys)
      for(;dx--;lo_PutPixel(++xs,ys,color))
      {
        if(d>0)
        {
          d+=d2;
          ys++;
        }
        else
          d+=d1;
      }
    else
      for(;dx--;lo_PutPixel(++xs,ys,color))
      {
              if(d>0)
        {
          d+=d2;
          ys--;
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
      for(;dy--;lo_PutPixel(xs,++ys,color))
      {
        if(d>0)
        {
          d+=d2;
          xs++;
        }
        else
          d+=d1;
      }
    else
      for(;dy--;lo_PutPixel(xs,--ys,color))
      {
        if(d>0)
        {
          d+=d2;
          xs++;
        }
        else
          d+=d1;
      }
  }
}

void Screen::lo_bar(int left,int top,int right,int bottom,color_t color)
{
  for(int w=right-left+1;top<=bottom;lo_hline(left,top++,w,color));
}

void Screen::vline(int x,int y,int h,color_t color)
{
  if(mClipRect.inX(x))
  {
    if(h<0)                      y -= h = -h;
    if(y+h>mClipRect.bottom+1)   h  = mClipRect.bottom+1-y;
    if(y<mClipRect.top)
    {
      h += y-mClipRect.top;
      y  = mClipRect.top;
    }

    if(h>0) lo_vline (x,y,h,color);
  }
}

void Screen::hline(int x,int y,int w,color_t color)
{
          if(!mClipRect.inY(y))
                return;
        if(w<0)
                x-=w=-w;
        if(x+w>mClipRect.right+1)
                w=mClipRect.right+1-x;
        if(x<mClipRect.left)
        {
                w+=x-mClipRect.left;
                x=mClipRect.left;
        }
        if(w>0)
                lo_hline(x,y,w,color);
}

void Screen::line(int xs,int ys,int xe,int ye,color_t color)
{
        if(xs>xe)
        {
                swap(xs,xe);
                swap(ys,ye);
        }
        if(xs>mClipRect.right || xe<mClipRect.left)
                return;
        if(xs==xe)
        {
                vline(xs,ys,ye-ys+1,color);
                return;
        }
        if(xs<mClipRect.left)
        {
                ys+=(long)(ye-ys)*(mClipRect.left-xs)/(xe-xs);
                if((xs=mClipRect.left)==xe)
                {
                        vline(xs,ys,ye-ys+1,color);
                        return;
                }
        }
        if(xe>mClipRect.right)
        {
                ye+=(long)(ye-ys)*(mClipRect.right-xe)/(xe-xs);
                if((xe=mClipRect.right)==xs)
                {
                        vline(xs,ys,ye-ys+1,color);
                        return;
                }

        }
        if(ys==ye)
        {
                hline(xs,ys,xe-xs+1,color);
                return;
        }
        if(ys<mClipRect.top)
        {
                if(ye<mClipRect.top)
                        return;
                xs+=(long)(xe-xs)*(mClipRect.top-ys)/(ye-ys);
                if(xs>xe)
                        return;
                if((ys=mClipRect.top)==ye)
                {
                        lo_hline(xs,ys,xe-xs+1,color);
                        return;
                }
        }
        else
                if(ys>mClipRect.bottom)
                {
                        if(ye>mClipRect.bottom)
                                return;
                        xs+=(long)(xe-xs)*(mClipRect.bottom-ys)/(ye-ys);
                        if(xs>xe)
                                return;
                        if((ys=mClipRect.bottom)==ye)
                        {
                                lo_hline(xs,ys,xe-xs+1,color);
                                return;
                        }
                }
        if(ye<mClipRect.top)
        {
                xe+=(long)(xe-xs)*(mClipRect.top-ye)/(ye-ys);
                if((ye=mClipRect.top)==ys)
                {
                        lo_hline(xs,ys,xe-xs+1,color);
                        return;
                }
        }
        else
                if(ye>mClipRect.bottom)
                {
                        xe+=(long)(xe-xs)*(mClipRect.bottom-ye)/(ye-ys);
                        if((ye=mClipRect.bottom)==ys)
                        {
                                lo_hline(xs,ys,xe-xs+1,color);
                                return;
                        }
                }
        lo_line(xs,ys,xe,ye,color);
}

void Screen::bar(int xs,int ys,int xe,int ye,color_t color)
{
        if(xs>xe)
                swap(xs,xe);
        if(ys>ye)
                swap(ys,ye);
        if(xs<mClipRect.left)
                xs=mClipRect.left;
        if(xe>mClipRect.right)
                xe=mClipRect.right;
        if(ys<mClipRect.top)
                ys=mClipRect.top;
        if(ye>mClipRect.bottom)
                ye=mClipRect.bottom;
        if(xs>xe || ys>ye)
                 return;
        lo_bar(xs,ys,xe,ye,color);
}

void Screen::rectangle(int xs,int ys,int xe,int ye,color_t color)
{
        if(xs>xe)
                swap(xs,xe);
        if(ys>ye)
                swap(ys,ye);
        hline(xs,ys,xe-xs+1,color);
        hline(xs,ye,xe-xs+1,color);
        vline(xs,ys+1,ye-ys-1,color);
        vline(xe,ys+1,ye-ys-1,color);
}

/// Графические примитивы ////////////////////////////////
void Screen::lo_PutImage(int x,int y,const image_t* image,const crect_t& rect)
{
        int bpp=mBpp>>3, // Bytes per pixel
            wb=image->width*bpp;
        const uchar* data=image->data;
        int h=image->height,w=image->width;
        if(y<rect.top)
        {
                if((h-=rect.top-y)<=0)
                        return;
                data+=wb*(rect.top-y);
                y=rect.top;
        }
        if(y+h>rect.bottom+1)
                h=rect.bottom-y+1;
        if(x+w>rect.right+1)
                if((w=rect.right-x+1)<=0)
                        return;
        int i,j,stx;
        if(x<rect.left)
                data+=(stx=rect.left-x)*bpp;
        else
                stx=0;
        for(i=0;i++<h;y++,data+=wb)
        {
                const uchar* ldata=data;
                for(int xl=x+(j=stx);j<w;j++,ldata+=bpp)
                        lo_PutPixel(xl++,y,*((long*)ldata));
        }
}

void Screen::lo_GetImage(int x,int y,int w,int h,image_t* image,const crect_t& rect)
{
        image->width=w;
        image->height=h;
        if(x>rect.right || y>rect.bottom)
                return;
        switch(mBpp)
        {
                case 32:
                {
                        ulong* data=(ulong*)image->data;
                        if(y<rect.top)
                        {
                                if((h-=rect.top-y)<=0)
                                        return;
                                data+=(rect.top-y)*w;
                                y=rect.top;
                        }
                        if(y+h>rect.bottom)
                                h=rect.bottom-y+1;
                        if(x+w>rect.right)
                                w=rect.right-x+1;
                        int stx=x<rect.left?rect.left-x:0;
                        for(int i=0;i<h;i++,y++,data+=w)
                                for(int j=stx;j<w;j++)
                                        data[j]=lo_GetPixel(x+j,y);
                }
                break;
        }
}

void Screen::lo_PutSprite(int x,int y,const sprite_t* image,const crect_t& rect)
{
  if((x-=image->x)>rect.right)
          return;  

  y-=image->y;

  sprite_t::strip_t* strip = (sprite_t::strip_t*)image->data;
  static masks [5]         = {0,0xFF000000,0xFFFF0000,0xFFFFFF00,0xFFFFFFFF};

//  ulong    shift, mask, s;
  ulong    bpp    = image->bpp >> 3,
           shift  = bpp >> 1,
           s      = (4-bpp) << 3,
           mask   = masks [bpp];
  int      swidth = image->width;

  //нет отсечения по левому краю

/*  switch (image->bpp)
  {
    case 8:  s = 24; shift = 0; bpp = 1; mask = 0xFF000000;   break;
    case 15:
    case 16: s = 16; shift = 1; bpp = 2; mask = 0xFFFF0000; break;
    case 24: s = 8;
    case 32: shift = 2; bpp = 4; mask = 0xFFFFFFFF; break;
    default: return;   
  }*/

  for(int h=image->height;y<rect.top;y++)
  {
    for (int x=0;x<swidth;x+=(strip->offs+strip->len)>>shift,strip=strip->next ());

    if(--h<=0)
        return;
  }

  if (y+h>rect.bottom)
    if ((h=rect.bottom-y+1)<=0)
       return;

  int w;
  if (x+(w=(uint)image->width)<rect.left)
      return;
  if (x+w>rect.right)
    if((w=rect.right-x+1)<=0)
      return;

  for (;h>0;h--,y++)
  {
    uint xl = x;

    for (int j=0;j<w;j+=strip->offs>>shift,xl+=strip->offs>>shift,strip=strip->next ())
    {
      int    len   = strip->len >> shift;
      uchar* data  = strip->data;
      len          = j + len < w ? len : w - j;

      for (int i=0;i<len;i++,data+=bpp)
        lo_PutPixel (xl+i,y,((*(ulong*)data) & mask)>>s); 

      xl          += strip->len >> shift;
      j           += strip->len >> shift;
    }    

    for (;j<swidth;j+=(strip->len+strip->offs)>>shift,strip=strip->next ());
  }
}

long Screen::rgb(uchar,uchar,uchar)
{
        return 0;
}

////////////////////////////////////////////////////////////////////////

void Screen::circle(int xc,int yc,int r,color_t color)
{
        int y=r,
            d=1-r,
            delta1=3,
            delta2=-2*r+5;
        for(int x=0;x<=y;x++)
        {
                 PutPixel(x+xc,y+yc,color);
                 PutPixel(-x+xc,y+yc,color);
                 PutPixel(x+xc,-y+yc,color);
                 PutPixel(-x+xc,-y+yc,color);
                 PutPixel(y+xc,x+yc,color);
                 PutPixel(-y+xc,x+yc,color);
                 PutPixel(y+xc,-x+yc,color);
                 PutPixel(-y+xc,-x+yc,color);
                  if(d<0)
                  {
                           d+=delta1;
                           delta1+=2;
                           delta2+=2;
                  }
                  else
                  {
                           d+=delta2;
                           delta1+=2;
                           delta2+=4;
                           y--;
                  }
         }
}

void Screen::ellipse(int x,int y,int a,int b,color_t color)
{
  int lx=0,ly=b;
  long aa=long(a)*long(a), aa2=2*aa, bb=long(b)*long(b) ,bb2=bb*2;
  long d=bb-aa*long(b)+aa/4, dx=0, dy=aa2*long(b);
  PutPixel(x,y-ly,color);
  PutPixel(x,y+ly,color);
  PutPixel(x-a,y,color);
  PutPixel(x+a,y,color);
  while (dx<dy)
 {
  if (d>0)
  {
   --ly;
   dy-=aa2;
   d-=dy;
  }
  ++lx;
  dx+=bb2;
  d+=bb + dx;
  PutPixel(x+lx,y+ly,color);
  PutPixel(x-lx,y+ly,color);
  PutPixel(x+lx,y-ly,color);
  PutPixel(x-lx,y-ly,color);
 }
 d+=((3*(aa-bb)/2-(dx+dy))/2);
 while (ly > 0)
 {
  if (d < 0)
  {
   ++lx;
   dx+=bb2;
   d+=bb+dx;
  }
  --ly;
  dy-=aa2;
  d+=aa-dy;
  PutPixel(x+lx,y+ly,color);
  PutPixel(x-lx,y+ly,color);
  PutPixel(x+lx,y-ly,color);
  PutPixel(x-lx,y-ly,color);
 }
}

inline long fraction(int a,int b) { return (a<<16)/b; }
inline long frac0(int a,int b) { return b?fraction(a,b):0; }

void Screen::triangle(const point_t* p,color_t color)
{
  int iMax=0,iMin=0,iMid=0,i;
  if(p[1].y<p[iMin].y)
    iMin=1;
  else
    if(p[1].y>p[iMax].y)
      iMax=1;
    if(p[2].y<p[iMin].y)
      iMin=2;
    else
      if(p[2].y>p[iMax].y)
        iMax=2;
  iMid=3-iMin-iMax;
  long dx1,dx2,x1,x2;
  x1=x2=p[iMin].x<<16;
  int ym=p[iMid].y;

  dx1=frac0(p[iMax].x-p[iMin].x,p[iMax].y-p[iMin].y);
  dx2=frac0(p[iMin].x-p[iMid].x,p[iMin].y-p[iMid].y);
  for(i=p[iMin].y;i<=ym;i++)
  {
     hline(x1>>16,i,((x2-x1)>>16)+1,color);
     x1+=dx1;
     x2+=dx2;
  }
  int dyxn;
  if((dyxn=p[iMax].y-p[iMid].y)!=0)
  {
    dx2=fraction(p[iMax].x-p[iMid].x,dyxn);
    for(ym=p[iMax].y;i<=ym;i++)
    {
      x1+=dx1;
      x2+=dx2;
      hline(x1>>16,i,((x2-x1)>>16)+1,color);
    }
  }
}

void Screen::fill(int x,int y,long fillColor,color_t color)
{
        if(!mClipRect.in(x,y))
                return;
        mFillColor=fillColor;
        mColor          =color;
        LineFill(x,y,1,x,x);
}

int Screen::LineFill(int x,int y,int dir,int PrevXL,int PrevXR)
{
        long c;
        for(int xl=x;--xl>=mClipRect.left;)
                 if((c=lo_GetPixel(xl,y))==mColor || c==mFillColor)
                         break;
        for(int xr=x;++xr<=mClipRect.right;)
                 if((c=lo_GetPixel(xr,y))==mColor || c==mFillColor)
                         break;
        --xr;
         ++xl;
        lo_hline(xl,y,xr-xl+1,mFillColor);
        if(mClipRect.inY(y+dir))
                  for(x=xl;x<=xr;x++)
                           if((c=lo_GetPixel(x,y+dir))!=mColor && c!=mFillColor)
                                         x=LineFill(x,y+dir,dir,xl,xr);
         if(mClipRect.inY(y-dir))
        {
                  for(x=xl;x<=PrevXL;x++)
                           if((c=lo_GetPixel(x,y-dir))!=mColor && c!=mFillColor)
                                      x=LineFill(x,y-dir,-dir,xl,xr);
                  for (x=PrevXR;x<=xr;x++)
                            if((c=lo_GetPixel(x,y-dir))!=mColor && c!=mFillColor)
                                     x=LineFill(x,y-dir,-dir,xl,xr);
        }
         return xl;
}

void Screen::polyline(int count,const point_t* p,color_t color)
{
        for(int i=1;i<count;i++)
                line(p[i-1].x,p[i-1].y,p[i].x,p[i].y,color);
}

void Screen::polygone(int n,const point_t* p,color_t color)
{
  int yMin,yMax,topPointIndex,i=1,pTop;
  for(pTop=yMin=yMax=p[topPointIndex=0].y;i<n;i++)
  {
    int piy=p[i].y;
    if(piy<pTop)
      pTop=p[topPointIndex=i].y;
    else
      if(piy>yMax)
        yMax=piy;
  }
  if((yMin=pTop)==yMax)
  {
    int xMin,xMax;
    xMin=xMax=p[0].x;
    for(i=1;i<n;i++)
    {
      int pix=p[i].x;
      if(pix<xMin)
        xMin=pix;
      else
        if(pix>xMax)
          xMax=pix;
    }
    hline(xMin,yMin,xMax-xMin+1,color);
    return;
  }
  int i1Next,i2Next;
  for(int i1=topPointIndex;;)
  {
    if(p[i1Next=i1<1?n-1:i1-1].y<p[i1].y)
      return;
    else
      if(p[i1Next].y==p[i1].y)
        i1=i1Next;
      else
        break;
  }
  for(int i2=topPointIndex;;)
  {
    if(p[i2Next=(i2+1)%n].y<p[i2].y)
      return;
    else
      if(p[i2Next].y==p[i2].y)
        i2=i2Next;
      else
        break;
  }
  int y1=p[i1Next].y,y2=p[i2Next].y;
  long x1=p[i1].x<<16,
       x2=p[i2].x<<16,
       dx1=fraction(p[i1Next].x-p[i1].x,y1-p[i1].y),
       dx2=fraction(p[i2Next].x-p[i2].x,y2-p[i2].y);
  for(int y=yMin;y<=yMax;)
  {
    hline(x1>>16,y,((x2-x1)>>16)+1,color);
    x1+=dx1;
    x2+=dx2;
    y++;
    if(y==y1)
    {
      i1=i1Next;
      if(--i1Next<0)
        i1Next=n-1;
      if(p[i1].y==(y1=p[i1Next].y))
        break;
      dx1=fraction(p[i1Next].x-p[i1].x,y1-p[i1].y);
    }
    if(y==y2)
    {
      i2=i2Next;
      if(p[i2].y==(y2=p[i2Next=(i2Next+1)%n].y))
        break;
      dx2=fraction(p[i2Next].x-p[i2].x,y2-p[i2].y);
    }
  }
}

void Screen::line(int xs,int ys,int xe,int ye,color_t color,long lineStyle)
{
        if(xs>xe)
        {
                swap(xs,xe);
                swap(ys,ye);
        }
        if(xs>mClipRect.right || xe<mClipRect.left)
                return;
        if(xs<mClipRect.left)
        {
                if(xs==xe)
                        return;
                ys+=(long)(ye-ys)*(mClipRect.left-xs)/(xe-xs);
                xs=mClipRect.left;
        }
        if(xe>mClipRect.right)
        {
                if(xs==xe)
                        return;
                ye+=(long)(ye-ys)*(mClipRect.right-xe)/(xe-xs);
                xe=mClipRect.right;
        }
        if(ys<mClipRect.top)
        {
                if(ye<mClipRect.top)
                        return;
                xs+=(long)(xe-xs)*(mClipRect.top-ys)/(ye-ys);
                if(xs>xe)
                        return;
                ys=mClipRect.top;
        }
        else
                if(ys>mClipRect.bottom)
                {
                        if(ye>mClipRect.bottom)
                                return;
                        xs+=(long)(xe-xs)*(mClipRect.bottom-ys)/(ye-ys);
                        if(xs>xe)
                                return;
                        ys=mClipRect.bottom;
                }
        if(ye<mClipRect.top)
        {
                xe+=(long)(xe-xs)*(mClipRect.top-ye)/(ye-ys);
                ye=mClipRect.top;
        }
        else
                if(ye>mClipRect.bottom)
                {
                        xe+=(long)(xe-xs)*(mClipRect.bottom-ye)/(ye-ys);
                        ye=mClipRect.bottom;
                }
#define MASK_PIXEL(x,y)        if(lineStyle&(1L<<(step++&0x1F))) lo_PutPixel(x,y,color)
// rem: xs<=xe
           int step=0;
          MASK_PIXEL(xs,ys);
          int dx=xe-xs;
          int dy=abs(ye-ys);
          if(dy<=dx)
          {
                    int d,d1,d2=(dy-dx)<<1;
                    d=(d1=dy<<1)-dx;
                    if(ye>ys)
                              for(;dx--;++xs)
                              {
                                      MASK_PIXEL(xs,ys);
                                if(d>0)
                                {
                                          d+=d2;
                                          ys++;
                                }
                                else
                                          d+=d1;
                              }
                    else
                              for(;dx--;++xs)
                              {
                                      MASK_PIXEL(xs,ys);
                                      if(d>0)
                                {
                                          d+=d2;
                                          ys--;
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
                              for(;dy--;++ys)
                              {
                                      MASK_PIXEL(xs,ys);
                                if(d>0)
                                {
                                          d+=d2;
                                          xs++;
                                }
                                else
                                          d+=d1;
                              }
                    else
                              for(;dy--;--ys)
                              {
                                      MASK_PIXEL(xs,ys);
                                if(d>0)
                                {
                                          d+=d2;
                                          xs++;
                                }
                                else
                                          d+=d1;
                              }
          }
#undef MASK_PIXEL
}

Screen::Screen   ()
       : mWidth (0), mHeight (0), mBpp (0),
         mClipRect (0,0,0,0)
{   }

Screen::Screen   (int width,int height,int bpp)
       : mWidth    (width), mHeight (height), mBpp (bpp),
         mClipRect (0,0,width-1,height-1)
{  }

Screen::~Screen ()
{    }
