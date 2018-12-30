#include <kernel\app.h>

#include <render\render.h>
#include <render\startup.h>
#include <render\bitmap.h>
#include <render\mtlcolor.h>

#include <gl\gl.h>

int main (int argc,char* argv [])
{
  K_FastInit (NULL);

  Pool* pool = new FloatPool (M_GetPool (),1024*1024*64);

  dout<<M_GetPool ()<<endl;
  dout<<pool<<endl;

  SetRenderPool (pool);

  Render        render;    
  RenderStartup startup;
  FreeCamera    cam;

  cam.Move (Vector3 (0,0,-5));

  cam.proj.SetOrtho (-5,5,-5,5,0,100);

  render.context.SetContext (&startup);
  render.viewer.SetCamera  (&cam);

  glClearColor      (0,0,0,0);
  glClear           (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable          (GL_DEPTH_TEST);
  glEnable          (GL_TEXTURE_2D);

  render.mtls.texFastScale = true;

  Bitmap mat ("image.jpg");
  Bitmap mat1 ("atoll.bmp");
  Bitmap mat2 ("atoll.bmp");

  mat.SetTiling (2,2);
  mat.Translate (.4,.4);

  render.mtls.SetMaterial (mat);
  render.mtls.SetMaterial (mat2);
  render.mtls.SetMaterial (mat1);

  render.BeginDraw  ();

  const int total = 1000000;
  float start = clock ();

  for (int i=0;i<total;i++)
  {
    render.mtls.SetMaterial (mat);
    render.mtls.SetMaterial (mat1);
  }  

  float end = clock ();

  dout<<"time\t"<<(end-start)/float (CLK_TCK)<<endl;
  dout<<"FPS\t"<<float (2*total)/(end-start)*float (CLK_TCK)<<endl;

  dout<<"size of bitmap\t"<<sizeof (Bitmap)<<endl;

  glBegin           (GL_QUADS);
    glNormal3f (0,0,-1);
    glTexCoord2f (0,1);
    glVertex3f (-5,-5,0);
    glTexCoord2f (1,1);
    glVertex3f (5,-5,0);
    glTexCoord2f (1,0);
    glVertex3f (5,5,0);
    glTexCoord2f (0,0);
    glVertex3f (-5,5,0);
  glEnd             ();

  mat1.params.min_filter = TEXTURE_2D_NEAREST;
  mat1.params.mag_filter = TEXTURE_2D_NEAREST;
  mat1.ApplyChanges ();

  render.mtls.SetMaterial (mat1);

  glBegin           (GL_QUADS);
    glNormal3f (0,0,-1);
    glTexCoord2f (0,1);
    glVertex3f (-5,-5,0);
    glTexCoord2f (1,1);
    glVertex3f (5,-5,0);
    glTexCoord2f (1,0);
    glVertex3f (5,5,0);
    glTexCoord2f (0,0);
    glVertex3f (-5,5,0);
  glEnd             ();

  render.EndDraw    ();

  MainLoop          ();

  return EXIT_SUCCESS;
}
