#define  DEBUG
#include <thread\app.h>
#include <thread\window.h>
#include <winuser.h>
#include <iostream.h>

class    Console:public Window
{
  public:
                        Console  (Window*,
                                  const rect_t&,
                                  const char* _WndName
                                 );
           virtual      ~Console ();

  protected:
           void         Init     (const rect_t&);

  private:
           char         mClassName [CLASS_NAME_LEN];     
};

class   Text: public Window
{
  public:
                        Text  (Window*,
                                  const rect_t&,
                                  const char* _WndName
                                 );
           virtual      ~Text ();

  protected:
           void         Init     (const rect_t&);

  private:
           char         mClassName [CLASS_NAME_LEN];     
};

#pragma pack(1)

typedef void (*OutTextFn)(Window*,int);

class consoleBuf: public streambuf
{
  friend class consoleBase;
  public:
    virtual int overflow   (int d);  
    virtual int underflow  (void);  

  protected:
                consoleBuf  (Window* wnd,OutTextFn fn):streambuf(), mOut (fn), mWnd (wnd) { unbuffered(1); }
    virtual     ~consoleBuf ();

  protected:
        OutTextFn  mOut;
        Window*    mWnd;
};

class consoleBase: public virtual ios
{
  protected:
    consoleBase (Window* wnd,OutTextFn fn):buf(wnd,fn) { ios::init(&this->buf); }
    consoleBuf buf;
};

class oConsolestream: public consoleBase, public ostream
{
  public:
    oConsolestream(Window* wnd,OutTextFn fn):consoleBase(wnd,fn),ostream(&buf) {}
};

#pragma pack()

int      Start (Application*,char*);
Window*  tout;

A_Start (Start,10000000,"Console test");
void    outtext (Window*,int);

#pragma comment (lib,"gdi32.lib")

int     Start (Application* app,char*)
{
  Window* wnd = new (app->pool()) Console (app->wnd(),rect_t (0,0,640,480),"Console");
  tout        = new (app->pool()) Text    (wnd,rect_t (20,20,580,420),"Console");

  SendMessage(tout->wnd(),EM_FMTLINES,1,0);

  oConsolestream out (tout,outtext);

  out<<"Hello world!"<<endl;
  out<<"...KKK"<<endl<<endl;;

  int state   = app->MainLoop ();

  delete tout;
  delete wnd;

  return state;
}

void     Console::Init     (const rect_t& rect)
{
    WNDCLASS    wc;
    LOGBRUSH    brush = {
      BS_SOLID,
      RGB (75,75,75),
      0
    };

    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = (WNDPROC) WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = instance ();
    wc.hIcon         = LoadIcon   ( NULL, IDI_APPLICATION );
    wc.hCursor       = LoadCursor ( NULL, IDC_ARROW ); 
//    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.hbrBackground = CreateBrushIndirect (&brush);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = mClassName;

    RegisterClass (&wc);

    create        (mClassName,
                   mClassName,
                   WS_SYSMENU | WS_OVERLAPPED | WS_VISIBLE | WS_MINIMIZEBOX,
                   rect
                  );
}

Console::Console  (Window* wnd,const rect_t& rect,const char* _WndName)
        : Window (wnd)
                
{
  strcpy (mClassName,_WndName);

  Init   (rect);
}

Console::~Console ()
{
  UnregisterClass (mClassName,instance());
}


void     Text::Init     (const rect_t& rect)
{
  create ("","EDIT",
          WS_CHILD|WS_BORDER|WS_VISIBLE|ES_LEFT|ES_MULTILINE|ES_READONLY|ES_AUTOVSCROLL,
          rect
         );

  SendMessage(wnd(),WM_SETFONT, 
             (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0); 
}

Text::Text  (Window* wnd,const rect_t& rect,const char* _WndName)
        : Window (wnd)
                
{
  strcpy (mClassName,_WndName);

  Init   (rect);
}

Text::~Text ()
{
//  UnregisterClass (mClassName,instance());
}

int consoleBuf::overflow(int d)
{
  (*mOut)(mWnd,d);
  return d;
}

int consoleBuf::underflow()  
{
  return EOF;
}

consoleBuf::~consoleBuf()
{    }

void    outtext (Window* wnd,int simb)
{
  static char RETURN [] = "\r\n";
  static char TAB   []  = "        ";

  switch (simb)
  {
    case '\n':
      SendMessage(wnd->wnd(),EM_REPLACESEL,0,(LPARAM)RETURN);
      break;
    case '\t':    
      SendMessage(wnd->wnd(),EM_REPLACESEL,0,(LPARAM)TAB);
      break;
    default:
    {
      char buf [2] = {simb,0};
      SendMessage(wnd->wnd(),EM_REPLACESEL,0,(LPARAM)buf);
    }  
  }
}

