//#include <windows.h>
#include <kernel\app.h>
#include <kernel\keydefs.h>
#include <window.h>
#include <misc.h>
#include <log.h>

#include <ogl.h>

#include <graph\glext.h>
#include <graph\camera.h>
#include <graph\utils.h>
#include <graph\font.h>
#include <console\gdicon.h>
#include <console\comline.h>
#include <console\command.h>

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

void     InitConsole();
void     InitLog    ();
void     DoneConsole();
void     DoneLog    ();
void     CmdQuit    (VarPool&,uint,ParamStack&,ostream&,DWORD);

void     SetDayTime ();
void     AnimModels ();

void     LinearInterpolation (GLfloat*,float t,const GLint*,const GLint*);

float    GetHeight     (const vector3d_t& v,LevelLoader& loader);

FloatPool               pool        (M_GetPool (),32*1024*1024); 
Debug                   debug       (&pool);
LevelLoader*            levelLoader = NULL; 
ConsoleBase             con (&pool,"logout",512,COMMAND_LINE_LEN);
GraphConsole*           grCon       = NULL;
Command*                command     = NULL;
CommandLine*            comLine     = NULL;                        
Window*                 conWindow   = NULL;
BOOL                    conOpen     = FALSE;

boundbox_t              box;
MainRender*             render        = NULL;
int                     kpress [1024] = {0};

Camera   eye      (vector3d_t(0),
                   vector3d_t(0,0,1),     
                   vector3d_t(0,1,0)
                  ); 
vector3d_t c (0);
int        li [16] = {-1};
int        lightCount = 0;
float      fps        = 0;
ulong      tps        = 0;

MODEL      mdl [32] = {-1};
ModelEnv*  me       = NULL;
SkyLight*  sky      = NULL;
SHADOW     shadow;

int main (int,char* [])
{
  Application* app      = A_GetApplication ();  

  M_PushPool            (&pool);  

  InitLog               ();

  LogMessage            ("-----------Windows startup------------");
  LogMessage            ("...Create console");

  InitConsole           ();

  LogMessage            ("...Create full screen windows");

  Window*      wnd      = IF_CreateWindow  (FULLSCREEN_WINDOW,
                                            M_GetPool (),
                                            app->instance (),
                                            rect_t (0,0,SCREEN_WIDTH,SCREEN_HEIGHT),
                                            "Render test"
                                           );
/*  Window*      wnd1     = IF_CreateWindow  (FULLSCREEN_WINDOW,
                                            M_GetPool (),
                                            app->instance (),
                                            rect_t (0,0,SCREEN_WIDTH,SCREEN_HEIGHT),
                                            "Render test shadow window"
                                           );*/

  LogMessage            ("...Set handlers");

  wnd->ShowCursor       (FALSE);
  wnd->InsertHandler    (WM_DESTROY,WND_Done);
  wnd->InsertHandler    (WM_KEYDOWN,Keyb);
  wnd->InsertHandler    (WM_KEYUP,Keyb);
  wnd->InsertHandler    (WM_MOUSEMOVE,Keyb);
//  wnd1->ShowCursor      (FALSE);

//  RENDER_VIEW_DEPTH *= 16;
  RENDER_VIEW_DEPTH /= 2;

  DoneConsole           ();

  LogMessage            ("---------Init OpenGL system---------------");

  LogMessage            ("...Load OpenGL unit");

  OGL_Load              ();
  OGL_DisplayMode       (OGL_DOUBLE | OGL_DEPTH | OGL_RGB);
  OGL_ReshapeFunc       (Reshape);
  OGL_DrawFunc          (Draw);

  LogMessage            ("...Create context");

  OGL_CreateContext     (OGL_CONTEXT0,wnd);
//  OGL_CreateContext     (OGL_CONTEXT1,wnd1);
  LogMessage            ("...Init extensions table");
  InitExtensions        ();
  LogMessage            ("...Start OpenGL unit");
  OGL_Start             (OGL_RESHAPE_CALL);
  
  F_SetResType          (RES_TEXTURE,"textures");

  LogMessage            ("------------Game init--------------");
  LogMessage            ("...Create static data");

  LevelLoader           level (M_GetPool(),"maps/park.map");
  levelLoader  = &level;

  level.Load            ();

  LogMessage            ("-------------Render init-------------");

  LogMessage            ("...Init fonts");

  InitFonts             ();

  LogMessage            ("...Create render");

  render       = new (M_GetPool()) MainRender (M_GetPool(),level);
  me           = level->mModelEnv;

  render->BindModelEnv (me);
  render->BindConsole  (con,0,60,25);
//  render->GetTreeEnv()->SetScale (vector3d_t (1,20,1));

  box     = render->GetLandMap ()->bbox;
  c       = (box.max+box.min)/2.0f;
  STEP    = length (box.max-box.min)/400.0f;

  LogMessage            ("...Precacheing models");

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

  LogMessage ("...Sky init");

  sky = new (M_GetPool()) 
        SkyLight (M_GetPool(),render->GetLightEnv (),render->GetSkyBox (),box);

  LogMessage ("...Create models");

  mdl [0] = me->CreateModel (md [3],c+vector3d_t (0,14,0),matrix_t::scale (vector3d_t (1)));

  eye.set (box.max / 2.0f,eye.dir (), eye.up());

  LogMessage ("-----------Main loop------------");  

  MainLoop              (Idle);  

  LogMessage            ("------------Done system------------");
  LogMessage            ("...Done fonts");

  DoneFonts             ();

  LogMessage            ("...Delete render");

  delete render;

  LogMessage            ("...Done OpenGL");

  OGL_Done              ();

  LogMessage            ("...Delete windows");

  delete wnd;
//  delete wnd1;

  LogMessage            ("...Delete console");  
  DoneConsole           ();
  LogMessage            ("...Close log");
  DoneLog               ();

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

void     InitConsole()
{
  Application* app = A_GetApplication ();  
  Window*      wnd = IF_CreateWindow  (SIMPLE_WINDOW,
                                       M_GetPool (),
                                       app->instance (),
                                       rect_t (200,200,400,300),
                                       "Wood GDI console"
                                      );
//  Window*      wnd1 = IF_CreateWindow (POPUP_WINDOW_CHILD,
//                                       M_GetPool (),
//                                       wnd,
//                                       rect_t (20,20,250,380),
//                                       "Wood GDI console"
//                                      );
  grCon = new (M_GetPool()) GDIConsole (M_GetPool(),wnd,con);
  conWindow = wnd;

  con.refresh (TRUE);
  con.refresh ();
  con.bind    (grCon);  
}

void     InitLog    ()
{
  class ConLog: public Log
  {
    public:
      ConLog (ConsoleBase& base): Log (&base.os()), con (base) {}

      virtual void textcolor (long color) { if (con.grcon ()) con.grcon()->textcolor (color); }

      ConsoleBase& con;
  };

  logOut  = new ConLog (con);
  command = new Command (M_GetPool(),128);
  comLine = new CommandLine (M_GetPool(),con,*command,COMMAND_LINE_LEN,8);

  command->LoadConfig ("config.cfg"); 

  command->bind ("quit",CmdQuit);
}

void     DoneConsole()
{
  delete grCon;
  delete conWindow;
  conWindow = NULL; 
  grCon = NULL;
}

void     DoneLog    ()
{
  command->SaveConfig ("config.cfg"); 

  if (logOut) delete logOut;
  if (comLine) delete comLine;
  if (command) delete command;
  logOut = NULL;
  command = NULL;
  comLine = NULL;
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
                       (*levelLoader)->mLandMap->bbox.max.y * 0.0005;

  camera.set (vector3d_t (camera.pos ().x,height*1.1,camera.pos ().z),
              camera.dir (),
              camera.up ()
             );  

  render->Draw          (camera);

  glColor3f (0,1,0);    OutText  (10,30,"FPS: %4.1f",fps);
  glColor3f (0,1,1);    OutText  (10,45,"TPS: %u",tps);
  glColor3f (1,0,1);    OutText  (10,60,1,"Light count: %u",lightCount);

  glColor3f (1,1,1);    OutText  (SCREEN_WIDTH*0.7,30,1,"glFinish:      %u",RENDER_GL_FINISH);
  glColor3f (1,1,1);    OutText  (SCREEN_WIDTH*0.7,45,1,"GL_FOG:        %u",glIsEnabled (GL_FOG));
  glColor3f (1,1,1);    OutText  (SCREEN_WIDTH*0.7,60,1,"GL_TEXTURE_2D: %u",glIsEnabled (GL_TEXTURE_2D));
  glColor3f (1,1,1);    OutText  (SCREEN_WIDTH*0.7,75,1,"Sky:           %u",render->GetState (RENDER_SKY_BIT));
  glColor3f (1,1,1);    OutText  (SCREEN_WIDTH*0.7,90,1,"Clear depth:   %u",render->GetState (RENDER_CLEAR_DEPTH_BUFFER_BIT));
  
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

  glFrontFace      (GL_CCW);
  glCullFace       (GL_CW);

  glClearColor     (0.1,0.2,0.3,0.5);
  glClear          (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  GLfloat          col [4] = {0.2,0.2,0.4,1};

  glFogf           (GL_FOG_MODE,GL_LINEAR);
  glFogf           (GL_FOG_START,RENDER_VIEW_DEPTH*0.25);
  glFogf           (GL_FOG_END,RENDER_VIEW_DEPTH*0.75);
  glFogfv          (GL_FOG_COLOR,col);

  glBlendFunc      (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

void  Idle  ()
{
  static clock_t last  = clock ();
  static int     total = 0;

  SetDayTime ();
//  AnimModels ();
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

void     AnimModels ()
{   
  me->ChangeProperties (mdl [7],EFFECT_TICK);
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
    if (!conOpen)
      kpress [key] = TRUE;
    else
    {
      if (key >= VK_0 && key <= VK_9) comLine->put (key-VK_0+'0');
      if (key >= VK_A && key <= VK_Z) comLine->put (key-VK_A+'a');
      if (key == VK_SPACE)            comLine->put (' ');
      if (key == VK_RETURN)           comLine->put ('\n');
      if (key == VK_PRIOR)            con.ScrollUp   ();
      if (key == VK_NEXT)             con.ScrollDown ();
      if (key == VK_BACK)             comLine->put (8);
      if (key == VK_TAB)              comLine->put ('\t');
      if (key == VK_UP)               comLine->prevcmd ();
      if (key == VK_DOWN)             comLine->nextcmd ();
    }   
  }  
  else if (msg == WM_KEYUP && key < 1024)
  {
    if (!conOpen)
    {
      kpress [key] = FALSE;  

      if (key == VK_1)
        RENDER_GL_FINISH = !RENDER_GL_FINISH;

      if (key == VK_2)
      {
        if (glIsEnabled (GL_FOG))  glDisable (GL_FOG);
        else                       glEnable  (GL_FOG);
      }  

      if (key == VK_3)
      {
        if (glIsEnabled (GL_TEXTURE_2D))  glDisable (GL_TEXTURE_2D);
        else                              glEnable  (GL_TEXTURE_2D);
      }  

      if (key == VK_4)
      {
        if (render->GetState (RENDER_SHADOWS_BIT))  render->ChangeState (RENDER_SHADOWS_BIT,FALSE);
        else                                        render->ChangeState (RENDER_SHADOWS_BIT,TRUE);
      }  

      if (key == VK_5)
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

      if (key == VK_6)
      {
        if (render->GetState (RENDER_CLEAR_DEPTH_BUFFER_BIT) )
          render->ChangeState (RENDER_CLEAR_DEPTH_BUFFER_BIT,FALSE);
        else  
          render->ChangeState (RENDER_CLEAR_DEPTH_BUFFER_BIT,TRUE);
      }  

      if (key == VK_9)
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

    if (key == VK_OEM_3)
    {
      render->GetConsole ()->ChangeMode ();
      conOpen = !conOpen;
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
//    float        height = mMap->bbox.max.y;
    float        height = 0;

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
//        if (temp.y < height) height = temp.y;
          if (temp.y > height) height = temp.y;
    }

    return height;
//    return height != mMap->bbox.max.y ? height : 0;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    dout<<"Error at GetMinHeight"<<endl;
    return 0;
  }
}

void     CmdQuit    (VarPool&,uint,ParamStack&,ostream&,DWORD)
{
  W32_PostQuitMessage (0);
}