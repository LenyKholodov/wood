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
Render* renders [2] = {NULL};
RenderStartup* sPtr = NULL;
RenderNode* nPtr = NULL;

int main (int argc,char* argv [])
{
  K_FastInit (NULL);

  DoTest ();

  return EXIT_SUCCESS;
}

Render* CreateRender (Viewport& vp,RenderNode& node,int left,int top)
{
  RenderStartup*   startup = new RenderStartup;
  Render*          render  = new Render;
  RenderNodesPass* pass    = new RenderNodesPass;
  RenderHelpers*   help    = new RenderHelpers;

  startup->window.left     = left;
  startup->window.top      = top;
  startup->window.width    = 400;
  startup->window.height   = 600;
  startup->window.type     = RW_STD_WINDOW;
  startup->window.mousePresent = true;

  pass->SetRootNode (&node);
  pass->SetViewport (&vp);  

  if (render->LockAccess ())
  {
    render->SetContext          (startup);
    render->RegisterInterface   (*help);
    render->AddPass             (*pass);  

    glClearColor                (0,0,0,0);
    glClear                     (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable                    (GL_DEPTH_TEST);

    render->UnlockAccess ();
  }  

  return render;
}

void     DoTest ()
{
  SetRenderPool (M_GetPool ());

  Viewport      vp, vp1;    
  RenderNode    node;
  HelperBasis   basis;

  node.BindObject (basis);

  node.Translate  (Vector3 (0,0,-25));
  node.Scale      (Vector3 (2,2,2));
  node.Rotate     (Vector3 (0,1,0),-M_PI/3.0f);
  node.Rotate     (Vector3 (1,0,0),M_PI/3.0f);

  nPtr = &node;

  vp.GetProj ().SetOrtho     (-10,10,-10,10,0,100);
  vp1.GetProj ().SetFrustum  (60,60,20,100);

  renders [0] = CreateRender (vp,node,0,0);
  renders [1] = CreateRender (vp1,node,400,0);

  MainLoop          (Idle);
}

void    Idle ()
{
  static int r = 0;

  rPtr = renders [(r++)&1];
  
  nPtr->Rotate (Vector3 (0,1,0),M_PI/600.0);

  rPtr->BeginDraw  ();

  glClear          (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  rPtr->DrawPasses ();

  rPtr->EndDraw    ();
}
