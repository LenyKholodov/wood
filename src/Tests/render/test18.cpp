#include <kernel\app.h>
#include <window.h>
#include <render\device.h>
#include <gl\gl.h>

using namespace Render;

static total = 0;

void TestDevice (HWND);

int main (int argc,char* argv [])
{
  Window* wnd = new PopUpWindow (M_GetPool (),rect_t (0,0,800,600),"Render::Device test");

  TestDevice (wnd->wnd ());

  delete wnd;

  return EXIT_SUCCESS;
}

void TestDevice (HWND wnd)
{
  Device::Format fmt (DF_RGBA24|DF_DEPTH|DF_STENCIL|DF_DOUBLE);

  WindowDevice wndDevice (wnd,fmt);

  HDC dc = wndDevice.GetDC ();

  HGLRC rc = wglCreateContext (dc);

  dout<<rc<<endl;

  clock_t start = clock ();

  const int total = 10000000;

  for (int i=0;i<total;i++)
  {
    wglGetCurrentContext ();
//    wglMakeCurrent (dc,rc);
//    wglMakeCurrent (NULL,NULL);
  }  

  clock_t end = clock ();

  dout<<"FPS:\t"<<float (total)/float (end-start)*float (CLK_TCK)<<endl;

  MainLoop ();
}