#include <kernel\app.h>
#include <window.h>
#include <gl/gl.h>
#include <gl/glu.h>

#pragma comment (lib,"opengl32.lib")

void        setDCPixelFormat ( HDC );
void        initializeRC ();
void        reshape (int,int);
void        draw ();
void        drawBox ();

class  TestWindow: public Window
{
  public:
                        TestWindow  (Pool*,
                                       const rect_t&
                                       );
                        TestWindow  (Window*,
                                       const rect_t&
                                      );
  protected:
           void         Init              (const rect_t&);
};

HDC dc1, dc2;

int main (int,char* [])
{
  Window* wnd = new PopUpWindow (M_GetPool (),rect_t (0,0,1024,768),"Hello world!");
//  Window* wnd1 = new TestWindow (M_GetPool (),rect_t (0,0,800,600));
//  Window* wnd1 = new TestWindow (wnd,rect_t (10000,10000,1024,768));
  Window* wnd1 = new TestWindow (wnd,rect_t (0,0,1024,768));

  SetActiveWindow (wnd->wnd ());

  dc1 = GetDC (wnd->wnd ());     
  dc2 = GetDC (wnd1->wnd ());

  setDCPixelFormat (dc1);
  setDCPixelFormat (dc2);

  HGLRC rc1 = wglCreateContext (dc1);

  dout<<wglMakeCurrent (dc2,rc1)<<endl;

  initializeRC     ();
  glViewport (0,0,256,256);
  drawBox          ();

  glColor3f (0,1,0);

  glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glBegin (GL_TRIANGLES);
    glVertex3f (-20,-20,0);
    glVertex3f (20,-20,0);
    glVertex3f (0,20,0);
  glEnd   ();
  
  uint texture = 0;

  glGenTextures (1,&texture);
  glBindTexture    (GL_TEXTURE_2D,texture);

  glCopyTexImage2D (GL_TEXTURE_2D,
                      0,
                      GL_RGBA, 
                      0,
                      0,
                      256,
                      256,
                      0
                     );  

  glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
  glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
  glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,     GL_REPEAT); 
  glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,     GL_REPEAT);

  dout<<wglMakeCurrent (dc1,rc1)<<endl;
  glClearColor (0,0,1,0);
  glViewport (0,0,1024,768);
  glEnable (GL_TEXTURE_2D);
  glTexEnvi        (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

  glColor3f (1,1,1);

  draw  ();

  SwapBuffers (dc2);
  SwapBuffers (dc1);

  MainLoop ();

  return EXIT_SUCCESS;
}

void    setDCPixelFormat ( HDC hdc )
{
        static PIXELFORMATDESCRIPTOR pfd =
        {
                sizeof ( PIXELFORMATDESCRIPTOR ),                       // sizeof this structure
                1,                                                                                      // version number
//                PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,        // flags
                PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,        // flags
                PFD_TYPE_RGBA,                                                          // RGBA pixel values
                24,                                                                                     // 24-bit color
                0, 0, 0, 0, 0, 0,                                                       // don't care about these
                0, 0,                                                                           // no alpha buffer
                0, 0, 0, 0, 0,                                                          // no accumulation buffer
                32,                                                                                     // 32-bit depth buffer
                0,                                                                                      // no stencil buffer
                0,                                                                                      // no auxiliary buffers
                PFD_MAIN_PLANE,                                                         // layer type
                0,                                                                                      // reserved (must be 0)
                0, 0, 0                                                                         // no layer masks
        };
        int     pixelFormat;

        pixelFormat = ChoosePixelFormat ( hdc, &pfd );
        SetPixelFormat ( hdc, pixelFormat, &pfd );
        DescribePixelFormat ( hdc, pixelFormat,
                              sizeof (PIXELFORMATDESCRIPTOR), &pfd );

        if ( pfd.dwFlags & PFD_GENERIC_FORMAT)
                MessageBox ( NULL, "Wrong video mode", "Attention", MB_OK );
}

void    initializeRC ()
{
  glEnable     ( GL_DEPTH_TEST );
  glClearColor ( 0.0, 0.0, 0.0, 0.0 );
  glMatrixMode (GL_PROJECTION);
  glOrtho      (-20,20,-20,20,-1,1);
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
}

void reshape (int w,int h)
{
  glViewport (0,0,w,h);
}

void  draw ()
{
  glClear  (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3f (1,0,1);
  glCallList (1);
  glFinish   ();
}

void TestWindow::Init (const rect_t& rect)
{
    WNDCLASS    wc;

    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = (WNDPROC) WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = instance ();
    wc.hIcon         = NULL;
    wc.hCursor       = NULL; 
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "Hey-ho";

    RegisterClass (&wc);

    create        ("Hey-ho",
                   "Hey-ho",
                   WS_POPUP|WS_MINIMIZE,
                   rect
                  );
}

TestWindow::TestWindow  (Pool* pool,const rect_t& rect)
             : Window (pool)
{
  Init (rect);
}

TestWindow::TestWindow  (Window* wnd,const rect_t& rect)
             : Window (wnd)
{
  Init (rect);
}

void drawBox  ()
{
        glNewList (1,GL_COMPILE);
    glBegin  (GL_QUADS);
      glTexCoord2f (0,0);
      glVertex3f (-10,-10,0);
      glTexCoord2f (1,0);
      glVertex3f ( 10,-10,0);
      glTexCoord2f (1,1);
      glVertex3f ( 10, 10,0);
      glTexCoord2f (0,1);
      glVertex3f (-10, 10,0);
    glEnd    ();
    glEndList ();
}
