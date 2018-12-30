#include <kernel\app.h>
#include <window.h>
#include <render\device.h>
#include <render\context.h>
#include <gl\gl.h>
#include <kernel\thread.h>

using namespace Render;

class TestThread: public Thread
{
  public:
    TestThread (Context& cntx,
     const Color3& col,
     const rect_t& r): rect (r), Thread (false), c (cntx),color (col) { StartThread (); }

    virtual int process ();

  private:
    Context& c;
    Color3   color;
    rect_t   rect;
};

bool flag = true;

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

//  wndDevice.SetSwapInterval (70);

  Context* c1 = wndDevice.CreateContext (RC_MULTI_THREAD);

  c1->BeginDraw ();

  glEnable (GL_DEPTH_TEST);    
  glOrtho  (-20,20,-20,20,-100,100);

  glClearColor (0,0.5,0.5,0);
  glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  c1->EndDraw ();

//  c1->SetVrtlViewport (0.3,0.3,0.5,0.5);
                        
  PBuffer* pbuf = wndDevice.CreatePixelBuffer (Device::Format (DF_RGBA24|DF_DEPTH|DF_ALPHA|DF_STENCIL));
//  PBuffer* pbuf = wndDevice.CreatePixelBuffer ();

  if (pbuf)
    dout<<pbuf->GetFormat ()<<endl;

  TestThread test1 (*c1,Color3 (1,0,0),rect_t (0,0,300,300)), 
             test2 (*c1,Color3 (0,0,1),rect_t (200,200,300,300)); 

  dout<<c1->GetVendor ()<<endl;
  dout<<c1->GetRenderer ()<<endl;
  dout<<c1->GetVersion ()<<endl;
  dout<<c1->GetExtensions ()<<endl;

  MainLoop ();

  dout<<"!heye"<<endl;

  delete c1;

//  flag = false;

//  while (K_GetCurrentThread ()->count ());
}

int TestThread::process ()
{
  while (flag)
  {
//    dout<<"wait begin "<<GetCurrentThreadId ()<<endl;
    c.BeginDraw ();
    
//    glViewport (rect.x,rect.y,rect.width,rect.height);
  //  dout<<"wait begin ok "<<GetCurrentThreadId ()<<endl;

//    glClearColor (1-color.x,1-color.y,1-color.z,0);
    glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glColor3fv ((GLfloat*)&color);

    glBegin (GL_TRIANGLES);
      glVertex2i (-10,-10);
      glVertex2i (10,-10);
      glVertex2i (0,10);
    glEnd   ();

    glColor3f (1,1,1);

    glBegin (GL_QUADS);
      glVertex2i (-20,-20);
      glVertex2i ( 20,-20);
      glVertex2i ( 20, 20);
      glVertex2i (-20, 20);
    glEnd ();

    c.EndDraw ();
  }  

  return 0;
}
