#define  DEBUG
#include <graph\anim.h>
#include <kernel\app.h>
#include <ogl.h>
#include <graph\textenv.h>
#include <graph\utils.h>
#include <misc.h>
#include <string.h>

Object3D* obj   = NULL;
int       frame = 0;

void     Idle     ();
void     Reshape  (int width,int height);
void     Draw     ();
void     Draw     (Object3D::Node);
void     Reshape  (int,int);

const    char*    FileName = "tank.3ds";
int      total             = 0;

int main (int,char* [])
{
  static Object3D  obj3ds (M_GetPool(),FileName);
  obj         = &obj3ds;

  if (!IsValid (obj))
  {
    char name [128];

    strcpy (name,"Error at load file: '");
    strcat (name,FileName);
    strcat (name,"'.\n Program abort!");
    
//    MessageBox (NULL,name,"Error!",MB_OK | MB_DEFBUTTON1);

    return EXIT_FAILURE;
  }

  Application*  app = A_GetApplication ();       
  Window*       wnd = IF_CreateWindow (SIMPLE_WINDOW,
                                       M_GetPool (),
                                       app->instance (),
                                       rect_t (0,0,800,600),
                                       "3DS Animation Test",
                                       NULL
                                      );

  OGL_Load              ();
  OGL_DisplayMode       (OGL_DOUBLE | OGL_DEPTH | OGL_RGB);
  OGL_ReshapeFunc       (Reshape);
  OGL_DrawFunc          (Draw);
  OGL_CreateContext     (OGL_CONTEXT0,wnd);
  OGL_Start             (OGL_RESHAPE_CALL);

  wnd->InsertHandler    (WM_DESTROY,WND_Done);

  MainLoop              (Idle);

  delete wnd;

  OGL_Done              ();


  return EXIT_SUCCESS;
}

void     Reshape (int width,int height)
{
  const vector3d_t eye (0,0,-200);
  const vector3d_t dir (0,0,1);

  glViewport       (0,0,width,height);
  glMatrixMode     (GL_PROJECTION);
  glLoadIdentity   ();
  glOrtho          (-80,80,-30,100,1.0,4000.0);  
//  glOrtho          (-200,200,-240,400,1.0,4000.0);  
//  glOrtho          (-80,80,-50,100,1.0,4000.0);  
//  glOrtho          (-400,400,-100,800,1.0,4000.0);  

  glMatrixMode     (GL_MODELVIEW);
  glLoadIdentity   ();
  gluLookAt        (eye.x,eye.y,eye.z,
                    dir.x,dir.y,dir.z,
                    0,1,0);    

  glClearColor     (0.5,0.5,0.5,1.0);
  glClear          (GL_COLOR_BUFFER_BIT);

  glShadeModel     (GL_SMOOTH);
//  glPolygonMode    (GL_FRONT_AND_BACK,GL_LINE);

  glEnable         (GL_DEPTH_TEST); 
  glEnable         (GL_COLOR_MATERIAL);
  glEnable         (GL_LIGHTING);
  glEnable         (GL_LIGHT0);
//  glEnable         (GL_TEXTURE_2D);

  GLfloat lpos [4] = {eye.x,eye.y,eye.z,1.0f};
  GLfloat ldir [4] = {dir.x,dir.y,dir.z,1.0f};  

  glLightfv        (GL_LIGHT0,GL_POSITION,lpos);
  glLightfv        (GL_LIGHT0,GL_SPOT_DIRECTION,ldir);
  glLightf         (GL_LIGHT0,GL_SPOT_CUTOFF,90.0f);  

  glColorMaterial  (GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
}

void     Draw    ()
{
  glClear     (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Draw        (obj->main());  

  glFinish    ();

  OGL_SwapBuffers ();

  total++;
}

void     Draw    (Object3D::Node node)
{
  matrix_t m (1), parent (1);  

  if (AM_GetPosMatrix (frame,node,&m,NULL,&parent))
  {
    glPushMatrix ();
    m.transpose  ();
    glMultMatrixf ((GLfloat*)&m);
  }  
  else
    glPushMatrix ();

  if (node.vertex() && node.facet())
  {
    vertex_t*    vertex = node.vertex ();
    facet_t*     facet  = node.facet  ();
    vector3d_t*  norm   = node.normal ();     

    const material_t* mat  = &node.owner()->GetMaterial(facet [0].texture).m;    

    if (mat) glColor3fv ((GLfloat*)&mat->diffuse);
//    if (mat) SetMaterial (*mat);
 
    glBegin (GL_TRIANGLES);
      for (int i=0;i<node.count (Object3D::FACET);i++,facet++)
      {
        glNormal3fv ((GLfloat*)&facet->n);
        for (int j=0;j<3;j++)
        {
          vertex_t    v = vertex [facet->index [j]];
          vector3d_t  n = norm   [facet->index [j]];

          glNormal3fv   ((GLfloat*)&n);
          glVertex3fv   ((GLfloat*)&v);
          glTexCoord2fv ((GLfloat*)&facet->tindex [j]);
        }  
      }
    glEnd   ();
  }

  glPopMatrix ();
  glPushMatrix ();
  parent.transpose  ();
  glMultMatrixf ((GLfloat*)&parent);

  for (Object3D::Node iter = node.node();iter.valid();iter=iter.next ())
    Draw (iter);

  glPopMatrix ();
}

void Idle ()
{
  static clock_t time = clock ();
  static clock_t last = clock ();

  if (clock () - time > CLK_TCK >> 5)
  {
    if (++frame == 100) frame = 0;
    time = clock ();
  }

  if (clock () - last > CLK_TCK)
  {
    dout<<"FPS: "<<float(total*CLK_TCK)/float (clock ()-last)<<endl;
    total = 0;
    last  = clock ();
  }

  Draw ();
}

