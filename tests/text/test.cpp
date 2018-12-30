#define  DEBUG
#include <windows.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include <gl\gl.h>

#include <graph\vector3d.h>
#include <graph\textenv.h>
#include <debug\debug.h>

#include <binfile.h>

SystemPool              sys;
Debug                   debug (&sys);
TextEnvironment*        tenv;
float                   z       = -1,
                        zstep   = -0.05;

void            Init            ();
void CALLBACK   Reshape         (int,int);
void CALLBACK   Draw            ();
void CALLBACK   Idle            ();

void     main ()
{
  auxInitPosition       (0,0,1024,768);
  auxInitDisplayMode    (AUX_RGB | AUX_DOUBLE | AUX_DEPTH);
  auxInitWindow         ("Texture environment test");  

//  F_RegisterPackFile    ("text.zip","pak");
//  F_AddPrefix           ("pak");

  Init                  ();

  auxReshapeFunc        (Reshape);
  auxIdleFunc           (Idle);
  auxMainLoop           (Draw);

  delete tenv;
}

void    Init ()
{
//  glEnable         (GL_TEXTURE_2D);

  tenv = new TextEnvironment  (&sys,1024);

  tenv->SetTextureName  (0,"texture.bmp");
  tenv->SetTextureName  (1,"texture1.bmp");
  tenv->SetTextureName  (2,"image.jpg");
  tenv->SetTextureName  (3,"atoll.bmp");
  tenv->SetTextureName  (4,"atoll1.bmp");
  tenv->SetTextureName  (5,"envmap.jpg");
  tenv->SetTextureName  (6,"envmap2.jpg");
  tenv->SetTextureName  (7,"envmapbf.jpg");
  tenv->SetTextureName  (8,"floor2.alp");
  tenv->SetTextureName  (9,"image.alp");
  tenv->SetTextureName  (10,"texture.alp");
  tenv->SetTextureName  (11,"palm1.alp");
}

void CALLBACK   Reshape         (int width,int height)
{
  glViewport       (0,0,width,height);
  glMatrixMode     (GL_PROJECTION);
  glLoadIdentity   ();
//  glOrtho          (-10,10,-20,20,-100.0,100.0);
  glFrustum        (-20,20,-20,20,1,100.0);

  glMatrixMode     (GL_MODELVIEW);
  glLoadIdentity   ();

  glClearColor     (0.5,0.5,0.5,1.0);
  glClear          (GL_COLOR_BUFFER_BIT);

  glEnable         (GL_DEPTH_TEST);  
  glEnable         (GL_NORMALIZE);
  glEnable         (GL_TEXTURE_2D);
  glEnable         (GL_BLEND);

  glBlendFunc      (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glTexEnvf        (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
}

void CALLBACK   Draw            ()
{
  glClear   (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity   ();  

  gluLookAt (0,0,5,
             0,0,0,
             0,1,0);

  glDisable (GL_BLEND);
  glDisable (GL_TEXTURE_2D);

  glColor3f (0.1,0,0);

  glBegin   (GL_QUADS);
    glVertex3f (-15,10,z-5);
    glVertex3f (15,10,z-5);
    glVertex3f (15,-10,z-5);
    glVertex3f (-15,-10,z-5);
  glEnd     ();

  glEnable         (GL_TEXTURE_2D);
  glEnable         (GL_BLEND);
  
  tenv->bind (11);

  glBegin   (GL_QUADS);
    glTexCoord2f (0,0);   glVertex3f (-15,10,z);
    glTexCoord2f (1,0);   glVertex3f (15,10,z);
    glTexCoord2f (1,1);   glVertex3f (15,-10,z);
    glTexCoord2f (0,1);   glVertex3f (-15,-10,z);
  glEnd     ();

  glFinish  ();

  auxSwapBuffers();
}

void CALLBACK   Idle            ()
{
  static clock_t time = clock ();

  if (clock () - time > CLK_TCK>>5)
  {
    if (z<-20 && zstep < 0) zstep = -zstep;
    if (z>-0.5 && zstep > 0)   zstep = -zstep;

//    z+=zstep;

    Draw ();

    time = clock ();
  }       
}