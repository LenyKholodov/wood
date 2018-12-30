#define  DEBUG
#include <window.h>
#include <misc.h>
#include <kernel\app.h>

int      main (int,char* [])
{
  Application*  app = A_GetApplication ();       
  Window*       wnd = IF_CreateWindow (SIMPLE_WINDOW,
                                       M_GetPool (),
                                       app->instance (),
                                       rect_t (0,0,800,600),
                                       "Window drawing"
                                      );

  HPEN   pen    = Window::CreatePen (pen_t (RGB(255,0,0),1,PS_SOLID));
  HBRUSH brush  = Window::CreateBrush (brush_t (RGB(0,0,255),BS_SOLID,0));
  HFONT  font   = Window::CreateGDIFont  (gdifont_t (16,16));

  wnd->SelectObject (pen);
  wnd->SelectObject (brush);
  wnd->SelectObject (font);
  wnd->SetTextColor (RGB(0,255,0));

  wnd->InsertHandler (WM_DESTROY,WND_Done);
  wnd->FillRect      (0,0,100,100);
  wnd->Line          (0,0,100,100);
  wnd->Line          (20,20,50,500);
  wnd->OutText       (300,300,"Hello world!");

  MainLoop           ();

  delete wnd;

  return EXIT_SUCCESS;
}