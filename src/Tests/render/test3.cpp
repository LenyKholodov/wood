#include <kernel.h>
#include <render\render.h>
#include <render\nodepass.h>
#include <render\startup.h>
#include <kernel\app.h>

#include <render\interface\helpers.h>
#include <render\helper.h>

#include <gl\gl.h>

void     DoTest ();
void     Idle   ();

Render* rPtr = NULL;
RenderStartup* sPtr = NULL;
RenderNode* nPtr = NULL;

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
  RenderNodesPass nodesPass;  
  RenderHelpers help;  
  RenderNode    node;
  HelperBasis   basis;

  node.BindObject (basis);

  node.Translate  (Vector3 (0,0,-50));
  node.Scale      (Vector3 (2,2,2));
  node.Rotate     (Vector3 (0,1,0),-M_PI/3.0f);
  node.Rotate     (Vector3 (1,0,0),M_PI/3.0f);

  nodesPass.SetRootNode (&node);
  nodesPass.SetViewport (&vp);

  vp.GetProj ().SetOrtho (-10,10,-10,10,0,100);
//  vp.GetProj ().SetPerspective (-10,10,-10,10,1,100);

  render.SetContext (&startup);
  render.RegisterInterface (help);

  render.SetViewport(&vp);
  render.AddPass    (nodesPass);  

  rPtr = &render;
  sPtr = &startup;
  nPtr = &node;

  glClearColor      (0,0,0,0);
  glClear           (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable          (GL_DEPTH_TEST);

  MainLoop          (Idle);
}

void    Idle ()
{
  glClear     (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  rPtr->Draw  ();

  nPtr->Translate  (Vector3 (0,0,-50));
  nPtr->Scale      (Vector3 (2,2,2));
  nPtr->Rotate     (Vector3 (0,1,0),M_PI / 1800.0);
  nPtr->Scale      (Vector3 (.5f,.5f,.5f));
  nPtr->Translate  (Vector3 (0,0,50));

/*  static int counter = 0;

  if (counter++ == 140)
  {
    counter = 0;
    sPtr->window.width = random (1024);
    sPtr->window.height  = random (768);
    sPtr->Apply ();
  }*/
}
