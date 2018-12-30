#include <gl/extgl.h>
#include <graph\utils.h>
#include <kernel\app.h>
#include <debug.h>
#include <ogl.h>
#include <misc.h>

void     Reshape  (int width,int height);
void     Draw     ();

int      main (int,char* [])
{
  Window*       wnd = new SimpleWindow  (M_GetPool (),rect_t (0,0,800,600),"TEST");

  SrvMakeCurrent (OGL_Load  ());
  OGL_DisplayMode       (OGL_DOUBLE | OGL_DEPTH | OGL_RGB);
  OGL_ReshapeFunc       (Reshape);
  OGL_DrawFunc          (Draw);
  OGL_CreateContext     (OGL_CONTEXT0,wnd);
  OGL_Start             (OGL_RESHAPE_CALL);  

  MainLoop              (Draw);

  delete wnd;

  OGL_Done              ();

  return 0;
}

void     Reshape (int width,int height)
{
  glViewport       (0,0,width,height);
  static char buf [4096*4096*4];

  glGetError  ();

  glHint (GL_GENERATE_MIPMAP_HINT,GL_NICEST);

  glBindTexture    (GL_TEXTURE_2D,1);
  glTexParameteri  (GL_TEXTURE_2D,GL_GENERATE_MIPMAP,GL_TRUE);
  glTexImage2D     (GL_TEXTURE_2D,0,GL_COMPRESSED_RGBA,2048,2048,0,GL_RGBA,GL_UNSIGNED_BYTE,buf);

  dout<<glGetError ()<<endl;

  glBindTexture    (GL_TEXTURE_2D,2);
  glTexParameteri  (GL_TEXTURE_2D,GL_GENERATE_MIPMAP,GL_TRUE);
  glTexImage2D     (GL_TEXTURE_2D,0,GL_COMPRESSED_RGBA,2048,2048,0,GL_RGBA,GL_UNSIGNED_BYTE,buf);

  dout<<glGetError ()<<endl;
}

void    Draw ()
{
  const int total = 1000000;

  float start = clock ();

  for (int i=0;i<total;i++)
  {
    glBindTexture (GL_TEXTURE_2D,1);
//    glBindTexture (GL_TEXTURE_2D,2);
  }  

  float end = clock ();

  dout<<"FPS:\t"<<float (total)/(end-start)*float (CLK_TCK)<<endl;
//  dout<<"FPS:\t"<<2.0f*float (total)/(end-start)*float (CLK_TCK)<<endl;

  glFinish ();
  OGL_SwapBuffers ();
}

