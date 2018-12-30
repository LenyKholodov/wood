//#include <windows.h>
#include <kernel\app.h>
#include <kernel\keydefs.h>
#include <window.h>
#include <misc.h>

#include <ogl.h>

#include <graph\glext.h>
#include <graph\camera.h>
#include <graph\utils.h>
#include <graph\font.h>

#include "r_main.h"
#include "g_loader.h"
#include "g_lndcache.h"

#include "r_mdlenv.h"
#include "r_mdlmulti.h"
#include "r_mdlsmp.h"
#include "r_mdlsprite.h"
#include "r_effwavewater.h"

#include "r_shadow.h"
#include "r_vbuffer.h"
#include "r_skylight.h"

const int       SCREEN_WIDTH  = 1024;
const int       SCREEN_HEIGHT = 768;

const float     TURN_ANGLE    = 3.14f / 72.0f;
float           STEP          = 1.0f;

const int       X_CENTER      = SCREEN_WIDTH / 2;
const int       Y_CENTER      = SCREEN_HEIGHT / 2;
const float     X_SENS        = -.002;
const float     Y_SENS        = .002;

void     Draw     ();
void     Reshape  (int,int);
void     Idle     ();
int      Keyb     (Window*,UINT,WORD,DWORD,DWORD);
void     Move     ();

void     InitFonts ();
void     DoneFonts ();

void     SetDayTime ();
void     AnimModels ();

void     AnimLights ();
void     InitLights ();

void     LinearInterpolation (GLfloat*,float t,const GLint*,const GLint*);

float    GetHeight     (const vector3d_t& v,LevelLoader& loader);

FloatPool               pool        (M_GetPool (),20*1024*1024); 
Debug                   debug       (&pool);
LevelLoader*            levelLoader = NULL; 

boundbox_t              box;
MainRender*             render        = NULL;
int                     kpress [1024] = {0};

vector3d_t              modelC;

Camera   eye      (vector3d_t(0),
                   vector3d_t(0,0,1),     
                   vector3d_t(0,1,0)
                  ); 
vector3d_t c (0);
int        lightCount = 0;
float      fps        = 0;
ulong      tps        = 0;

MODEL      mdl [32] = {-1};
ModelEnv*  me       = NULL;
SkyLight*  sky      = NULL;

int main (int,char* [])
{
  Application* app      = A_GetApplication ();  

  Window*      wnd      = IF_CreateWindow  (FULLSCREEN_WINDOW,
                                            M_GetPool (),
                                            app->instance (),
                                            rect_t (0,0,SCREEN_WIDTH,SCREEN_HEIGHT),
                                            "Render test"
                                           );
  Window*      wnd1     = IF_CreateWindow  (FULLSCREEN_WINDOW,
                                            M_GetPool (),
                                            app->instance (),
                                            rect_t (0,0,SCREEN_WIDTH,SCREEN_HEIGHT),
                                            "Render test shadow window"
                                           );

  wnd->ShowCursor       (FALSE);
  wnd->InsertHandler    (WM_DESTROY,WND_Done);
  wnd->InsertHandler    (WM_KEYDOWN,Keyb);
  wnd->InsertHandler    (WM_KEYUP,Keyb);
  wnd->InsertHandler    (WM_MOUSEMOVE,Keyb);
  wnd1->ShowCursor      (FALSE);

//  RENDER_VIEW_DEPTH *= 8;
  RENDER_VIEW_DEPTH /= 2;

  OGL_Load              ();
  OGL_DisplayMode       (OGL_DOUBLE | OGL_DEPTH | OGL_RGB);
  OGL_ReshapeFunc       (Reshape);
  OGL_DrawFunc          (Draw);
  OGL_CreateContext     (OGL_CONTEXT0,wnd);
  OGL_CreateContext     (OGL_CONTEXT1,wnd1);
  InitExtensions        ();
  OGL_Start             (OGL_RESHAPE_CALL);

  M_PushPool            (&pool);  

  LevelLoader           level (M_GetPool(),"maps/park.map");
  levelLoader  = &level;

  level.Load            ();

  InitFonts             ();

  render       = new (M_GetPool()) MainRender (M_GetPool(),level);
  me           = level->mModelEnv;

  render->BindModelEnv (me);

  box     = render->GetLandMap ()->bbox;
  c       = (box.max+box.min)/2.0f;
  STEP    = length (box.max-box.min)/400.0f;

  MODELKEEPER kpr [4] = {
    me->BindKeeper (new (M_GetPool()) SimpleModelKeeper (M_GetPool(),me,RENDER_SMPMODEL_MESH_NUM)),
    me->BindKeeper (new (M_GetPool()) MultiModelKeeper (M_GetPool(),me,RENDER_MULTIMODEL_MESH_NUM)),
    me->BindKeeper (new (M_GetPool()) Sprite3DModelKeeper (M_GetPool(),me)),
    me->BindKeeper (new (M_GetPool()) WaterModelKeeper (M_GetPool(),me))
  };  
                   
  MODELDATA  md [16] = {
    me->LoadModelData (kpr [0],FALSE,"models/tree.wmd"),
    me->LoadModelData (kpr [1],FALSE,"models/tree0.wmd"),
    me->LoadModelData (kpr [1],FALSE,"models/tree1.wmd"),
    me->LoadModelData (kpr [1],FALSE,"models/baba.wmd"),

    me->LoadModelData (kpr [2],FALSE,"models/sprite1.mdl"),
    me->LoadModelData (kpr [1],FALSE,"models/palma.wmd"),
    me->LoadModelData (kpr [0],FALSE,"models/vishnya.wmd"),
    me->LoadModelData (kpr [1],FALSE,"models/tree2.wmd"),

    me->LoadModelData (kpr [1],FALSE,"models/tree3.wmd"),
    me->LoadModelData (kpr [3],FALSE),
    me->LoadModelData (kpr [1],FALSE,"palma1.wmd"),
  };  

  sky = new (M_GetPool()) 
        SkyLight (M_GetPool(),render->GetLightEnv (),render->GetSkyBox (),box);

  mdl [0] = me->CreateModel (md [3],modelC=c+vector3d_t (-30,8,-20),matrix_t::scale (vector3d_t (1)));

  InitLights ();

  eye.set (box.max / 2.0f,eye.dir (), eye.up());
  MainLoop              (Idle);  

  DoneFonts             ();

  OGL_Done              ();

  delete render;
  delete wnd;
  delete wnd1;

  M_PopPool             ();    

  return EXIT_SUCCESS;
}

void     InitFonts ()
{
  dout<<"Init fonts"<<endl;
  LoadFont         (0,"textures\\font.tga",M_GetPool(),0,128,0,16*8);
  LoadFont         (1,"textures\\font.tga");
  SetFont          (0);
  SetScreenSize    (SCREEN_WIDTH,SCREEN_HEIGHT);
}

void     DoneFonts ()
{
  dout<<"Done fonts"<<endl;
  DestroyFont    (0);
  DestroyFont    (1);
}

BOOL     LevelLoader::LoadData ()
{
  access()->mLandMap       = new (objPool) LandscapeMap   (objPool,&level);
  access()->mLandCache     = new (objPool) LandscapeCache (objPool,*this,
                                                 access()->mLandMap->cacheWidth,
                                                 access()->mLandMap->cacheHeight,
                                                 4 * 1024 * 1024
                                                );

  access()->mMatEnv        = new (objPool) MatEnv         (objPool,&level,
                                                 RENDER_STATIC_TEXTURES_CACHE_SIZE,
                                                 RENDER_DINAMIC_TEXTURES_CACHE_SIZE
                                                );
  access()->mModelEnv      = new (objPool) ModelEnv       (objPool,access()->mLandMap,
                                                 RENDER_MODEL_DATA_NUM,
                                                 RENDER_MODEL_NUM,
                                                 RENDER_VIS_MODEL_NUM,
                                                 512*1024,
                                                 512*1024  
                                                 );
  access()->mVertexBuffer  = new (objPool) VertexBuffer (objPool,RENDER_VERTEX_BUFFER_SIZE);

  return TRUE;
}

void     LevelLoader::UnloadData ()
{
  delete access()->mMatEnv;
  delete access()->mModelEnv;
  delete access()->mLandCache;
  delete access()->mLandMap;
  delete access()->mVertexBuffer;
}

void     Draw       ()
{  
  Camera camera      = eye;
  float  height      = GetHeight       (camera.pos (),*levelLoader) + 
                       (*levelLoader)->mLandMap->bbox.max.y * 0.05;

  camera.set (vector3d_t (camera.pos ().x,height*1.1,camera.pos ().z),
              camera.dir (),
              camera.up ()
             );  

  render->Draw          (camera);

  glColor3f (0,1,0);    OutText  (10,30,"FPS: %4.1f",fps);
  glColor3f (0,1,1);    OutText  (10,45,"TPS: %u",tps);
  glColor3f (1,0,1);    OutText  (10,60,1,"Light count: %u",lightCount);

  glColor3f (1,1,1);    OutText  (SCREEN_WIDTH*0.7,30,1, "glFinish:      %u",RENDER_GL_FINISH);
  glColor3f (1,1,1);    OutText  (SCREEN_WIDTH*0.7,45,1, "GL_FOG:        %u",glIsEnabled (GL_FOG));
  glColor3f (1,1,1);    OutText  (SCREEN_WIDTH*0.7,60,1, "GL_TEXTURE_2D: %u",glIsEnabled (GL_TEXTURE_2D));
  glColor3f (1,1,1);    OutText  (SCREEN_WIDTH*0.7,75,1, "Sky:           %u",render->GetState (RENDER_SKY_BIT));
  glColor3f (1,1,1);    OutText  (SCREEN_WIDTH*0.7,90,1, "Clear depth:   %u",render->GetState (RENDER_CLEAR_DEPTH_BUFFER_BIT));
  glColor3f (1,1,1);    OutText  (SCREEN_WIDTH*0.7,105,1,"Shadows:       %u",render->GetState (RENDER_SHADOWS_BIT));
  
  if (RENDER_GL_FINISH)  glFinish ();
  else                   glFlush  ();

  OGL_SwapBuffers       ();   
}

void     Reshape  (int width,int height)
{
  glViewport       (0,0,width,height);
  glMatrixMode     (GL_PROJECTION);
  glLoadIdentity   ();

  gluPerspective   (RENDER_VIEW_ANGLE*57.3f,(float)width/(float)height,1.0,RENDER_VIEW_DEPTH*0.85);

  glMatrixMode     (GL_MODELVIEW);
  glLoadIdentity   ();

  glEnable         (GL_DEPTH_TEST); 
  glEnable         (GL_LIGHTING);
  glEnable         (GL_CULL_FACE);
  glEnable         (GL_TEXTURE_2D);  

  glEnable         (GL_FOG);

  glFrontFace      (GL_CCW);
  glCullFace       (GL_CW);

  glClearColor     (0.1,0.2,0.3,0.5);
  glClear          (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  GLfloat          col [4] = {0.4,0.4,0.4,1};

//  glFogf           (GL_FOG_MODE,GL_LINEAR);
  glFogf           (GL_FOG_MODE,GL_EXP2);
  glFogf           (GL_FOG_START,RENDER_VIEW_DEPTH*0.25);
  glFogf           (GL_FOG_END,RENDER_VIEW_DEPTH*0.75);
  glFogf           (GL_FOG_DENSITY,0.05);
  glFogfv          (GL_FOG_COLOR,col);

  glBlendFunc      (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

void  Idle  ()
{
  static clock_t last  = clock ();
  static int     total = 0;

  SetDayTime ();
  AnimLights ();
  AnimModels ();
  Move ();
  Draw ();

  total++;

  if (total >= 25)
  {
    int pgcount = render->GetPgCount ();

    fps         = float(total*CLK_TCK)/float(clock()-last);
    tps         = float(pgcount*CLK_TCK)/float(clock()-last);

//    dout<<"FPS: "<<fps<<" TPS: "<<tps<<endl;

    last  = clock ();
    total = 0;
  }
}

const   float LIGHT_RADIUS = 8;

float      fromAngle (float angle) { return angle / FL_RAD; }
lightsrc_t src1, src2, src3;
lightsrc_t locSrc;
SHADOW     shadow [3]  = {-1};
int        li     [16] = {-1};

void     InitLights ()
{
  src1.shiness       = 0.4;
  src1.light.diffuse = vector3d_t (1,0,0);
  src2.shiness       = 0.4;
  src2.light.diffuse = vector3d_t (0,1,0);
  src3.shiness       = 0.4;
  src3.light.diffuse = vector3d_t (0,0,1);
  src1.light.shiness = src2.light.shiness = src3.light.shiness = 1.0f;
  src1.light.angle   = src2.light.angle   = src3.light.angle = 3.14 / 6;
  src1.light.specular= src2.light.specular = src3.light.specular = vector3d_t (0);
  src1.light.ambient = src2.light.ambient  = src3.light.ambient  = vector3d_t (0);

  src1.light.pos     = vector3d_t (LIGHT_RADIUS*cos(0)+modelC.x,modelC.y+12,LIGHT_RADIUS*sin(0)+modelC.z);
  src2.light.pos     = vector3d_t (LIGHT_RADIUS*cos(fromAngle (120))+modelC.x,modelC.y+12,LIGHT_RADIUS*sin(fromAngle (120))+modelC.z);
  src3.light.pos     = vector3d_t (LIGHT_RADIUS*cos(fromAngle (240))+modelC.x,modelC.y+12,LIGHT_RADIUS*sin(fromAngle (240))+modelC.z);

  src1.light.dir     = normalize (modelC - src1.light.pos);
  src2.light.dir     = normalize (modelC - src2.light.pos);
  src3.light.dir     = normalize (modelC - src3.light.pos);

  locSrc.shiness       = 1;
  locSrc.light.diffuse = vector3d_t (0.7,.7,7.);
  locSrc.light.shiness = 1;
  locSrc.light.angle   = 3.14 / 12.0f;
  locSrc.light.specular= vector3d_t (0);
  locSrc.light.ambient = vector3d_t (0.3,0.3,0.3);
  locSrc.light.pos     = eye.pos ();
  locSrc.light.dir     = eye.dir ();

  li [0]             = render->GetLightEnv()->NewLight (src1,TLE_GLOBAL_LIGHT|TLE_ENABLE_LIGHT);
  li [1]             = render->GetLightEnv()->NewLight (src2,TLE_GLOBAL_LIGHT|TLE_ENABLE_LIGHT);
  li [2]             = render->GetLightEnv()->NewLight (src3,TLE_GLOBAL_LIGHT|TLE_ENABLE_LIGHT);
  li [3]             = render->GetLightEnv()->NewLight (locSrc,TLE_GLOBAL_LIGHT|TLE_ENABLE_LIGHT);

  shadow [0]         = render->GetShadowEnv()->CreateShadow (li [0],mdl [0]);
  shadow [1]         = render->GetShadowEnv()->CreateShadow (li [1],mdl [0]);
  shadow [2]         = render->GetShadowEnv()->CreateShadow (li [2],mdl [0]);

  render->GetShadowEnv()->EnableShadow (shadow [0]);
  render->GetShadowEnv()->EnableShadow (shadow [1]);
  render->GetShadowEnv()->EnableShadow (shadow [2]);
}

void     AnimLights ()
{
  static clock_t time = clock ();

  static float angle = 0;
  float        astep = 5;

  src1.light.pos   = vector3d_t (LIGHT_RADIUS*cos(fromAngle (angle))+modelC.x,modelC.y+12,LIGHT_RADIUS*sin(fromAngle (angle))+modelC.z);
  src2.light.pos   = vector3d_t (LIGHT_RADIUS*cos(fromAngle (angle+120))+modelC.x,modelC.y+12,LIGHT_RADIUS*sin(fromAngle (angle+120))+modelC.z);
  src3.light.pos   = vector3d_t (LIGHT_RADIUS*cos(fromAngle (angle+240))+modelC.x,modelC.y+12,LIGHT_RADIUS*sin(fromAngle (angle+240))+modelC.z);

  src1.light.dir   = normalize (modelC - src1.light.pos);
  src2.light.dir   = normalize (modelC - src2.light.pos);
  src3.light.dir   = normalize (modelC - src3.light.pos);    

  locSrc.light.pos     = eye.pos ();
  locSrc.light.dir     = eye.dir ();

  render->GetLightEnv()->ChangeLight (li [0],src1);
  render->GetLightEnv()->ChangeLight (li [1],src2);
  render->GetLightEnv()->ChangeLight (li [2],src3);
  render->GetLightEnv()->ChangeLight (li [3],locSrc);

  if (clock () - time > CLK_TCK<<1)
  {
    render->GetShadowEnv()->refresh    (shadow [0]);
    render->GetShadowEnv()->refresh    (shadow [1]);
    render->GetShadowEnv()->refresh    (shadow [2]);

    angle     += astep;
    
    time       = clock ();
  }
}       

void     AnimModels ()
{   
//  me->ChangeProperties (mdl [7],EFFECT_TICK);
  me->ChangeProperties (mdl [0],MODEL_MULT_MATRIX,matrix_t::rotate (vector3d_t (0,1,0),3.14f/60.0f));
}

void     SetDayTime ()
{
  static  float   time1 = 0;
  static  float   step  = 0.08f;
  static  clock_t last  = clock ();

  if (clock () - last > CLK_TCK >> 4)
  {
    time1 += step;

    if (time1 >= 24.0f) time1 = 0.0f;

    sky->refresh (time1);

    static clock_t last1 = clock ();
    static clock_t last2 = clock ();

    if (clock () - last1 > CLK_TCK )
    {
//      render->GetShadowEnv()->refresh (shadow);
      last1 = clock ();
    }  

    last = clock ();
  }
}

void Move ()
{
  if (kpress [VK_UP] || kpress [VK_W])
      eye.set (eye.pos () + eye.dir ()*STEP,eye.dir (),eye.up ());

  if (kpress [VK_DOWN] || kpress [VK_S])
      eye.set (eye.pos () - eye.dir ()*STEP,eye.dir (),eye.up ());

  if (kpress [VK_A])
      eye.set (eye.pos () + matrix_t::rotate (vector3d_t (0,1,0),3.14f/2.0f)*eye.dir ()*STEP,eye.dir (),eye.up ());  

  if (kpress [VK_D])
      eye.set (eye.pos () + matrix_t::rotate (vector3d_t (0,1,0),-3.14f/2.0f)*eye.dir ()*STEP,eye.dir (),eye.up ());  
  
  if (kpress [VK_LEFT])
    eye.set (eye.pos(),matrix_t::rotate (vector3d_t (0,1,0),TURN_ANGLE)*eye.dir (),eye.up());
  
  if (kpress [VK_RIGHT])
    eye.set (eye.pos(),matrix_t::rotate (vector3d_t (0,1,0),-TURN_ANGLE)*eye.dir (),eye.up());


  if (kpress [VK_ESCAPE])
      W32_PostQuitMessage (0);  

  vector3d_t pos = eye.pos ();

  if         (pos.x < box.min.x) pos.x = box.min.x;
  else if    (pos.x > box.max.x) pos.x = box.max.x;
  if         (pos.y < box.min.y) pos.y = box.min.y;
  else if    (pos.y > box.max.y) pos.y = box.max.y;
  if         (pos.z < box.min.z) pos.z = box.min.z;
  else if    (pos.z > box.max.z) pos.z = box.max.z;

  eye.set (pos,eye.dir(),eye.up());
}

int      Keyb    (Window*,UINT msg,WORD key,DWORD lParam,DWORD)
{
  if (msg == WM_MOUSEMOVE)
  {
    static state = FALSE;

    if (state)
    {
      state = FALSE;
      return 0;
    }

    float       xOffs  = 2.0f*float(LOWORD(lParam)-X_CENTER)/float(SCREEN_WIDTH);
    float       yOffs  = 2.0f*float(HIWORD(lParam)-Y_CENTER)/float(SCREEN_HEIGHT);

    W32_SetCursorPos (X_CENTER,Y_CENTER);

    eye.set (eye.pos(),matrix_t::rotate (vector3d_t (0,1,0),-xOffs)*eye.dir (),eye.up());

    vector3d_t  right  = eye.up () ^ eye.dir();

    eye.set (eye.pos(),matrix_t::rotate (right,yOffs)*eye.dir (),eye.up());

    state       = TRUE;
  }
  else if (msg == WM_KEYDOWN && key < 1024)
  {
    kpress [key] = TRUE;
  }  
  else if (msg == WM_KEYUP && key < 1024)
  {
    kpress [key] = FALSE;  

    if (key == VK_1)
      RENDER_GL_FINISH = !RENDER_GL_FINISH;

    if (key == VK_F)
    {
      if (render->GetLightEnv ()->IsEnabled (li [3])) render->GetLightEnv ()->DisableLight (li [3]);
      else   
      {
        render->GetLightEnv ()->EnableLight (li [3]);
      }  
    }  

    if (key == VK_L)
      if (render->GetLightEnv ()->IsEnabled (li [0]))
      {
        render->GetLightEnv ()->DisableLight (li [0]);
        render->GetLightEnv ()->DisableLight (li [1]);
        render->GetLightEnv ()->DisableLight (li [2]);
      }
      else
      {
        render->GetLightEnv ()->EnableLight (li [0]);
        render->GetLightEnv ()->EnableLight (li [1]);
        render->GetLightEnv ()->EnableLight (li [2]);
      }

    if (key == VK_1 + 1)
    {
      if (glIsEnabled (GL_FOG))  glDisable (GL_FOG);
      else                       glEnable  (GL_FOG);
    }  

    if (key == VK_1 + 2)
    {
      if (glIsEnabled (GL_TEXTURE_2D))  glDisable (GL_TEXTURE_2D);
      else                              glEnable  (GL_TEXTURE_2D);
    }  

    if (key == VK_1 + 3)
    {
      if (render->GetState (RENDER_SHADOWS_BIT))  render->ChangeState (RENDER_SHADOWS_BIT,FALSE);
      else                                        render->ChangeState (RENDER_SHADOWS_BIT,TRUE);
    }  

    if (key == VK_1 + 4)
    {
      if (render->GetState (RENDER_SKY_BIT))  
      {
        render->ChangeState (RENDER_SKY_BIT,FALSE);
        render->ChangeState (RENDER_CLEAR_COLOR_BUFFER_BIT,TRUE);
      }
      else 
      {
        render->ChangeState (RENDER_SKY_BIT,TRUE);
        render->ChangeState (RENDER_CLEAR_COLOR_BUFFER_BIT,FALSE);
      }  
    }  

    if (key == VK_1 + 5)
    {
      if (render->GetState (RENDER_CLEAR_DEPTH_BUFFER_BIT) )
        render->ChangeState (RENDER_CLEAR_DEPTH_BUFFER_BIT,FALSE);
      else  
        render->ChangeState (RENDER_CLEAR_DEPTH_BUFFER_BIT,TRUE);
    }  

    if (key == VK_1 + 8)
    {
      lightsrc_t light;

      memset (&light,0,sizeof (light));

      light.light.diffuse    = vector3d_t (randomfl(),randomfl(),randomfl());
      light.light.specular   = vector3d_t (randomfl(),randomfl(),randomfl());
      light.light.ambient    = vector3d_t (randomfl(),randomfl(),randomfl());
      light.light.shiness    = randomfl ();
      light.light.angle      = 3.14f / 3.0f;
      light.light.pos        = eye.pos ();
      light.light.dir        = eye.dir ();
      light.light.farlight   = FALSE;
      light.shiness          = randomfl ();

//      li [lightCount++]      = render->GetLightEnv ()->NewLight (light,TLE_GLOBAL_LIGHT|TLE_ENABLE_LIGHT);
      li [lightCount++]      = render->GetLightEnv ()->NewLight (light);
    }
  }  
 
  return 0;
}

float    GetHeight     (const vector3d_t& v,LevelLoader& loader)
{
  D_Try
  {
    LandscapeMap*  mMap    = loader->mLandMap;
    LandscapeCache* mCache = loader->mLandCache;

    int          grnode = int(v.z/mMap->step)*mMap->width+int(v.x/mMap->step);
    facetpack_t* fp     = (facetpack_t*)mCache->get (grnode,GRC_CHUNK_FACET_PACK);
    float        height = mMap->bbox.max.y;

    vector3d_t line [2] = {vector3d_t (v.x,0,v.z),vector3d_t (v.x,mMap->bbox.max.y,v.z)};
    vertex_t   temp (0);

    for (int i=0;i<mCache->GetNodeInfo (grnode).chunk [GRC_CHUNK_FACET_PACK].count;i++,fp++)
    {
      facetpack_t* pack = fp;

      facet_t& facet    = pack->facet;
      plane_t  plane    = {facet.n.x,facet.n.y,facet.n.z,-(facet.n & mMap->vertex [facet.index [0]])};      
      vertex_t vrtx [3] = {mMap->vertex [facet.index [0]],mMap->vertex [facet.index [1]],mMap->vertex [facet.index [2]]};

      IntersectionPoint  (line,plane,temp);
      
      if (IsInsidePolygon (temp,vrtx,3))
        if (temp.y < height) height = temp.y;
    }

    return height != mMap->bbox.max.y ? height : 0;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    dout<<"Error at GetMinHeight"<<endl;
    return 0;
  }
}
