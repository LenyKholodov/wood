#include <kernel.h>
#include <render\render.h>
#include <render\startup.h>
#include <kernel\app.h>

#include <render\interface\primitives.h>
#include <render\texdefines.h>

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
  RenderPrimitives help;  

  cam.proj.SetOrtho (-10,10,-10,10,0,100);
//  vp.GetProj ().SetPerspective (-10,10,-10,10,1,100);

  cam.Move (Vector3 (0,0,-5));

  render.SetContext (&startup);
  render.RegisterInterface (help);

  render.SetCamera (&cam);
//  render.AddPass    (nodesPass);  

  glColor3f         (1,0,0);
  glClearColor      (0,0,0,0);
  glClear           (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable          (GL_DEPTH_TEST);

  glTranslatef      (0,0,-5);
  glRotatef         (30,0,1,0);
  glRotatef         (30,1,0,0);
  glScalef          (4,4,4);

  glEnable          (GL_CULL_FACE);
//  glEnable          (GL_LIGHTING);
  glEnable          (GL_LIGHT0);

  GLfloat spec [4] = {1,1,1,1};
  GLfloat diff [4] = {1,1,1,1};

  glLightfv         (GL_LIGHT0,GL_SPECULAR,spec);
  glLightfv         (GL_LIGHT0,GL_DIFFUSE,diff);  

//  Bitmap mat ("image.jpg");
//  SingleTexture2D mat ("smap.bmp");
  SingleTexture2D mat ("image.jpg");

  mat.diffuse  = Color4 (1,1,1);
  mat.specular = Color4 (1,1,1);

//  glPolygonMode    (GL_FRONT_AND_BACK,GL_LINE);

  render.BeginDraw ();

//  help.DrawDummy    (mtl);
//  help.DrawCylinder (mtl);
//  help.DrawSphere   (mtl);
  help.DrawCone (&mat);
//  help.DrawTeapot (mtl);

  render.DrawPasses ();

  render.EndDraw   ();

  MainLoop          ();
}
