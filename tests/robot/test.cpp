#define  DEBUG
#include <thread\app.h>
#include <thread\fswindow.h>
#include <graph\ogl.h>

int      start   (Application*,char*);
void     Draw    (HDC);
void     Reshape (int,int,HDC);       

A_Start (start,M_GetPool());

int     start (Application* app,char*)
{
  Window*    wnd = new (app->pool()) FullScreenWindow (app->pool(),rect_t (0,0,800,600),"Robot test");
  OpenGLView ogl (wnd,Draw,Reshape);

  return app->MainLoop ();
}

void     Reshape (int width,int height,HDC)
{
  glViewport       (0,0,width,height);
  glMatrixMode     (GL_PROJECTION);
  glLoadIdentity   ();
  glOrtho          (-80,80,-30,100,1.0,4000.0);

  glMatrixMode     (GL_MODELVIEW);
  glLoadIdentity   ();

  glClearColor     (0.5,0.5,0.5,1.0);
  glClear          (GL_COLOR_BUFFER_BIT);

  glEnable         (GL_DEPTH_TEST);   
}

void     Draw    (HDC hdc)
{
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  

  glFinish ();        

  SwapBuffers (hdc);
}