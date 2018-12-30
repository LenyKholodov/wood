#include <kernel.h>
#include <render\render.h>
#include <render\startup.h>
#include <kernel\app.h>

#include <render\interface\nodes.h>
#include <render\interface\text.h>
#include <render\utils\flycamera.h>
#include <render\utils\fpscounter.h>

#include <render\bitmap.h>

#include <gl\gl.h>

const   int     N1 = 300;
const   int     N2 = 300;
const   double  FL_PI = 3.1415926;

void     DoTest ();
void     Idle   ();

Render*   r = NULL;
Camera*   c = NULL;
VertexBuffer* vb = NULL;


uint* index = NULL;

void     InitTorus (VertexBuffer*);

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
  VertexBuffer  vbuf;

  startup.screen.fullScreen = true;

  render.SetContext        (&startup);
  render.RegisterInterface (text); 

  render.AddPass           (fps,RP_DRAW_PASS);

  InitTorus (&vbuf);

  Bitmap tex0 ("atoll.bmp");

  r = &render;
  c = &cam;
  vb = &vbuf;

  cam.proj.SetFrustum (60,60,1,1000);

  render.SetCamera (&cam);
  render.GetMatEnv ().SetCurrent (tex0);
                        
  cam.BindWindow (*startup.GetWindow ());

  glColor3f         (1,0,0);
  glClearColor      (0,0,0.5,0);
  glClear           (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable          (GL_DEPTH_TEST);
  glEnable          (GL_TEXTURE_2D);
//  glActiveTextureARB (GL_TEXTURE0_ARB);
//  glEnable          (GL_TEXTURE_2D);
//  glEnable          (GL_FOG);

  float fogColor[4] = {0.8f, 0.8f, 0.8f, 1.0f};

  glFogi(GL_FOG_MODE, GL_LINEAR); 
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogf(GL_FOG_START, 0.0);   
  glFogf(GL_FOG_END, 50.0);  

  glFogi(GL_FOG_COORDINATE_SOURCE_EXT, GL_FOG_COORDINATE_EXT);

  glEnable          (GL_CULL_FACE);

  MainLoop          (Idle);
}

void     Idle   ()
{
  r->BeginDraw      ();

  r->SetCamera      (c);

  r->GetBufEnv ().SetCurrent (*vb);

//  glPolygonMode    (GL_FRONT_AND_BACK,GL_LINE);
  glColor3f        (1,1,1);

//  glActiveTextureARB (GL_TEXTURE0_ARB);
//  glEnable          (GL_TEXTURE_2D);


//  glColorMask      (GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
  glDrawElements  (GL_TRIANGLES,N1*N2*2*3,GL_UNSIGNED_INT,index);
//  glColorMask      (GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

  r->DrawPasses   ();  

  r->EndDraw      ();
}

void InitTorus (VertexBuffer* vb)
{
  float         r1 = 4;         //radius low
  float         r2 = 1;         //radius high

  struct vrtx_t
  {
    vertex_t    v;
//    Color3      c;

//    struct tex_t
//    {
//      float       u,v;
//    }tex;

//    float fog;
  };

  vrtx_t* vertex = new vrtx_t [N1*N2];
  index  = new uint [N1*N2*2*3];

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

/*      vertex [k].c.x = cos (phi);
      vertex [k].c.y = sin (phi);
      vertex [k].c.z = cos (psi)*sin(psi);

      vertex [k].fog = (1.0f-sin (phi))*10.0f;*/

      if (vertex [k].v.x > maxx) maxx = vertex [k].v.x;
      if (vertex [k].v.y > maxy) maxy = vertex [k].v.y;

      if (vertex [k].v.x < minx) minx = vertex [k].v.x;
      if (vertex [k].v.y < miny) miny = vertex [k].v.y;
    }
  }

  float width  = maxx-minx,
        height = maxy-miny;

//  for (i=0;i<N1*N2;i++)
//  {
//    vertex [i].tex.u = (vertex [i].v.x - minx) / width;
//    vertex [i].tex.v = (vertex [i].v.y - miny) / height;
//  }

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
      }

  vb->CreateBuffer (0);
  vb->SetBuffer    (0);
  vb->BindData     (vertex,N1*N2*sizeof (vrtx_t));
  vb->SetVertexes  (0,VB_FLOAT);
//  vb->SetColors    (sizeof (vertex_t),VB_FLOAT);
//  vb->SetTexCoords (0,sizeof (vertex_t)+sizeof (Color3),VB_FLOAT);
//  vb->SetFogCoords (sizeof (vertex_t)+sizeof (Color3)+sizeof (vrtx_t::tex_t),VB_FLOAT);
}
