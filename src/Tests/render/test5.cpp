#include <kernel.h>
#include <render\render.h>
#include <render\nodepass.h>
#include <render\startup.h>
#include <kernel\app.h>

#include <render\interface\helpers.h>

#include <gl\gl.h>
#include <gl\glu.h>

Render* rPtr = NULL;
Viewport* vpPtr = NULL;
RenderHelpers* hPtr = NULL;
Camera* cam = NULL;

void     DoTest ();
void     Idle   ();

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
  RenderHelpers help;  
  FreeCamera    camera;

  vp.GetProj ().SetOrtho (-10,10,-10,10,0,100);
//  vp.GetProj ().SetPerspective (-10,10,-10,10,1,100);
  vp.BindCamera (camera);

  render.SetContext (&startup);
  render.RegisterInterface (help);

  render.SetViewport(&vp);

  glClearColor      (0,0,0,0);
  glClear           (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable          (GL_DEPTH_TEST);

  rPtr = &render;
  vpPtr = &vp;
  hPtr = &help;
  cam = &camera;

  MainLoop          (Idle);
}

void     Idle   ()
{
  static float angle = 0;

//  cam->SetLookAt (Vector3 (sin (angle) * 20.0f,0,cos (angle) * 20.0f),Vector3 (.0f),Vector3 (0,1,0));  
//  cam->Rotate (EulerAngle (0,0,5));

  glPushMatrix ();
  cam->SetLookAt (Vector3 (3,3,3),Vector3 (.0f),Vector3 (0,1,0));  

  angle += 0.001;

  rPtr->BeginDraw ();
  rPtr->SetViewport (vpPtr);
  Matrix m (1);

  glGetFloatv (GL_MODELVIEW_MATRIX,(GLfloat*)&m);

  dout<<m<<endl;
  glClear         (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  hPtr->DrawDummy    (Color3 (1,0,0));
  hPtr->DrawSphere   (Vector3 (.0f),1,Color3 (0,1,0));
  hPtr->DrawCylinder (Color3 (0,0,1));
//  help.DrawCylinder (Vector3 (.0f),Vector3 (0,0,1),2,Color3 (0,1,0));
//  help.DrawCone     (Vector3 (.0f),Vector3 (0.2,0,-1),M_PI/3.0f,Color3 (0,1,0));
//  help.DrawPointer  (Vector3 (.0f),Vector3 (0,0,-1),Color3 (0,1,0));
  hPtr->DrawBasis    ();

  rPtr->EndDraw   ();

  glPopMatrix ();
}