#include <kernel.h>
#include <render\render.h>
#include <render\startup.h>
#include <kernel\app.h>

#include <render\interface\helpers.h>
#include <render\interface\primitives.h>
#include <render\interface\text.h>
#include <render\interface\nodes.h>
#include <render\helpers.h>
#include <render\inscription.h>
#include <render\primitives.h>
#include <render\utils\flycamera.h>
#include <render\utils\fpscounter.h>
#include <render\texdefines.h>
#include <render\fontdefs.h>

#include <gl\gl.h>

class Robot;

void     DoTest ();
void     Idle   ();

Render* r = NULL;
RenderHelpers* h = NULL;
Viewport* v = NULL;
Camera*   c = NULL;
RenderNode* root = NULL;
RenderNode* node1 = NULL;
Robot* rob = NULL;

struct Robot
{
  RenderNode   root;
  PCone        dummyHand,dummyLeg, dummyBody;
  PSphere      sphere;
//  PBox         dummyHand,dummyLeg, dummyBody;
  PTeapot      teapot;
  HelperSphere null;

  struct Body
  {
    RenderNode* body;

    struct Hand
    {
      RenderNode* hand;
      
      RenderNode* up;
      RenderNode* mid;
      RenderNode* down;
    }leftHand, rightHand;

    struct Leg
    {
      RenderNode* leg;
      
      RenderNode* up;
      RenderNode* mid;
      RenderNode* down;        
    }leftLeg, rightLeg;
  }body;  

  RenderNode* head;

        Robot (RenderNode&,Material* head,Material* body);

  void  CreateBody (Material* head,Material* body);
  void  CreateHand (Body::Hand&);
  void  CreateLeg  (Body::Leg&);

  void  RotateHands ();
  void  RotateLegs  ();
};

class TestFont: public TrueTypeFont
{
  public:
        TestFont (const char*);

  protected:
    virtual void Bind ();

  private:
    Bitmap      bmp;
};

Robot::Robot (RenderNode& link,Material* head,Material* body)
       : null (Vector3 (.0f),0)
{
  root.SetNodeAABB (AABB (Vector3 (-100,-100,-100),Vector3 (100,100,100)));

  link.AddChild (root);

  CreateBody    (head,body);
}

void  Robot::CreateBody (Material* headMaterial,Material* bodyMaterial)
{
//  dummyLeg.SetColor (Color3 (1,1,1));  
//  dummyBody.SetColor (Color3 (1,0,0));
  dummyLeg.SetMaterial (bodyMaterial);
  dummyBody.SetMaterial (bodyMaterial);
  sphere.SetMaterial (bodyMaterial);
  dummyHand.SetMaterial (bodyMaterial);
  teapot.SetMaterial (headMaterial);

  body.body = root.AddChild  (dummyBody,Matrix::translate (Vector3 (0,10,0))*Matrix::scale (10,25,8));

  head      = root.AddChild  (teapot,Matrix::translate (Vector3 (0,38,0))*Matrix::scale (Vector3 (2,2,2)));

  body.leftHand.hand  = body.body->AddChild (null,Matrix::translate (-.4,.2,-.1));
  body.rightHand.hand = body.body->AddChild (null,Matrix::translate (.4,.2,-.1));

  body.leftLeg.leg    = body.body->AddChild (null);
  body.rightLeg.leg   = body.body->AddChild (null,Matrix::scale (-1,1,1));

  CreateHand (body.leftHand);
  CreateHand (body.rightHand);

  CreateLeg  (body.rightLeg);
  CreateLeg  (body.leftLeg);
}

void  Robot::CreateHand (Body::Hand& hand)
{
  hand.up   = hand.hand->AddChild  (dummyHand,Matrix::scale (Vector3 (.5,.5,.5)));
  hand.mid  = hand.up->AddChild    (dummyHand,Matrix::translate (Vector3 (0,-.5,0))*Matrix::scale (Vector3 (.5,.8,.5)));
  hand.down = hand.mid->AddChild   (sphere,Matrix::translate (Vector3 (0,-.35,0))*Matrix::scale (Vector3 (.7,.3,.7)));
}

void  Robot::CreateLeg  (Body::Leg& leg)
{
  leg.up   = leg.leg->AddChild  (dummyLeg,Matrix::translate (Vector3 (.15,-.35,0))*Matrix::scale (.4,.4,.4));
  leg.mid  = leg.up->AddChild   (dummyLeg,Matrix::translate (Vector3 (0,-.45,0))*Matrix::scale (Vector3 (.5,1,.5)));
  leg.down = leg.mid->AddChild  (dummyLeg,Matrix::translate (Vector3 (0,-.32,0))*Matrix::scale (Vector3 (1.2,.3,1.7)));
}

void Robot::RotateHands ()
{  
  body.leftHand.hand->Rotate (Vector3 (-1,0,0),0.05);
  body.rightHand.hand->Rotate (Vector3 (-1,0,0),-0.025);
}

void Robot::RotateLegs  ()
{
  body.leftLeg.leg->Rotate (Vector3 (1,0,0),0.05);
  body.rightLeg.leg->Rotate (Vector3 (-1,0,0),-0.05);
}

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
  FPSCounter    fps;
  RenderNodes   nodeRender;
  FlyCamera     cam (render);
  RenderNode    node;

//  startup.window.type = RW_STD_WINDOW;
  //startup.screen.fullScreen = true;

  render.context.SetContext (&startup);

  render.RegisterInterface (nodeRender);

  render.passes.AddPass    (fps,RP_DRAW_PASS);

//  startup.screen.fullScreen = true;

  TestFont        font ("image.jpg");
  Inscription     msg ("Привет мир!",&font);  

  font.charset = RUSSIAN_CHARSET;
  font.face = "Comic Sans MS";

  node.SetNodeAABB (AABB (Vector3 (-1000,-1000,-1000),Vector3 (1000,1000,1000)));

//  startup.screen.fullScreen = true;

  nodeRender.SetRootNode (&node);
//  node.Translate  (Vector3 (10,10,10));

  SingleTexture2D mat ("smap.bmp");
  SingleTexture2D mat1 ("image.jpg");

  mat.diffuse  = Color4 (1,1,1);
  mat.specular = Color4 (1,1,1);

  Robot robot (node,&mat,&mat1);

  node.AddChild (msg,Matrix (1));

  rob = &robot;

  r = &render;
  root = &node;
  c = &cam;

//  vp.GetProj ().SetOrtho (-10,10,-10,10,-5,10000);
//  vp.GetProj ().SetPerspective (-10,10,-10,10,1,100);
  cam.proj.SetFrustum (60,60,1,1000);

//  glEnable (GL_NORMALIZE);

  render.viewer.SetCamera (&cam);
                        
  cam.BindWindow (*startup.GetWindow ());

/*  const int total = 10000000;
  float start = clock ();

  for (int i=0;i<total;i++)
  {    
//    float dist = render.viewer.GetDist (Vector3 (0.0f));
    float angle = render.viewer.GetCos (Vector3 (0,1,0));
  }  

  float end = clock ();

  dout<<"time\t"<<(end-start)/float (CLK_TCK)<<endl;
  dout<<"FPS\t"<<float (2*total)/(end-start)*float (CLK_TCK)<<endl;*/

  glColor3f         (1,0,0);
  glClearColor      (0,0,0.5,0);
  glClear           (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable          (GL_DEPTH_TEST);

  glEnable          (GL_CULL_FACE);

  dout<<"in"<<endl;

  MainLoop          (Idle);

  dout<<"exit"<<endl;
}

void     Idle   ()
{
  r->BeginDraw      ();

  r->viewer.SetCamera      (c);

  rob->RotateHands  ();
//  rob->RotateLegs   ();

//  root->Translate   (Vector3 (-4,0,0));
  root->Rotate      (Vector3 (0,1,0),.005);
//  root->Translate   (Vector3 (4,0,0));

//  root->Rotate      (EulerAngle (0.32,0.32,0.32));
//  node1->Rotate     (EulerAngle (1,-1,1));

//  h->DrawDummy    (Vector3 (5,0,0),Vector3 (1,2,3),Color3 (1,0,0));
//  h->DrawSphere   (Vector3 (0,0,-5),1,Color3 (0,1,0));
//  h->DrawCylinder (Color3 (0,0,1));
//  help.DrawCylinder (Vector3 (.0f),Vector3 (0,0,1),2,Color3 (0,1,0));
//  help.DrawCone     (Vector3 (.0f),Vector3 (0.2,0,-1),M_PI/3.0f,Color3 (0,1,0));
//  help.DrawPointer  (Vector3 (.0f),Vector3 (0,0,-1),Color3 (0,1,0));
  r->helpers.DrawBasis    (5);
  r->helpers.DrawGrid     (-50,-50,12,12,-15,8.0f,Color3 (1,1,1));
  r->DrawPasses   ();

  dout<<"Dist:\t"<<r->viewer.GetDist (Vector3 (0.0f))<<endl;

  r->EndDraw      ();
}

void TestFont::Bind ()
{
  TrueTypeFont::Bind ();

  glEnable (GL_TEXTURE_2D);
  glEnable   (GL_TEXTURE_GEN_S);
  glEnable   (GL_TEXTURE_GEN_T);

  static GLfloat s_params[4] = {1.0f,1.0f,0.0f,1};
  static GLfloat t_params[4] = {0.817f,-0.817f,0.817f,1};

  glTexGeni  (GL_S,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
  glTexGeni  (GL_T,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
  glTexGenfv (GL_S,GL_OBJECT_PLANE,s_params);
  glTexGenfv (GL_T,GL_OBJECT_PLANE,t_params);


  glTexGeni  (GL_S,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
  glTexGeni  (GL_T,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
  glTexEnvi  (GL_TEXTURE_2D,GL_TEXTURE_ENV,GL_REPLACE);

  GetRender ()->mtls.SetMaterial (bmp);
}

TestFont::TestFont (const char* name) : bmp (name)
{   }