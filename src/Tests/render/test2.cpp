#include <kernel.h>
#include <render\render.h>
#include <render\nodepass.h>
#include <render\startup.h>
#include <kernel\app.h>

#include <render\interface\helpers.h>

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

  RenderStartup startup;
  Render        render;    
  Viewport      vp;
//  RenderNodesPass nodesPass;  
  RenderHelpers help;  

  vp.GetProj ().SetOrtho (-10,10,-10,10,0,100);
//  vp.GetProj ().SetPerspective (-10,10,-10,10,1,100);

  render.SetContext (&startup);
  render.RegisterInterface (help);

  render.SetViewport(&vp);
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

  help.DrawDummy    (Color3 (1,0,0));
  help.DrawSphere   (Vector3 (.0f),1,Color3 (0,1,0));
  help.DrawCylinder (Color3 (0,0,1));
//  help.DrawCylinder (Vector3 (.0f),Vector3 (0,0,1),2,Color3 (0,1,0));
//  help.DrawCone     (Vector3 (.0f),Vector3 (0.2,0,-1),M_PI/3.0f,Color3 (0,1,0));
//  help.DrawPointer  (Vector3 (.0f),Vector3 (0,0,-1),Color3 (0,1,0));
  help.DrawBasis    ();

  dout<<startup.ogl.extensions<<endl;

  render.Draw       ();

  MainLoop          ();
}
