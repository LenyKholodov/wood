#include <kernel.h>
#include <render\render.h>
#include <render\nodepass.h>
#include <render\startup.h>
#include <kernel\app.h>

#include <render\interface\primitives.h>
#include <render\primitives.h>

#include <gl\gl.h>

void     DoTest ();
void     Idle   ();

int main (int argc,char* argv [])
{
  K_FastInit (NULL);

  DoTest ();

  return EXIT_SUCCESS;
}

RenderNode* test = NULL;
Render*     r    = NULL;

void     DoTest ()
{
  SetRenderPool (M_GetPool ());

  Render        render;    
  RenderStartup startup;
  Viewport      vp;
  RenderPrimitives prim; 
  RenderNodesPass nodeRender;
  FreeCamera      cam;
  RenderNode    node;

  render.RegisterInterface (prim);

  r = &render;

  PBox box;  

  nodeRender.SetRootNode (&node);
  nodeRender.SetViewport (&vp);

//  node.AddChild (box,Matrix::translate (Vector3 (0,0,1)));
  test = node.AddChild (box,Matrix::scale (Vector3 (3,3,3)));
  test = test->AddChild (box,Matrix::translate (Vector3 (0.1,0.1,0.1)));

  test = &node;

//  cam.Move (Vector3 (0,0,-15));
  cam.SetLookAt (Vector3 (0,0,-15),Vector3 (0,0,0),Vector3 (1,1,1));

  vp.GetProj ().SetOrtho (-10,10,-10,10,0,10000);
//  vp.GetProj ().SetPerspective (-10,10,-10,10,1,100);
//  vp.GetProj ().SetFrustum (60,60,1,1000);
  vp.BindCamera (cam);

  render.AddPass (nodeRender);

  render.SetContext (&startup);

//  glEnable (GL_NORMALIZE);

  render.SetViewport(&vp);
                        
  glColor3f         (1,0,0);
  glClearColor      (0,0,0,0);
  glClear           (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable          (GL_DEPTH_TEST);

  glEnable          (GL_CULL_FACE);

  MainLoop          (Idle);
}

void Idle ()
{
  r->BeginDraw      ();

  glClear           (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glPolygonMode     (GL_FRONT_AND_BACK,GL_LINE);

//  test->Translate   (Vector3 (0.1,0,0));
  test->Rotate      (Vector3 (1,0,0),3.14/120.0f);

  r->DrawPasses     ();

  r->EndDraw        ();
}
