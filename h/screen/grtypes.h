#ifndef __GR_TYPES_H__
#define __GR_TYPES_H__

#include <types.h>
#include <binfile.h>

#ifndef __RGB_STRUCT_DEFINE__
#define __RGB_STRUCT_DEFINE__

struct  rgb_t
{
  uchar  red;
  uchar  green;
  uchar  blue;
};

struct  rgba_t
{
  uchar blue;
  uchar green;
  uchar red;
  uchar alpha;
};

#endif


struct crect_t
{
  public:
        int left,top,right,bottom;

                crect_t  ()        {}
                crect_t  (int l,int t,int r,int b) { setup(l,t,r,b); }
                crect_t  (int w,int h): left (0), top (0), right (w-1), bottom (h-1) {}

        BOOL    inX     (int x)       const { return x>=left && x<=right; }
        BOOL    inY     (int y)       const { return y>=top && y<=bottom; }
        BOOL    in      (int x,int y) const { return inX(x) && inY(y);    }
        BOOL    empty   (void)        const { return left>right || top>bottom; }

        crect_t& operator  = (const crect_t& r) { left=r.left; right=r.right; top=r.top; bottom=r.bottom; return *this; }
        crect_t& operator &= (const crect_t& r) { if(left<r.left)    left=r.left;
                                               if(right>r.right)  right=r.right;
                                               if(top<r.top)      top=r.top;
                                               if(bottom>r.bottom)bottom=r.bottom;
                                               return *this;
                                             }
        void    setup       (int l,int t,int r,int b) { left=l; right=r; top=t; bottom=b; }
        crect_t& add         (int x,int y)             { left+=x; right+=x; top+=y; bottom+=y; return *this; }
        int     width       (void)  const             { return right-left+1; }
        int     height      (void)  const             { return bottom-top+1; }
        void    dump        (ostream& os) const { os<<'('<<left<<','<<top<<")-("
                                                    <<right<<','<<bottom<<")\n"; 
                                                }

        inline friend ostream& operator<<(ostream& os,const crect_t& c)
               { c.dump(os); return os; }
};

typedef struct
{
  int x,y;
} point_t;

typedef long color_t;

////////////////////////////////////////////////////////////////////////////////
// Image
////////////////////////////////////////////////////////////////////////////////

class image_t
{
  friend class imagelist_t;
  private:
                image_t  ();
                image_t  (int,int,int);

                void* operator new       (size_t);
                void* operator new       (size_t,void* x) { return x; }
                void* operator new []    (size_t);
                void  operator delete [] (void*);

  public:
        int          width,height,bpp;
        uchar*       data;

        enum {
          TGA = 0,
          BMP,
          JPEG
        };

  public:
                ~image_t ();

           void     operator delete (void* x);

////////////////////////////////////////////////////////////////////////////////
///Создание картинки
////////////////////////////////////////////////////////////////////////////////
    static image_t* create  (int w,int h,int bpp);
    static image_t* create  (int w,int h,int bpp,void* data);
    static image_t* create  (BinFile*,int mode = BMP);
    static image_t* create  (const char*);
    static void     dispose (image_t*);

////////////////////////////////////////////////////////////////////////////////
///Сохранение картинки
////////////////////////////////////////////////////////////////////////////////
           bool      save    (BinFile*)    const;   
           bool      save    (const char*) const;   

////////////////////////////////////////////////////////////////////////////////
///Размеры
////////////////////////////////////////////////////////////////////////////////
    static size_t   size    (int w,int h,int bpp) { return w * h * (bpp >> 3) + sizeof (image_t); }
    inline size_t   size    () const { return width * height * (bpp >> 3)  + sizeof (image_t) ; }

  private:
        uchar  mData [];
};

////////////////////////////////////////////////////////////////////////////////
// Sprite
////////////////////////////////////////////////////////////////////////////////

class sprite_t
{
  private:
                sprite_t ();

                void* operator new       (size_t);
                void* operator new []    (size_t);
                void  operator delete [] (void*);

  public:
    int         x,y,width,height,bpp;
    uchar       data[];

                struct strip_t
                {
                  uint16        len;     //Длина цепи (в байтах)
                  uint16        offs;    //Длина пробела (в байтах)
                  uchar         data []; //Данные

                  strip_t*      next () const { return (strip_t*)((uchar*)this + len + sizeof (strip_t)); }
                };


                ~sprite_t ();

////////////////////////////////////////////////////////////////////////////////
///Создание спрайта
////////////////////////////////////////////////////////////////////////////////
    static sprite_t* create   (const image_t*,color_t zcolor = 0);
    static sprite_t* create   (const image_t*,const image_t*);
    static sprite_t* create   (BinFile*);
    static sprite_t* create   (const char*);
    static void      dispose  (sprite_t*);

////////////////////////////////////////////////////////////////////////////////
///Сохранение спрайта
////////////////////////////////////////////////////////////////////////////////
           bool      save     (BinFile*)    const;   
           bool      save     (const char*) const;   

           void      operator delete (void*);

////////////////////////////////////////////////////////////////////////////////
///Размеры спрайта
////////////////////////////////////////////////////////////////////////////////
           size_t    size     () const;
};

////////////////////////////////////////////////////////////////////////////////
///Упаковка картинок
////////////////////////////////////////////////////////////////////////////////
class  imagelist_t
{
  private:
                        imagelist_t ();

                void* operator new       (size_t);
                void* operator new []    (size_t);
                void  operator delete [] (void*);

  public:
        uint   count, bpp;
        uchar  data [];

                        ~imagelist_t ();

           void      operator delete (void*);

////////////////////////////////////////////////////////////////////////////////
///Создание
////////////////////////////////////////////////////////////////////////////////
    static imagelist_t* create   (BinFile*);
    static imagelist_t* create   (const char*);
    static imagelist_t* create   (int c,image_t** imgs);
    static imagelist_t* create   (int c,int bitpp,crect_t*);
    static void         dispose  (imagelist_t*);

////////////////////////////////////////////////////////////////////////////////
///Распаковка
////////////////////////////////////////////////////////////////////////////////
    inline image_t*     GetImage (int i) const  { return (i>=0 && i<count)?(image_t*)(data+((long*)data)[i]):NULL; }

////////////////////////////////////////////////////////////////////////////////
///Размер
////////////////////////////////////////////////////////////////////////////////
           size_t       size     ()      const;

////////////////////////////////////////////////////////////////////////////////
///Сохранение
////////////////////////////////////////////////////////////////////////////////
           bool         save     (BinFile*)     const;
           bool         save     (const char*)  const;
};

#endif
