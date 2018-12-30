#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <screen\grtypes.h>

class Window;
class memScreen;

////////////////////////////////////////////////////////////////////////////////
///Экран
////////////////////////////////////////////////////////////////////////////////
class Screen
{
  friend class memScreen;
  public:
                        Screen   ();
                        Screen   (int width,int height,int bpp);
      virtual           ~Screen  ();

////////////////////////////////////////////////////////////////////////////////
///Создание экранов
////////////////////////////////////////////////////////////////////////////////
    static  memScreen*    create   (Window*);
    static  Screen*       create   (Window*,const crect_t&);
    static  memScreen*    create   (Screen*,int bpp = 32); //mscreen
//    static  Screen        create   (Screen*,const crect_t&); //ascreen

////////////////////////////////////////////////////////////////////////////////
///Характеристики
////////////////////////////////////////////////////////////////////////////////
    virtual bool          IsValid () const;
    inline  int           width   () const { return mWidth;    }
    inline  int           height  () const { return mHeight;   }
    inline  int           bpp     () const { return mBpp;      }

    inline  int           getmaxx () const { return mWidth-1;  }
    inline  int           getmaxy () const { return mHeight-1; }

    inline  const crect_t& view    () const              { return mClipRect;        }
    inline  void          view    (const crect_t& w)     { mClipRect=w; correct (); }

  public:
////////////////////////////////////////////////////////////////////////////////
///Примитивы
////////////////////////////////////////////////////////////////////////////////
    inline  void          PutPixel        (int x,int y,color_t);
    inline  long          GetPixel        (int x,int y);

            void          vline           (int x,int y,int h,color_t);
            void          hline           (int x,int y,int w,color_t);
            void          line            (int x0,int y0,int x1,int y1,color_t);
            void          bar             (int left,int top,int right,int bottom,color_t);

            void          rectangle       (int xs,int ys,int xe,int ye,color_t);
            void          line            (int xs,int ys,int xe,int ye,color_t,long lineStyle);
            void          polyline        (int count,const point_t* p,color_t);
            void          circle          (int xc,int yc,int r,color_t);
            void          ellipse         (int xc,int yc,int a,int b,color_t);
            void          polygone        (int count,const point_t* p,color_t);
            void          triangle        (const point_t*,color_t);
    inline  void          triangle        (int x1,int y1,int x2,int y2,int x3,int y3,color_t color);

            void          fill            (int x,int y,color_t fillColor,color_t border);
    inline  void          cls             (color_t = 0);

    inline  void          PutImage        (int x,int y,const image_t*);
    inline  void          GetImage        (int x,int y,int w,int h,image_t*);
    inline  void          PutSprite       (int x,int y,const sprite_t*);

    virtual color_t       rgb             (uchar red,uchar green,uchar blue);
    virtual void          rgb             (color_t color,uchar& red,uchar& green,uchar& blue);

////////////////////////////////////////////////////////////////////////////////
///Работа с палитрой (эмуляция для 32bit)
////////////////////////////////////////////////////////////////////////////////
    inline  void          PutPalette      (const rgba_t*,int index = 0,int count = 1);
    inline  void          GetPalette      (rgba_t*,int index = 0,int count = 1);

  protected:
    virtual void          correct         ();

  protected:
////////////////////////////////////////////////////////////////////////////////
///Виртуальные примитивы
////////////////////////////////////////////////////////////////////////////////
    virtual void          lo_PutPixel     (int x,int y,color_t color);
    virtual long          lo_GetPixel     (int x,int y);
    virtual void          lo_vline        (int x,int y,int h,color_t color);
    virtual void          lo_hline        (int x,int y,int w,color_t color);
    virtual void          lo_line         (int x0,int y0,int x1,int y1,color_t color);
    virtual void          lo_bar          (int left,int top,int right,int bottom,color_t color);

    virtual void          lo_PutImage     (int x,int y,const image_t* image,const crect_t& rect);
    virtual void          lo_GetImage     (int x,int y,int w,int h,image_t* image,const crect_t& rect);
    virtual void          lo_PutSprite    (int x,int y,const sprite_t* image,const crect_t& rect);

    virtual void          lo_PutPalette   (int,int,const rgba_t*) {}
    virtual void          lo_GetPalette   (int,int,rgba_t*) {}

  private:
            int           LineFill        (int,int,int,int,int);

  protected:
        int     mWidth,mHeight,mBpp;
        crect_t  mClipRect;

  private:
        int     mColor, mFillColor;
};

////////////////////////Inline methods/////////////////////////////////////////

inline bool    IsValid (Screen* screen)
{
  return screen && screen->IsValid ();
}

inline void    Screen::PutPixel (int x,int y,color_t color)
{ 
  if (mClipRect.in(x,y))   
    lo_PutPixel(x,y,color); 
}

inline color_t Screen::GetPixel(int x,int y)
{ 
  return mClipRect.in(x,y) ? lo_GetPixel (x,y) : 0; 
}

inline void    Screen::PutImage(int x,int y,const image_t* image)
{ 
  if (IsValid()) 
    lo_PutImage(x,y,image,mClipRect); 
}

inline void    Screen::GetImage(int x,int y,int w,int h,image_t* image)
{ 
  if (IsValid()) 
    lo_GetImage(x,y,w,h,image,mClipRect); 
}

inline void    Screen::PutSprite(int x,int y,const sprite_t* image)
{ 
  if (IsValid()) 
    lo_PutSprite(x,y,image,mClipRect); 
}

inline void    Screen::triangle (int x1,int y1,int x2,int y2,int x3,int y3,color_t color)
{ 
  point_t p[3] = { {x1,y1}, {x2,y2}, {x3,y3} };
  triangle (p,color); 
}

inline void    Screen::cls(color_t fill)
{ 
  if (IsValid()) 
    lo_bar (mClipRect.left,mClipRect.top,mClipRect.right,mClipRect.bottom,fill); 
}

inline void    Screen::PutPalette      (const rgba_t* pal,int index,int count)
{
  if (IsValid ())
    lo_PutPalette (index,count,pal);
}

inline void    Screen::GetPalette      (rgba_t* pal,int index,int count)
{
  if (IsValid ())
    lo_GetPalette (index,count,pal);
}

#ifdef DEBUG
  #pragma comment (lib,"screend.lib")
#else
  #pragma comment (lib,"screen.lib")
#endif

#endif
