#define  DEBUG
#include <window.h>
#include <misc.h>
#include <kernel\keydefs.h>
#include <kernel\app.h>

#include <console\gdicon.h>

ConsoleBase*  base = NULL;

int      Keyb     (Window*,UINT,WORD,DWORD,DWORD);

int      main (int,char* [])
{
  Window*       wnd = ext_new   SimpleWindow ( M_GetPool (),rect_t (0,0,800,600),
                                       "Simple console"
                                      );

  ConsoleBase con        (M_GetPool(),"log",256,1024);
  GDIConsole  gdiCon     (M_GetPool(),wnd,con);

  base = &con;

  con.refresh (TRUE);

  for (int i=0;i<200;i++)
  {
    con.os()<<i;
    for (int j=0;j<18;j++)
      con.os()<<" Hello world ";
//      con.os()<<"^"<<j<<" Hello world ";
    con.os()<<endl;
  }  

  con.refresh ();

  wnd->InsertHandler     (WM_DESTROY,WND_Done);
  wnd->InsertHandler     (WM_KEYDOWN,Keyb);
  wnd->InsertHandler     (WM_KEYUP,Keyb);

  MainLoop               ();

  delete wnd;

  return EXIT_SUCCESS;
}

int      Keyb    (Window*,UINT msg,WORD key,DWORD lParam,DWORD)
{
  if (msg == WM_KEYDOWN && key < 1024)
  {
    switch (key)
    {
      case VK_UP: base->ScrollUp (); break;
      case VK_DOWN: base->ScrollDown (); break;
    }
  }  

  return 0;
}
