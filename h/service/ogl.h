#ifndef __KERNEL_OGL_SERVICE__
#define __KERNEL_OGL_SERVICE__

#include <window\window.h>
#include <kernel\service.h>

#define         OGL_CONTEXT_NUM         4
#define         OGL_CONTEXT0            0
#define         OGL_CONTEXT1            1
#define         OGL_CONTEXT2            2
#define         OGL_CONTEXT3            3

#define         OGL_RGB                 0
#define         OGL_RGBA                OGL_RGB
#define         OGL_INDEX               1
#define         OGL_DOUBLE              2

#define         OGL_DRAW_WINDOW         4
#define         OGL_DRAW_BITMAP         8

#define         OGL_ACCUM               16
#define         OGL_ALPHA               32
#define         OGL_DEPTH24             64
#define         OGL_STENCIL             128
#define         OGL_AUX                 256
#define         OGL_DEPTH16             512
#define         OGL_DEPTH               OGL_DEPTH16

#define         OGL_COLOR16             1024
#define         OGL_COLOR24             2048
#define         OGL_COLOR32             4096

#define         OGL_STEREO              8192

#define         OGL_RESHAPE_CALL        1
#define         OGL_DRAW_CALL           2

typedef void    (*ReshapeFn)(int,int);
typedef void    (*DrawFn)();
typedef void    (*ChoosePFFn)(HDC,uint mode = OGL_DEPTH | OGL_COLOR16 | OGL_DRAW_WINDOW);

////////////////////////////////////////////////////////////////////////////////
///Поддержка OpenGL
////////////////////////////////////////////////////////////////////////////////
class  OGLService: public Service
{
  friend BOOL   OGL_CreateContext (int,Window*,ChoosePFFn,Thread*);
  friend BOOL   OGL_CreateContext (int,HDC,ChoosePFFn,Thread*);

  public:
                OGLService  ();
       virtual  ~OGLService ();

    virtual   const char*       ServiceName () const { return "OpenGL service"; }

////////////////////////////////////////////////////////////////////////////////
///Создание / копирование / удаление контекста
////////////////////////////////////////////////////////////////////////////////
                BOOL        CreateContext      (int context,Window*,ChoosePFFn = ChoosePF);
                BOOL        CreateContext      (int context,HDC,ChoosePFFn  = ChoosePF);
                BOOL        DuplicateContext   (int src,int dest);
                BOOL        DeleteContext      (int context);

////////////////////////////////////////////////////////////////////////////////
///Инициализация
////////////////////////////////////////////////////////////////////////////////
                void        DisplayMode        (uint);
                void        Reshape            (ReshapeFn);       
                void        Draw               (DrawFn);
                void        Start              (uint);       

////////////////////////////////////////////////////////////////////////////////
///Выбор текущего контекста
////////////////////////////////////////////////////////////////////////////////
                BOOL        MakeCurrentContext (int context);

////////////////////////////////////////////////////////////////////////////////
///Выброс на экран
////////////////////////////////////////////////////////////////////////////////
                void        SwapBuffers        ();

////////////////////////////////////////////////////////////////////////////////
///Работа с расширениями
////////////////////////////////////////////////////////////////////////////////
                BOOL        IsExtPresent       (const char*);       

  protected:
       static   int         handler            (Window*,UINT,WPARAM,LPARAM,DWORD); 
       static   void        ChoosePF           (HDC,uint mode);

  protected:
               HGLRC        mContext [OGL_CONTEXT_NUM];
               Window*      mWindow  [OGL_CONTEXT_NUM];
               HDC          mHDC     [OGL_CONTEXT_NUM];
               ReshapeFn    mReshape;
               DrawFn       mDraw;        
               int          mCurContext;
               uint         mDisplayMode;
};

#endif