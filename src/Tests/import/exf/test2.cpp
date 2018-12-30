#include <kernel.h>
#include <graph\wxfimp.h>
#include <ogl.h>
#include <kernel/app.h>

WXFImport*      LoadModel (const char*);
Window*         InitGraph (const char* wndName,int width = 800,int height = 600);
void            DoneGraph (Window*);
void            Idle      ();

boundsphere_t   GetBound  (WXFImport*);
void            Positionize ();

WXFImport*      mdl = NULL;

int main (int argc,char* argv [])
{
  mdl = LoadModel ("q:/tests/3dsmax/data/1.wxf");

  if (!mdl->lock ())
    return EXIT_FAILURE;

  dout<<"load ok!"<<endl;

  Window* wnd = InitGraph ("WXF export test");
  Positionize ();

  dout<<"at main loop->"<<endl;

  MainLoop              (Idle);

  mdl->unlock ();

  DoneGraph (wnd);

  delete mdl;

  return EXIT_SUCCESS;  
}

WXFImport*      LoadModel (const char* name)
{
  return new (M_GetPool ()) WXFImport (name);  
}

void     Reshape (int width,int height)
{
  glViewport (0,0,width,height);
}

void    Draw (WXFMesh* mesh)
{
  if (mesh->vertexes.verts && mesh->faces.faces)
  {
    vertex_t*    vertex  = mesh->vertexes.verts;
    vector3d_t*  normals = mesh->vertexes.normals;
    WXFFace*     face    = mesh->faces.faces;

    if (normals)
    {
      glBegin (GL_TRIANGLES);
        for (int i=0;i<mesh->faces.count;i++,face++)
        {
          for (int j=0;j<3;j++)
          {
            glNormal3fv ((GLfloat*)&normals [face->index [j]]);
            glVertex3fv ((GLfloat*)&vertex [face->index [j]]);
          }  
        }    
      glEnd   ();
    }
    else
    {
      glBegin (GL_TRIANGLES);
        for (int i=0;i<mesh->faces.count;i++,face++)
        {
          glNormal3fv ((GLfloat*)&face->n);
          for (int j=0;j<3;j++)
            glVertex3fv ((GLfloat*)&vertex [face->index [j]]);
        }    
      glEnd   ();
    }
  }
}

void     SetMaterial (WXFMaterial* mtl)
{
  glMaterialfv  (GL_FRONT_AND_BACK,GL_AMBIENT,(GLfloat*)&mtl->ambient);
  glMaterialfv  (GL_FRONT_AND_BACK,GL_DIFFUSE,(GLfloat*)&mtl->diffuse);
  glMaterialfv  (GL_FRONT_AND_BACK,GL_SPECULAR,(GLfloat*)&mtl->specular);
  glMaterialf   (GL_FRONT_AND_BACK,GL_SHININESS,mtl->shininess);  
}

void     Draw    (WXFNode* node)
{
  bool colMat = false;

  if (node->mtl)
    SetMaterial (node->mtl);
  else
  {
    glEnable         (GL_COLOR_MATERIAL);
    glColorMaterial  (GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glColor3fv       ((GLfloat*)&node->wireColor);

    colMat = true;
  }  

  switch (node->type)
  {
    case WXFNode::MESH:
         Draw (node->data.mesh);
         break;         
  }

  if (colMat)
    glDisable (GL_COLOR_MATERIAL);

  Ring<WXFNode*>::iter iter  = node->children;
  uint                 count = node->children.count ();

  for (int i=0;i<count;i++,iter++)
    Draw (iter);
}

void     Draw    (WXFImport* mdl)
{
  glClear      (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix ();
//  SetCamera    ();
  Draw         (mdl->root ());  

  glPopMatrix  ();
  glFinish     ();

  OGL_SwapBuffers ();
}


Window*    InitGraph (const char* wndName,int w,int h)
{
  Application*  app = A_GetApplication ();       
  Window*       wnd = ext_new SimpleWindow (M_GetPool (),
                                       rect_t (0,0,w,h),
                                       wndName
                                      );

  if (!wnd)
    return NULL;

  SrvMakeCurrent        (OGL_Load  ());
  OGL_DisplayMode       (OGL_DOUBLE | OGL_DEPTH | OGL_RGB);
  OGL_ReshapeFunc       (Reshape);
  OGL_CreateContext     (OGL_CONTEXT0,wnd);
  OGL_Start             (OGL_RESHAPE_CALL);

  wnd->InsertHandler    (WM_DESTROY,WND_Done);

  glMatrixMode          (GL_PROJECTION);
  glLoadIdentity        ();
//  glOrtho               (-80,80,-30,100,1.0,4000.0);   //???

  glMatrixMode          (GL_MODELVIEW);
  glLoadIdentity        ();

  glClearColor          (0,0,0.4,1.0);
  glClear               (GL_COLOR_BUFFER_BIT);

  glShadeModel          (GL_SMOOTH);

  glEnable              (GL_DEPTH_TEST);

//  glPolygonMode         (GL_FRONT_AND_BACK,GL_LINE);

  glEnable              (GL_LIGHTING);
  glEnable              (GL_LIGHT0);

  float k = 0.2;

/*  GLfloat diffuse  [4] = {k,k,k,1};
  GLfloat specular [4] = {k,k,k,1};
  GLfloat ambient  [4] = {k,k,k,1};
  GLfloat dir      [4] = {  0,  0,-1,0};  

  glLightfv        (GL_LIGHT0,GL_DIFFUSE,diffuse);
  glLightfv        (GL_LIGHT0,GL_AMBIENT,ambient);
  glLightfv        (GL_LIGHT0,GL_SPECULAR,specular);
  glLightfv        (GL_LIGHT0,GL_SPOT_DIRECTION,dir);
  glLightf         (GL_LIGHT0,GL_SPOT_CUTOFF,90.0f);  */

  return wnd;
}

void    DoneGraph (Window* wnd)
{
  delete wnd;
  OGL_Done  ();
}

void Idle ()
{
  Draw (mdl);
}

void            GetBound  (WXFMesh& mesh,boundbox_t& box)
{
  vector3d_t c (.0f);

  for (int i=0;i<mesh.vertexes.count;i++)
    c += mesh.vertexes.verts [i];  

  c /= float (mesh.vertexes.count);

  vector3d_t dim (.0f);

  for (i=0;i<mesh.vertexes.count;i++)
  {
    vector3d_t& v = mesh.vertexes.verts [i];
    vector3d_t  d = abs (c - v);

    if (d.x > dim.x) dim.x = d.x;
    if (d.y > dim.y) dim.y = d.y;
    if (d.z > dim.z) dim.z = d.z;    
  }

  boundbox_t b;

  b.min = c - dim;
  b.max = c + dim;

  if (b.max.x > box.max.x) box.max.x = b.max.x;
  if (b.max.y > box.max.y) box.max.y = b.max.y;
  if (b.max.z > box.max.z) box.max.z = b.max.z;

  if (b.min.x < box.min.x) box.min.x = b.min.x;
  if (b.min.y < box.min.y) box.min.y = b.min.y;  
  if (b.min.z < box.min.z) box.min.z = b.min.z;
} 

void    GetBound (WXFNode* node,boundbox_t& box)
{
  if (node->type == WXFNode::MESH)
    GetBound (*node->data.mesh,box);

  Ring<WXFNode*>::iter iter  = node->children;
  uint                 count = node->children.count ();

  for (int i=0;i<count;i++,iter++)
    GetBound (iter,box);  
}

boundsphere_t      GetBound  (WXFImport* mdl)
{
  boundbox_t     box;
  boundsphere_t  s;

  box.min = vector3d_t (10000000.0f);
  box.max = vector3d_t (-10000000.0f);

  GetBound (mdl->root (),box);

  s.center = (box.max + box.min) / 2.0f;
  s.r      = length (box.max - box.min) / 2.0f;

  return s;  
}

void   Positionize ()
{
  boundsphere_t s = GetBound (mdl);
  vector3d_t&   c = s.center;
  float         r = s.r;
  vector3d_t pos (c.x,c.y,c.z+r+1);

  float      len   = r;  
  float      dist  = length (c-pos);
  float      zmin  = dist - r,
             zmax  = dist + r;
  float      k     = zmin / dist;
  len             *= k;

  glMatrixMode   (GL_PROJECTION);
  glLoadIdentity ();
  glFrustum      (-len,len,-len,len,zmin,zmax);  

  glMatrixMode   (GL_MODELVIEW);
  glLoadIdentity ();
  gluLookAt      (pos.x,pos.y,pos.z,
                  c.x,c.y,c.z,
                  0,1,0
                 );
}
