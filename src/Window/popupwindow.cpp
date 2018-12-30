#include <pch.h>

void  PopUpWindow::Init  (const rect_t& rect)
{
  D_Try
  {
    WNDCLASS    wc;

    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = instance ();
    wc.hIcon         = LoadIcon   ( NULL, IDI_APPLICATION );
    wc.hCursor       = LoadCursor ( NULL, IDC_ARROW ); 
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = mClassName;

    RegisterClass (&wc);

    create        (mClassName,
                   mClassName,
                   WS_POPUP | WS_VISIBLE,
                   rect
                  );
  }
  D_Exc ("PopUpWindow: Error at Init () "<<this);
}

PopUpWindow::PopUpWindow  (Pool* _Pool,const rect_t& rect,const char* name)
                 : Window (_Pool)
{
  strcpy (mClassName,name);
  Init   (rect);
}

PopUpWindow::PopUpWindow  (Window* _Window,const rect_t& rect,const char* name)
                 : Window (_Window)
{
  strcpy (mClassName,name);
  Init   (rect);
}

PopUpWindow::~PopUpWindow ()
{
  UnregisterClass (mClassName,instance ());
}
