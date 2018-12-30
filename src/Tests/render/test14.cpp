#include <kernel\app.h>

#include <render\render.h>
#include <render\startup.h>
#include <render\bitmap.h>

#include <gl\gl.h>

int main (int argc,char* argv [])
{
  SetRenderPool (M_GetPool ());

  Render        render;    
  RenderStartup startup;
  FreeCamera    cam;

  cam.Move (Vector3 (5,0,-.4));
//  cam.proj.SetOrtho (-5,5,-5,5,0,100);
  cam.proj.SetPerspective (-5,5,-5,5,.1,100);

  render.context.SetContext (&startup);
  render.viewer.SetCamera  (&cam);

  glClearColor      (0,0,0,0);
  glClear           (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable          (GL_DEPTH_TEST);
  glEnable          (GL_TEXTURE_2D);
  
  MultiMaterial mtl;
  Bitmap        bmp ("image.jpg");

 // bmp.enable = false;
  mtl.AddSubMaterial (bmp);

  mtl.diffuse  = Color3 (1);
//  mtl.ambient  = Color3 (0.4);
  mtl.specular = Color3 (1);
  mtl.shine    = 0.8;
  mtl.shade    = MTL_SHADE_GOURAND;

  static float spec [] = {1,1,1,1};
  static float pos [] = {0,0,-5,1};
  static float dir [] = {0,0,1,0};

  glLightfv (GL_LIGHT0,GL_SPECULAR,spec);
  glLightfv (GL_LIGHT0,GL_DIFFUSE,spec);
  glLightfv (GL_LIGHT0,GL_AMBIENT,spec);
  glLightfv (GL_LIGHT0,GL_POSITION,pos);
  glLightfv (GL_LIGHT0,GL_SPOT_DIRECTION,dir);

  render.mtls.SetMaterial (mtl);

  glTexEnvi (GL_TEXTURE_2D,GL_TEXTURE_ENV,GL_MODULATE);
//  glTexEnvi (GL_TEXTURE_2D,GL_TEXTURE_ENV,GL_DECAL);

/*  const int total = 1000000;
  float start = clock ();

  for (int i=0;i<total;i++)
  {
    render.mtls.SetMaterial (mtl);
  }  

  float end = clock ();

  dout<<"time\t"<<(end-start)/float (CLK_TCK)<<endl;
  dout<<"FPS\t"<<float (total)/(end-start)*float (CLK_TCK)<<endl;*/
    
  render.BeginDraw  ();

  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);

  render.mtls.SetMaterial (mtl);

  glBegin           (GL_QUADS);
    glNormal3f (0,1,-1);
    glTexCoord2f (0,1);
    glNormal3f (0,-1,-1);
    glVertex3f (-5,-5,0);
    glNormal3f (-3,-3,-1);
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
