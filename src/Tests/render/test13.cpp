#include <kernel.h>
#include <render\render.h>
#include <render\startup.h>
#include <kernel\app.h>

#include <render\interface\nodes.h>
#include <render\interface\text.h>
#include <render\utils\flycamera.h>
#include <render\utils\fpscounter.h>

#include <render\texdefines.h>
#include <render\vbdefs.h>
#include <render\ibdefs.h>

#include <gl\gl.h>

#define getOffs(X,Y) ((ulong)&((X*)NULL)->Y)

const   int     N1 = 30;
const   int     N2 = 30;
const   double  FL_PI = 3.1415926;

struct vrtx_t
{
  vertex_t    v;
  vector3d_t  n;
  Color3      c;   

  struct tex_t
  {
    float u,v;
  }tex;
};

void     DoTest ();
void     Idle   ();

Render*   r = NULL;
Camera*   c = NULL;
SingleVB* vb = NULL;
SingleIB* ib = NULL;
Material*  m = NULL;
Texture2D* t1 = NULL;
Texture2D* t2 = NULL;

void     InitTorus ();

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
  FlyCamera     cam (render);
  RenderText    text; 
  StaticVB      vbuf;
  StaticIB      ibuf;

  startup.screen.fullScreen = true;

  render.context.SetContext  (&startup);

  render.passes.AddPass (fps,RP_DRAW_PASS);

  Bitmap tex0 ("atoll.bmp");
  Bitmap tex1 ("fog.bmp");
  CompositeMaterial mtl;

  tex0.SetTexNumber (0);
  tex1.SetTexNumber (1);

  mtl.AddSubMaterial (tex0);
  mtl.AddSubMaterial (tex1);
  
  r = &render;
  c = &cam;
  vb = &vbuf;
  ib = &ibuf;
  t1 = &tex0;
  t2 = &tex1;

  m = &mtl;

  InitTorus ();

/*  uint  total = 1000000;
  float start = clock ();

  for (int i=0;i<total;i++)
    render.vb.SetVB (vbuf);

  float end   = clock ();

  dout<<"FPS:\t"<<float (total)/(end-start)*float (CLK_TCK)<<endl;*/


  cam.proj.SetFrustum (60,60,1,1000);

  render.viewer.SetCamera (&cam);
                        
  cam.BindWindow (*startup.GetWindow ());

  glColor3f         (1,0,0);
  glClearColor      (0,0,0.5,0);
  glClear           (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable          (GL_DEPTH_TEST);
//  glEnable          (GL_FOG);

  float fogColor[4] = {0.8f, 0.8f, 0.8f, 1.0f};

  glFogi(GL_FOG_MODE, GL_LINEAR); 
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogf(GL_FOG_START, 0.0);   
  glFogf(GL_FOG_END, 50.0);  

  glFogi(GL_FOG_COORDINATE_SOURCE_EXT, GL_FOG_COORDINATE_EXT);

  glEnable          (GL_CULL_FACE);
  glEnable          (GL_LIGHTING);
  glEnable          (GL_LIGHT0);
  glEnable          (GL_COLOR_MATERIAL);

  uint maxTextures = 0;
  float max_aniso = 0;

  glGetIntegerv (GL_MAX_TEXTURE_UNITS,(int*)&maxTextures);  
  glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);

  dout<<maxTextures<<endl;
  dout<<"anis:\t"<<max_aniso<<endl;

  MainLoop          (Idle);
}

void     Idle   ()
{
  r->BeginDraw        (); 

  r->viewer.SetCamera (c);
  r->vb.SetVB         (*vb);
  r->mtls.SetMaterial (*m);
//  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);

  ib->DrawBuffer      ();
  r->DrawPasses       ();

  t1->transform *= Matrix::rotate (Vector3 (0,0,1),0.01);
  t2->transform *= Matrix::rotate (Vector3 (0,0,1),-0.001);

  r->EndDraw          ();
}

void InitTorus ()
{
  float         r1 = 4;         //radius low
  float         r2 = 1;         //radius high

  vrtx_t* vertex = new vrtx_t [N1*N2];
  uint*   index  = new uint [N1*N2*2*3];

  int   k = 0;

  float minx = 1000000,
        maxx = -minx,
        miny = minx,
        maxy = -miny;

  for (int i=0;i<N1;i++)
  {
    double phi = float (i*2)*float (FL_PI)/float (N1);

    for (int j=0;j<N2;j++,k++)
    {
      double psi = float (j*2)*float (FL_PI)/float (N2);

      vertex [k].v.x = (r1+r2*cos(psi)) * cos(phi);
      vertex [k].v.y = (r1+r2*cos(psi)) * sin(phi);
      vertex [k].v.z = r2 * sin(psi);      

//      vertex [k].c.x = cos (phi);
//      vertex [k].c.y = sin (phi);
//      vertex [k].c.z = cos (psi)*sin(psi);

      vertex [k].c.x = 1;
      vertex [k].c.y = 1;
      vertex [k].c.z = 1;

      if (vertex [k].v.x > maxx) maxx = vertex [k].v.x;
      if (vertex [k].v.y > maxy) maxy = vertex [k].v.y;

      if (vertex [k].v.x < minx) minx = vertex [k].v.x;
      if (vertex [k].v.y < miny) miny = vertex [k].v.y;
    }
  }

  float width  = maxx-minx,
        height = maxy-miny;

  for (i=0;i<N1*N2;i++)
  {
    vertex [i].tex.u = (vertex [i].v.x - minx) / width;
    vertex [i].tex.v = (vertex [i].v.y - miny) / height;
  }  

  uint* ind = index;

  for (i=k=0;i<N1;i++)
      for (int j=0;j<N2;j++,ind+=6)
      {
        ind [0] = i*N2 + j;
        ind [1] = ((i+1) % N1) * N2 +j;
        ind [2] = ((i+1) % N1) * N2 + (j+1) % N2;

        ind [3] = i*N2 + j;
        ind [4] = ((i+1) % N1) * N2 + (j+1) % N2;
        ind [5] = i * N2 + (j+1) % N2;

        vector3d_t n = (vertex [ind [1]].v-vertex [ind [0]].v) ^ 
                       (vertex [ind [2]].v-vertex [ind [0]].v);

        vertex [ind [0]].n -= n;        
        vertex [ind [1]].n -= n;
        vertex [ind [2]].n -= n; 

        n = (vertex [ind [4]].v-vertex [ind [3]].v) ^ 
            (vertex [ind [5]].v-vertex [ind [3]].v);

        vertex [ind [3]].n -= n;        
        vertex [ind [4]].n -= n;
        vertex [ind [5]].n -= n; 
      }

  for (i=0;i<N1*N2;i++)
    vertex [i].n.normalize ();
 
  vb->SetData          (sizeof (vrtx_t),N1*N2,vertex);
  vb->SetVertexArray   (getOffs (vrtx_t,v),VB_FLOAT);
  vb->SetNormalArray   (getOffs (vrtx_t,n),VB_FLOAT);
  vb->SetColorArray    (getOffs (vrtx_t,c),VB_FLOAT);
  vb->SetTexCoordArray (0,getOffs (vrtx_t,tex),VB_FLOAT);
  vb->SetTexCoordArray (1,getOffs (vrtx_t,tex),VB_FLOAT);
  
  ib->SetTriangles     (N1*N2*2,IB_UINT,index);

  delete [] index;
  delete [] vertex;
}
