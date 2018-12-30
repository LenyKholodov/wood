#ifndef __FLAT_SCREEN_H__
#define __FLAT_SCREEN_H__

#include <screen\mscreen.h>

////////////////////////////////////////////////////////////////////////////////
///Экран с указателем на буфер
////////////////////////////////////////////////////////////////////////////////
template <class T> class FlatScreen: public memScreen
{
  public:
                        FlatScreen  (int w,int h,void* ptr);

////////////////////////////////////////////////////////////////////////////////
///Характеристика
////////////////////////////////////////////////////////////////////////////////
    virtual void*       ptr         () const  { return mData;   }
    virtual bool        IsValid     () const;                   
    static  int         PtrSize     ()        { return sizeof (T); }

  public:   
            void*       operator new (size_t,void* x) { return x; }    
            void*       operator new (size_t size)    { return ::operator new (size); }

  protected:
    virtual void        lo_PutPixel (int x,int y,color_t);
    virtual long        lo_GetPixel (int x,int y);
    virtual void        lo_PutSprite(int x,int y,const sprite_t*,const crect_t&);
    virtual void        lo_vline    (int x,int y,int h,color_t);
    virtual void        lo_line     (int x0,int y0,int x1,int y1,color_t);

    virtual void        lo_PutImage (int x,int y,const image_t*,const crect_t&);
    virtual void        lo_GetImage (int x,int y,int w,int h,image_t*,const crect_t&);

  protected:
        T*              mData;
};

class fScreen32bit:public FlatScreen<ulong>
{
  public:
                 fScreen32bit (int w,int h,void* ptr):FlatScreen<ulong> (w,h,ptr) {}

  protected:
    virtual void lo_hline     (int x,int y,int w,color_t);
    virtual void lo_bar       (int left,int top,int right,int bottom,color_t);

  public:
    virtual color_t rgb       (uchar red,uchar green,uchar blue);
    virtual void    rgb       (color_t,uchar&,uchar&,uchar&);            
};

class fScreen8bit: public FlatScreen<uchar>
{
  public:
                        fScreen8bit     (int w,int h,void* ptr);

    virtual color_t     rgb             (uchar red,uchar green,uchar blue);
    virtual void        rgb             (color_t,uchar&,uchar&,uchar&);

////////////////////////////////////////////////////////////////////////////////
///Обновление только на экраны 32 bit
////////////////////////////////////////////////////////////////////////////////
    virtual void        refresh         ();

  protected:
    virtual void        lo_PutPalette   (int,int,const rgba_t*);
    virtual void        lo_GetPalette   (int,int,rgba_t*);

    virtual void        lo_hline        (int x,int y,int w,color_t);
    virtual void        lo_bar          (int left,int top,int right,int bottom,color_t);

  private:
    inline  bool        check           (int color) { return color >= 0 && color < 256; }

  private:
        rgba_t          mPalette [256];
};
  
#endif