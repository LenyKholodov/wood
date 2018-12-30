#include <window.h>
#include <kernel\app.h>

void     Idle ();

Window* wnd = NULL;

int  main (int,char* [])
{
  wnd = ext_new SimpleWindow (M_GetPool(),rect_t (0,0,800,600),"Double buffer TEST");

  dout<<"create new window "<<wnd<<endl;

  MainLoop (Idle);

  dout<<"std delete window"<<endl;

  return EXIT_SUCCESS;
}

void     Idle ()
{
  wnd->BeginDraw ();
  wnd->SelectObject (&brush_t (RGB(0,0,128),BS_SOLID,0));
  wnd->Cls          ();
  wnd->Line      (10,10,100,100);
  wnd->EndDraw   ();
}