#define DEBUG
#include <graph\utils.h>
#include <kernel\app.h>
#include <debug.h>
#include <ogl.h>
#include <misc.h>

void     Reshape  (int width,int height);
void     Draw     ();

int      main (int,char* [])
{
  Application*  app = A_GetApplication ();       
  Window*       wnd = IF_CreateWindow (SIMPLE_WINDOW,
                                       M_GetPool (),
                                       app->instance (),
                                       rect_t (0,0,800,600),
                                       "3DS Animation Test"
                                      );

  OGL_Load              ();
  OGL_DisplayMode       (OGL_DOUBLE | OGL_DEPTH | OGL_RGB);
  OGL_ReshapeFunc       (Reshape);
  OGL_DrawFunc          (Draw);
  OGL_CreateContext     (OGL_CONTEXT0,wnd);
  OGL_Start             (OGL_RESHAPE_CALL);  

  wnd->InsertHandler    (WM_DESTROY,WND_Done);

  MainLoop              (Draw);

  delete wnd;

  OGL_Done              ();


  return EXIT_SUCCESS;
}

void     Reshape (int width,int height)
{
  glViewport       (0,0,width,height);
  glMatrixMode     (GL_PROJECTION);
  glLoadIdentity   ();
  glOrtho          (-80,80,-30,100,1.0,4000.0);  

  gluLookAt        (12,13,14,0,0,0,0,1,0);

  glMatrixMode     (GL_MODELVIEW);
  glLoadIdentity   ();
}

void    Draw ()
{
  matrix_t   m (1);
  frustum_t  f;

  glGetFloatv (GL_PROJECTION_MATRIX,(GLfloat*)&m);

  m.transpose ();

  CalcFrustum  (m,f);

  dout<<"Matrix:"<<endl;

  for (int i=0;i<6;i++)
    dout<<f.clip [i].a<<" "<<f.clip [i].b<<" "<<f.clip [i].c<<" "<<f.clip [i].d<<endl;

  CalcGLFrustum  (f);

  dout<<"GL:"<<endl;

  for (i=0;i<6;i++)
    dout<<f.clip [i].a<<" "<<f.clip [i].b<<" "<<f.clip [i].c<<" "<<f.clip [i].d<<endl;
}

