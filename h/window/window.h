#ifndef __WINDOW__
#define __WINDOW__

#include <window\windefs.h>
#include <graph\grtypes.h>
#include <misc.h>

const    int            CLASS_NAME_LEN  = 128;

class    Window;

typedef  int      (*HandlerFn)(Window*,UINT,WPARAM,LPARAM,DWORD _UserParam);
//typedef  int      (*HandlerFn)(Window*,UINT,WORD,LONG,DWORD _UserParam);

struct   handler_t
{
  HandlerFn     fn;
  DWORD         param;
};

extern HINSTANCE       A_GetInstance ();

////////////////////////////////////////////////////////////////////////////////
///Оконная поддержка
////////////////////////////////////////////////////////////////////////////////
class   Window: public MemObject
{
  public:
                        Window  (Pool*);
                        Window  (Pool*,
                                 const char* _WndName,
                                 const char* _ClassName,
                                 DWORD       _Style,
                                 const rect_t&
                                );
                        Window  (Window*);
                        Window  (Window*,
                                 const char* _WndName,
                                 const char* _ClassName,
                                 DWORD       _Style,
                                 const rect_t&
                                );
           virtual      ~Window ();


////////////////////////////////////////////////////////////////////////////////
///Перехват сообщений
////////////////////////////////////////////////////////////////////////////////
          BOOL          InsertHandler   (UINT,HandlerFn,DWORD = 0);
          void          DeleteHandler   (UINT);

////////////////////////////////////////////////////////////////////////////////
///Поддержка
////////////////////////////////////////////////////////////////////////////////
   virtual void         Show            (int);
   virtual void         Draw            ();
   static  void         ShowCursor      (BOOL);

          HWND          wnd             ()     const { return mHWND; }
          HINSTANCE     instance        ()     const { return mInst; }
   static Window*       GetWindow       (HWND);
          HDC           GetDC           ();

////////////////////////////////////////////////////////////////////////////////
///Обработчик сообщений окна
////////////////////////////////////////////////////////////////////////////////
   virtual  LRESULT     handler         (UINT,WPARAM,LPARAM);

////////////////////////////////////////////////////////////////////////////////
///Рисование
////////////////////////////////////////////////////////////////////////////////
            void        PutPixel       (int,int,long);                
            long        GetPixel       (int,int);

            void        LineTo         (int,int);       
            void        Line           (int,int,int,int);

            void        FillRect       (int x,int y,int width,int height,HBRUSH = NULL);
            void        Cls            (HBRUSH = NULL);
            void        Rectangle      (int x,int y,int width,int height);

            void        MoveTo         (int x,int y);
            void        GetPos         (int& x,int& y);

            void        OutText        (int,int,const char*);

////////////////////////////////////////////////////////////////////////////////
///Изменение атрибутов окна
////////////////////////////////////////////////////////////////////////////////
            void        SelectObject   (void*);

            void        SetTextColor   (long);
            void        SetBkColor     (long);
            long        GetTextColor   ();
            long        GetBkColor     ();

            int         width          () const;
            int         height         () const;

////////////////////////////////////////////////////////////////////////////////
///Создание атрибутов окна
////////////////////////////////////////////////////////////////////////////////            
   static   HPEN        CreatePen      (const pen_t&);
   static   HBRUSH      CreateBrush    (const brush_t&);
   static   HFONT       CreateGDIFont  (const gdifont_t&);
   static   void        DeleteObject   (void*);

////////////////////////////////////////////////////////////////////////////////
///Двойная буферезация
////////////////////////////////////////////////////////////////////////////////
            void        BeginDraw      ();
            void        EndDraw        ();    

////////////////////////////////////////////////////////////////////////////////
///Распределение памяти
////////////////////////////////////////////////////////////////////////////////
            void*       operator new    (size_t size,Pool* = Memory::M_GetPool ());
            void        operator delete (void*);

  protected:    
   static   LONG WINAPI WndProc         (HWND,UINT,WPARAM,LPARAM);
   static   int         WndDestroy      (Window* wnd,UINT,WPARAM,LPARAM,DWORD);
            void        Destroy         (bool);

            BOOL        AddWindow       (const Window*);
            void        DelWindow       (const Window*);

            BOOL        create          (const char* _WndName,
                                         const char* _ClassName,
                                         DWORD       _Style,
                                         const rect_t&
                                        );

  protected:
                Window*                    mParent;
                HWND                       mHWND;
                HDC                        mHDC, mRealDC;
                HBITMAP                    mDoubleBuf, mOldBuf;
                HINSTANCE                  mInst;
                BOOL                       mDestroy;
                Dict<handler_t>            mHandDict;
                Ring<Window*>              mWindRing;
    static      Window*                    mFuck;

  public:
    static      Dict<Window*>*             mWindDict;
};

#ifdef DEBUG
  #pragma comment (lib,"winfaced.lib")
#else
  #pragma comment (lib,"winface.lib")
#endif

#endif