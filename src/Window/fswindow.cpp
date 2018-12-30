#include <pch.h>

void  FullScreenWindow::Init  (const rect_t& rect)
{
  D_Try
  {
    WNDCLASS    wc;
    DEVMODE     mode;   

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
                   WS_POPUP | WS_VISIBLE,
                   rect
                  );

    memset(&mode,0,sizeof(DEVMODE));

    mode.dmSize         = sizeof(DEVMODE);

    mode.dmPelsWidth    = rect.width;  
    mode.dmPelsHeight   = rect.height;  
    mode.dmFields       = DM_PELSWIDTH | DM_PELSHEIGHT;

    ChangeDisplaySettings (&mode,CDS_FULLSCREEN);    
  }
  D_Exc ("FullScreenWindow: Error at Init ()");
}

FullScreenWindow::FullScreenWindow  (Pool* _Pool,const rect_t& rect,const char* name)
                 : Window (_Pool)
{
  strcpy (mClassName,name);

  Init (rect);
}

FullScreenWindow::FullScreenWindow  (Window* _Window,const rect_t& rect,const char* name)
                 : Window (_Window)
{
  strcpy (mClassName,name);

  Init (rect);
}

FullScreenWindow::~FullScreenWindow ()
{
  UnregisterClass (mClassName,instance ());
}
