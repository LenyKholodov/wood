#include <kernel\app.h>
#include <window.h>
#include <gl\extgl.h>
#include <ogl.h>

glBindBufferARBPROC glBindBufferARB = NULL;
glDeleteBuffersARBPROC glDeleteBuffersARB = NULL;
glGenBuffersARBPROC glGenBuffersARB = NULL;
glIsBufferARBPROC glIsBufferARB = NULL;
glBufferDataARBPROC glBufferDataARB = NULL;
glBufferSubDataARBPROC glBufferSubDataARB = NULL;
glGetBufferSubDataARBPROC glGetBufferSubDataARB = NULL;
glMapBufferARBPROC glMapBufferARB = NULL;
glUnmapBufferARBPROC glUnmapBufferARB = NULL;
glGetBufferParameterivARBPROC glGetBufferParameterivARB = NULL;
glGetBufferPointervARBPROC glGetBufferPointervARB = NULL;

const   int     N1 = 50;
const   int     N2 = 50;

void     Reshape (int width,int height);
void     Draw    ();
void     DrawTorus ();
void     InitTorus ();
void     Idle      ();
void     InitExtensions ();

uint     buffer = 0;
void*    vertexes = NULL;
void*    colors    = NULL;
uint*    indexes   = NULL;

int main (int,char* [])
{
  Window*       wnd = ext_new SimpleWindow (M_GetPool (),
                                       rect_t (0,0,800,600),
                                       "VBO support test"
                                      );

  SrvMakeCurrent (OGL_Load  ());
  OGL_DisplayMode       (OGL_DOUBLE | OGL_DEPTH | OGL_RGB);
  OGL_ReshapeFunc       (Reshape);
  OGL_DrawFunc          (Draw);
  OGL_CreateContext     (OGL_CONTEXT0,wnd);
  OGL_Start             (OGL_RESHAPE_CALL);

  InitExtensions        ();
  InitTorus             ();

//  wnd->InsertHandler    (WM_DESTROY,WND_Done);

  MainLoop              (Idle);

  delete wnd;

  OGL_Done              ();

  return EXIT_SUCCESS;
}

void     Reshape (int width,int height)
{
  glViewport       (0,0,width,height);
  glMatrixMode     (GL_PROJECTION);
  glLoadIdentity   ();
  glOrtho          (-10,10,-10,10,1.0,4000.0);  

  glMatrixMode     (GL_MODELVIEW);
  glLoadIdentity   ();
  gluLookAt        (0,0,10,
                    0,0,-1,
                    0,1,0);    

  glClearColor     (0.5,0.5,0.5,1.0);
  glClear          (GL_COLOR_BUFFER_BIT);

  glShadeModel     (GL_SMOOTH);
  glPolygonMode    (GL_FRONT_AND_BACK,GL_LINE);

  glEnable         (GL_DEPTH_TEST); 
}

void     Draw    ()
{
  glClear     (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  DrawTorus   ();

  glFinish    ();

  OGL_SwapBuffers ();
}

void Idle ()
{
  Draw ();
}

void InitTorus ()
{
  float         r1 = 4;         //radius low
  float         r2 = 1;         //radius high

  typedef uint index_t [3];

  vertex_t* vertex = new vertex_t [N1*N2];
  index_t*  index  = new index_t [N1*N2*2];

  int   k = 0;
  for (int i=0;i<N1;i++)
  {
    float       phi = i*2*FL_PI/N1;

    for (int j=0;j<N2;j++,k++)
    {
      float     psi = j*2*FL_PI/N2;

      vertex [k].x = (r1+r2*cos(psi)) * cos(phi);
      vertex [k].y = (r1+r2*cos(psi)) * sin(phi);
      vertex [k].z = r2 * sin(psi);
    }
  }

  for (i=k=0;i<N1;i++)
      for (int j=0;j<N2;j++,k+=2)
      {
        index [k][0] = i*N2 + j;
        index [k][1] = ((i+1) % N1) * N2 +j;
        index [k][2] = ((i+1) % N1) * N2 + (j+1) % N2;

        index [k+1][0] = i*N2 + j;
        index [k+1][1] = ((i+1) % N1) * N2 + (j+1) % N2;
        index [k+1][2] = i * N2 + (j+1) % N2;
      }

  glGenBuffersARB   (1,&buffer);
  glBindBufferARB   (GL_ARRAY_BUFFER_ARB,buffer);  
  dout<<glGetError ()<<endl;
  glBufferDataARB   (GL_ARRAY_BUFFER_ARB,N1*N2*sizeof (vertex_t),vertex,GL_STATIC_DRAW_ARB);
 
  indexes = (uint*)index;
//  vertexes = vertex; //!!!!!!!
}

void    DrawTorus ()
{
  glBindBufferARB     (GL_ARRAY_BUFFER_ARB,buffer);
  glVertexPointer     (3,GL_FLOAT,0,vertexes);

  glEnableClientState (GL_VERTEX_ARRAY);
  glDrawElements      (GL_TRIANGLES,N1*N2*2,GL_UNSIGNED_INT,indexes);
}

void     InitExtensions ()
{
  glBindBufferARB = (glBindBufferARBPROC) wglGetProcAddress("glBindBufferARB");
  glDeleteBuffersARB = (glDeleteBuffersARBPROC) wglGetProcAddress("glDeleteBuffersARB");
  glGenBuffersARB = (glGenBuffersARBPROC) wglGetProcAddress("glGenBuffersARB");
  glIsBufferARB = (glIsBufferARBPROC) wglGetProcAddress("glIsBufferARB");
  glBufferDataARB = (glBufferDataARBPROC) wglGetProcAddress("glBufferDataARB");
  glBufferSubDataARB = (glBufferSubDataARBPROC) wglGetProcAddress("glBufferSubDataARB");
  glGetBufferSubDataARB = (glGetBufferSubDataARBPROC) wglGetProcAddress("glGetBufferSubDataARB");
  glMapBufferARB = (glMapBufferARBPROC) wglGetProcAddress("glMapBufferARB");
  glUnmapBufferARB = (glUnmapBufferARBPROC) wglGetProcAddress("glUnmapBufferARB");
  glGetBufferParameterivARB = (glGetBufferParameterivARBPROC) wglGetProcAddress("glGetBufferParameterivARB");
  glGetBufferPointervARB = (glGetBufferPointervARBPROC) wglGetProcAddress("glGetBufferPointervARB");  

  dout<<glBufferDataARB<<endl;
}
