#define  DEBUG
#include <ogl.h>
#include <thread\app.h>
#include <graph\utils.h>
#include <graph\matrix.h>

void     Draw     ();
void     Reshape  (int,int);
int      Done     (Window*,UINT,WORD,DWORD,DWORD);

matrix_t  m;
frustum_t f;

int main (int,char* [])
{
  Application*  app = A_GetApplication ();       
  Window*       wnd = IF_CreateWindow (SIMPLE_WINDOW,
                                       M_GetPool (),
                                       app->instance (),
                                       rect_t (0,0,800,600),
                                       "Split Facet Test"
                                      );

  OGL_Load              ();
  OGL_DisplayMode       (OGL_DOUBLE | OGL_DEPTH | OGL_RGB);
  OGL_ReshapeFunc       (Reshape);
  OGL_DrawFunc          (Draw);
  OGL_CreateContext     (OGL_CONTEXT0,wnd);

  wnd->InsertHandler    (WM_DESTROY,Done);

  MainLoop              ();

  delete wnd;

  OGL_Done              ();

  return EXIT_SUCCESS;
}

void     Reshape (int width,int height)
{
  glViewport       (0,0,width,height);
  glMatrixMode     (GL_PROJECTION);
  glLoadIdentity   ();
  glOrtho          (-20,20,-20,20,1.0,4000.0);  

  CalcGLFrustum    (f);

  glLoadIdentity   ();
  glOrtho          (-40,40,-40,40,1.0,4000.0);

  glMatrixMode     (GL_MODELVIEW);
  glLoadIdentity   ();
}

void     Draw    ()
{
  glClear     (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glColor3f   (1,1,1);

  vertex_t  v [3] = { vertex_t (-25,-25,-5),
                      vertex_t (25,-25,-5),                    
                      vertex_t (0,35,-5)
                    };
  vertex_t  dest [32];

  int trCount = SplitFacet (v,6,f.clip,dest);

  dout<<trCount<<endl;

  glBegin (GL_TRIANGLES);
    for (int i=0;i<3*trCount;i++)
      glVertex3fv ((GLfloat*)&dest [i]);
  glEnd   ();

  glFinish    ();

  OGL_SwapBuffers ();
}

int      Done     (Window*,UINT,WORD,DWORD,DWORD)
{
  PostQuitMessage (0);
  return 0;
}
