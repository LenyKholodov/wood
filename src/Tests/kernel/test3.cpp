#define  DEBUG
//#include <windows.h>
#include <ogl.h>
#include <kernel\app.h>

void     Reshape  (int width,int height);
void     Draw     ();
int      Done     (Window*,UINT,WORD,DWORD,DWORD);

int      main (int,char* [])
{
  Application*  app = A_GetApplication ();       
  Window*       wnd = IF_CreateWindow (SIMPLE_WINDOW,
                                       M_GetPool (),
                                       app->instance (),
                                       rect_t (0,0,800,600),
                                       "OGLUnit test"
                                      );

  OGL_Load              ();
  OGL_DisplayMode       (OGL_DOUBLE | OGL_DEPTH | OGL_RGB);
  OGL_ReshapeFunc       (Reshape);
  OGL_DrawFunc          (Draw);
  OGL_CreateContext     (OGL_CONTEXT0,wnd);
  OGL_Start             (OGL_RESHAPE_CALL);

  wnd->InsertHandler    (WM_DESTROY,Done);

  MainLoop              ();

  delete wnd;

  OGL_Done              ();

  return EXIT_SUCCESS;
}

void   Reshape    (int width,int height)
{
  glViewport       (0,0,width,height);
  glMatrixMode     (GL_PROJECTION);
  glLoadIdentity   ();
  glOrtho          (-10,10,-20,20,-100.0,100.0);

  glClearColor     (0.5,0.5,0.5,1.0);
  glClear          (GL_COLOR_BUFFER_BIT);

  glEnable         (GL_DEPTH_TEST);  
}

void   Draw   ()
{
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glColor3f (1,0,0);

  glBegin (GL_TRIANGLES);
    glVertex3f (0,8,-8);
    glVertex3f (8,0,-8);
    glVertex3f (-8,0,-8);
  glEnd   ();

  glFinish ();

  OGL_SwapBuffers ();
}

#include <windows.h>

int      Done     (Window*,UINT,WORD,DWORD,DWORD)
{
  PostQuitMessage (0);
  return 0;
}
