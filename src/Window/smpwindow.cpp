#include <pch.h>

void   SimpleWindow::Init  (const rect_t& rect)
{
  D_Try
  {
    WNDCLASS    wc;

    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = (WNDPROC) WndProc;
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
                   WS_SYSMENU | WS_OVERLAPPED | WS_VISIBLE | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
                   rect
                  );
  }
  D_Exc ("SimpleWindow: Error at Init ()");
}

SimpleWindow::SimpleWindow  (Pool* pool,const rect_t& rect,const char* _WndName)
             : Window (pool)
{
  strcpy (mClassName,_WndName);
  Init   (rect);
}

SimpleWindow::SimpleWindow  (Window* wnd,const rect_t& rect,const char* _WndName)
             : Window (wnd)
{
  strcpy (mClassName,_WndName);
  Init   (rect);
}

SimpleWindow::~SimpleWindow ()
{
  UnregisterClass (mClassName,instance ());
}
