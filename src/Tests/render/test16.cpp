#include <kernel.h>
#include <time.h>

#include <render\pass.h>
#include <render\primitive.h>

#include <gl\gl.h>
#include <gl\glut.h>

using namespace Render;

class TestPrimitive: public Primitive
{
  public:
                TestPrimitive (float x,float y,float z):pos (x,y,z) {}

    Vector3 pos;

    virtual void DrawPrimitive (const Context&)
    {
      glPushMatrix ();
      glTranslatef (pos.x,pos.y,pos.z);

      glBegin (GL_TRIANGLES);
        glVertex3f (-10,-10,0);
        glVertex3f ( 10,-10,0);
        glVertex3f (  0, 10,0);
      glEnd   ();

      glPopMatrix ();
    }
};

class TestState: public State
{
  public:
    TestState (int i,int cnt=1): test (i), State (cnt) {}

    int test;

    void ApplyState (pass_t pass) {
      dout<<"...state "<<test<<" pass "<<(int)pass<<endl;
    }
};

class ColorState: public State
{
  public:
    Color4 color;
           ColorState (const Color4& c): color (c) {}
    void ApplyState (pass_t) {
      glColor4fv ((GLfloat*)&color);
    }
};

static TestPrimitive p1 (0,0,-10), p2 (5,3,-15);
static TestState     s1 (1), s2 (2,2), s3 (3,3), s4 (4,2);
static ColorState    c1 (Color4 (1,0,0,0.5)), c2 (Color4 (0,0,1,0.5));

float glod = 1.0;
Pass* pass = NULL;

void reshape (int,int);
void draw ();

static total = 0;

void main (int argc,char* argv [])
{
  K_FastInit (NULL);

  glutInit (&argc,argv);
  glutInitWindowSize (800,600);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE);
  glutCreateWindow ("RenderPass\\RenderSorter\\RenderPrimitive\\RenderState test");
  glutReshapeFunc (reshape);  
  glutDisplayFunc (draw);

  glClearColor (0,0.4,0.4,0);
  glEnable (GL_DEPTH_TEST);    
  glOrtho  (-20,20,-20,20,-100,100);

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  Sorter sorter;

//  pass = new RenderPass (8,1024,glod);
//  pass = new RenderPass (2,16*1024,glod);
  pass = new Pass (1,16*1024,sorter);

  uchar order [2] = {0};

/*  MultiState ms;

  ms.AddState (s1);
  ms.AddState (s2);
  ms.AddState (s3);
  ms.AddState (s4);


  uint count = ms.GetPassCount ();

  for (int i=0;i<count;i++)
    ms.ApplyState (i);

*/

//  pass->SetPassState (&ms);
//  pass->SetSortOrder (1,order);
  pass->SetDistanceMode (RP_BACK_TO_FRONT);
  pass->Enable          (0);

//  pass->AddPassState (s3);    
//  pass->AddPassState (s4);    

/*  const uint total = 1000; 
  uint  pcount = 0;

  clock_t start = clock ();

  for (int i=total;i--;)
  {
//    float depth = 1.0;

//    for (int i=0;i<10;i++,depth-=0.1)
    for (int i=0;i<1024;i++)
    {
//      pass->SetDepth (depth);
      
      pass->SetState (0,s1);
      pass->DrawPrimitive (p [0]);

      pass->SetState (0,s2);
      pass->DrawPrimitive (p [0]);

      pass->SetState (0,s3);
      pass->DrawPrimitive (p [0]);

      pass->SetState (0,s4);
      pass->DrawPrimitive (p [0]);

      pcount+=4;
    }  

//    dout<<"flush"<<endl;

    pass->Flush ();
  }

  clock_t end = clock ();

  dout<<"FPS:\t"<<float (pcount)/float (end-start)*float (CLK_TCK)<<endl;
  dout<<"FPS1:\t"<<float (total)/float (end-start)*float (CLK_TCK)<<endl;

*/

  glutMainLoop ();
}

void draw ()
{
  glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);  

  Matrix view, proj, tm;

  glGetFloatv (GL_MODELVIEW_MATRIX,(GLfloat*)&view);  
  glGetFloatv (GL_PROJECTION_MATRIX,(GLfloat*)&proj);  

  tm = view * proj;
  tm.transpose ();

  pass->SetDepth ((tm*p1.pos).z);
  pass->SetState (0,c1);
  pass->DrawPrimitive (p1);

  pass->SetDepth ((tm*p2.pos).z);
  pass->SetState (0,c2);
  pass->DrawPrimitive (p2);

  pass->Flush ();

  glutSwapBuffers ();
}

void reshape (int w,int h)
{
  glViewport (0,0,w,h);
}
