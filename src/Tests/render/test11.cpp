#include <kernel\app.h>

#include <render\interface\text.h>
#include <render\fontdefs.h>
#include <render\render.h>
#include <render\startup.h>

#include <gl\gl.h>

void     DoTest ();

int main (int argc,char* argv [])
{
  K_FastInit (NULL);

  DoTest ();

  return EXIT_SUCCESS;
}

void     DoTest ()
{
  SetRenderPool (M_GetPool ());

  Render        render;    
  RenderStartup startup;
  FreeCamera    cam;

  cam.Move (Vector3 (0,0,-5));
  cam.proj.SetOrtho (-5,5,-5,5,0,100);
//  vp.GetProj ().SetPerspective (-10,10,-10,10,1,100);

  render.SetContext (&startup);
  render.SetCamera  (&cam);

  glClearColor      (0,0,0,0);
  glClear           (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable          (GL_DEPTH_TEST);
  glEnable          (GL_TEXTURE_2D);

  dout<<"OpenGL 1.2\t"<<extgl_Extensions.OpenGL12<<endl;
  dout<<"OpenGL 1.3\t"<<extgl_Extensions.OpenGL13<<endl;
  dout<<"OpenGL 1.4\t"<<extgl_Extensions.OpenGL14<<endl;

  render.GetMatEnv ().SetParam (ME_TEXTURE_FAST_SCALE,true);

  RenderText    text;  

  render.RegisterInterface (text);

  TrueTypeFont  font, font1; 

//  text.AddResource (font);
//  text.AddResource (font1);

//  glBindBufferARB = (glBindBufferARBPROC) extgl_GetProcAddress("glBindBufferARB");
//  glDeleteBuffersARB = (glDeleteBuffersARBPROC) extgl_GetProcAddress("glDeleteBuffersARB");

  text.SetCurrent  (font);
  text.SetCurrent  (font1);

  render.BeginDraw  ();

  const int total = 1000000;
  float start = clock ();

  for (int i=0;i<total;i++)
  {
    text.SetCurrent (font);
    text.SetCurrent (font1);
  }  

  float end = clock ();

  dout<<"time\t"<<(end-start)/float (CLK_TCK)<<endl;
  dout<<"FPS\t"<<float (2*total)/(end-start)*float (CLK_TCK)<<endl;

  render.EndDraw    ();

  MainLoop          ();
}

