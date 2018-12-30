#include "pch.h"
#include <kernel\keydefs.h>
#include <window.h>
#include <kernel\app.h>
#include <misc.h>
#include <log.h>

#include <ogl.h>
#include <default.h>

#include <graph\glext.h>
#include "camera.h"
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

#include "i_main.h"

#include "cv_mem.h"

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
void     Move     ();

void     InitFonts ();
void     DoneFonts ();

void     InitConsole();
void     InitLog    ();
void     DoneConsole();
void     DoneLog    ();
void     CmdQuit    (VarPool&,uint,ParamStack&,ostream&,DWORD);
void     Rain       ();
void     Kadr25     ();
                        
void     SetDayTime ();
void     AnimModels ();

void     DrawFog    ();

float    GetHeight     (const vector3d_t& v,LevelLoader& loader);

class    MoveInputClient: public InputClient
{
  public:
                MoveInputClient (InputTranslator& input): InputClient (input) {}

   virtual void event        (const KeyEvent&);
   virtual void event        (const MouseEvent&);
}; 

class    ConsoleInputClient: public InputClient
{
  public:
                ConsoleInputClient (InputTranslator& input): InputClient (input) {}

   virtual void event        (const KeyEvent&);
}; 

//StaticInit              init;
FloatPool*              mainPool     = NULL;
//FloatPool               pool        (M_GetPool (),16*1024*1024); 
//Debug                   debug       (&pool);
ConsoleBase*            con          = NULL;
InputTranslator*        input        = NULL;
MoveInputClient*        moveInput    = NULL;
ConsoleInputClient*     consoleInput = NULL;
LevelLoader*            levelLoader  = NULL; 
GraphConsole*           grCon        = NULL;
Command*                command      = NULL;
CommandLine*            comLine      = NULL;                        
Window*                 conWindow    = NULL;

boundbox_t              box;
MainRender*             render        = NULL;
int                     kpress [1024] = {0};

Camera   eye      (vector3d_t(.0f),
                   vector3d_t(0,0,1),     
                   vector3d_t(0,1,0)
                  ); 
vector3d_t c (.0f);
int        li [16] = {-1};
int        lightCount = 0;
float      fps        = 0;
ulong      tps        = 0;
int        frame_cnt  = 0;

MODEL      mdl [32] = {-1};
ModelEnv*  me       = NULL;
SkyLight*  sky      = NULL;
SHADOW     shadow, shadow1;
int        test_pgcount  = 0;
BOOL       free_look     = FALSE;
BOOL       cull_face     = 0;

////////////////////////////////////////////////////////////////////////////////
///Глюк, без массива прога вылетает !!!
////////////////////////////////////////////////////////////////////////////////
static     uint glukBigArray [0x10000];

int main (int,char* [])
{
//  Application* app      = A_GetApplication ();  

  mainPool = ext_new  FloatPool (M_GetPool (),32*1024*1024); 
  con      = ext_new  ConsoleBase (mainPool,"logout",512,COMMAND_LINE_LEN);

  M_PushPool            (mainPool);  

  InitLog               ();

  LogMessage            ("-----------Windows startup------------");
  LogMessage            ("...Create console");

  InitConsole           ();

  LogMessage            ("...Create full screen windows");

  Window*      wnd      = ext_new FullScreenWindow (M_GetPool (),
                                            rect_t (0,0,SCREEN_WIDTH,SCREEN_HEIGHT),
                                            "Render test"
                                           );
  LogMessage            ("...Set handlers");

  wnd->ShowCursor       (FALSE);
  wnd->InsertHandler    (WM_DESTROY,WND_Done);
//  wnd1->ShowCursor      (FALSE);

  RENDER_VIEW_DEPTH *= 8;
//  RENDER_VIEW_DEPTH /= 4;
//  RENDER_VIEW_DEPTH /= 128;

//  DoneConsole           ();

  LogMessage            ("---------Init input system----------------");

  input                 = new InputTranslator (mainPool,wnd);
  moveInput             = new MoveInputClient (*input);
  consoleInput          = new ConsoleInputClient (*input);

  input->SetFocus       (InputTranslator::ID_KEYBOARD,moveInput);
  input->SetFocus       (InputTranslator::ID_MOUSE   ,moveInput);
  input->SetMousePos    (X_CENTER,Y_CENTER);

  LogMessage            ("---------Init OpenGL system---------------");

  LogMessage            ("...Load OpenGL unit");

  SrvMakeCurrent (OGL_Load  ());
  OGL_DisplayMode       (OGL_DOUBLE | OGL_DEPTH | OGL_RGB); //RGB!@!!!!!!!!
  OGL_ReshapeFunc       (Reshape);
  OGL_DrawFunc          (Draw);

  LogMessage            ("...Create context");

  OGL_CreateContext     (OGL_CONTEXT0,wnd);
  LogMessage            ("...Init extensions table");
//  InitExtensions        ();
  LogMessage            ("...Start OpenGL unit");
  OGL_Start             (OGL_RESHAPE_CALL);
  
  F_AddFastPath         ("textures","textures/");

  LogMessage            ("------------Game init--------------");
  LogMessage            ("...Create static data");

  LevelLoader           level (M_GetPool(),"maps/park.map");
  levelLoader  = &level;

  level.Load            ();

  if (!IsValid (&level))
  {
    D_Message ("::Error at load level");
//    return;
  }

  LogMessage            ("-------------Render init-------------");

  InitFonts             ();

  LogMessage            ("...Create render");

  render       = new (M_GetPool()) MainRender (M_GetPool(),level);

  if (!IsValid (render))
  {
    D_Message ("::Error at create render");
//    return;
  }

  me           = render->GetModelEnv ();

  render->BindConsole  (*con,0,60,25);
  render->ChangeState  (RENDER_SHADOWS_BIT,FALSE);
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
    me->LoadModelData (kpr [1],FALSE,"models/tree.wmd"),
    me->LoadModelData (kpr [1],FALSE,"models/tree0.wmd"),
    me->LoadModelData (kpr [1],FALSE,"models/tree1.wmd"),
    me->LoadModelData (kpr [1],FALSE,"models/baba.wmd"),

    me->LoadModelData (kpr [2],FALSE,"models/sprite1.mdl"),
    me->LoadModelData (kpr [1],FALSE,"models/palma.wmd"),
    me->LoadModelData (kpr [0],FALSE,"models/vishnya.wmd"),
    me->LoadModelData (kpr [1],FALSE,"models/tree2.wmd"),

    me->LoadModelData (kpr [1],FALSE,"models/tree3.wmd"),
    me->LoadModelData (kpr [3],FALSE),
    me->LoadModelData (kpr [1],FALSE,"models/palma1.wmd"),
    me->LoadModelData (kpr [1],TRUE,"models/tree4.wmd"),
  };  

  LogMessage ("...Sky init");

  sky = new (M_GetPool()) 
        SkyLight (M_GetPool(),render->GetLightEnv (),render->GetSkyBox (),box);

  sky->SetShine (0.1);
//  sky->SetShine (0.85);

  LogMessage ("...Create models");

  for (int i=0;i<32;mdl [i++] = -1);

//  mdl [4] = me->CreateModel (md [7],c+vector3d_t (0,14,0),matrix_t::scale (vector3d_t (1)));

  mdl [4] = me->CreateModel (md [6],c+vector3d_t (0,14,0),matrix_t::scale (vector3d_t (1)));
  mdl [0] = me->CreateModel (md [11],c+vector3d_t (50,14,22),matrix_t::scale (vector3d_t (1)));
  mdl [1] = me->CreateModel (md [11],c+vector3d_t (-50,14,-22),matrix_t::scale (vector3d_t (1)));

  mdl [5] = me->CreateModel (md [4],c+vector3d_t (40,14,30),0,15,15); //sprites

//  mdl [1] = me->CreateModel (md [4],c+vector3d_t (20,14,10),0,10,10);//sprites
//  mdl [2] = me->CreateModel (md [4],c+vector3d_t (22,14,12),0,10,10);
//  mdl [3] = me->CreateModel (md [11],c+vector3d_t (25,24,-22),matrix_t(1));

  eye.set (box.max / 2.0f,eye.dir, eye.up);

  LogMessage ("-----------Main loop------------");  

  MainLoop              (Idle);  

  LogMessage            ("------------Done system------------");

  DoneFonts             ();

  LogMessage            ("...Delete render");

  delete render;

  LogMessage            ("...Done OpenGL");

  OGL_Done              ();

  LogMessage            ("...Delete windows");

  delete moveInput;
  delete consoleInput;
  delete input;
  delete wnd;

  LogMessage            ("...Delete console");  
  DoneConsole           ();
  LogMessage            ("...Close log");
  DoneLog               ();

  M_PopPool             ();    

  dout<<"All ok exit application"<<endl;

  return EXIT_SUCCESS;
}

void     InitFonts ()
{
  LogMessage       ("...Init fonts");
  LoadFont         (0,"textures\\font.tga",M_GetPool(),0,128,0,16*8);
  LoadFont         (1,"textures\\font.tga");
  SetFont          (0);
  SetScreenSize    (SCREEN_WIDTH,SCREEN_HEIGHT);
}

void     DoneFonts ()
{
  LogMessage     ("...Done fonts");
  DestroyFont    (0);
  DestroyFont    (1);
}

void     InitConsole()
{
  Window*      wnd = ext_new SimpleWindow (M_GetPool (),
                                       rect_t (200,200,400,300),
                                       "Wood GDI console"
                                      );

  con->refresh (TRUE);

  grCon     = ext_new GDIConsole (M_GetPool(),wnd,*con);
  conWindow = wnd;

//  con.refresh (TRUE);
//  con.refresh ();
//  con.bind    (grCon);  
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

  if (logOut)
    delete logOut;  

  logOut  = ext_new ConLog (*con);
  command = ext_new Command (M_GetPool(),128);
  comLine = ext_new CommandLine (M_GetPool(),*con,*command,COMMAND_LINE_LEN,8);

  command->LoadConfig ("config.cfg"); 

  command->bind ("quit",CmdQuit);
}

void     DoneConsole()
{
  delete grCon;
  delete conWindow;
  conWindow = NULL; 
  grCon     = NULL;
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

BOOL     LevelLoader::IsValid  () const
{
  return ::IsValid (access ()->mLandMap) &&
         ::IsValid (access ()->mVertexBuffer);
}

BOOL     LevelLoader::LoadData ()
{
  levelmemavail_t meminfo;

  LoadMemoryInfo     (level,meminfo);

  access()->mLandMap       = new (objPool) LandscapeMap   (objPool,*this,meminfo.land_cache.mem_max*1.2);
  access()->mVertexBuffer  = new (objPool) VertexBuffer (objPool,RENDER_VERTEX_BUFFER_SIZE);

  return TRUE;
}

void     LevelLoader::UnloadData ()
{
  delete access()->mLandMap;
  delete access()->mVertexBuffer;
}

void     Draw       ()
{  
  Camera Camera      = eye;

  (*levelLoader)->mLandMap->SetPos (eye.pos);

  float  height      = GetHeight       (Camera.pos,*levelLoader) + 
                       (*levelLoader)->mLandMap->bbox.max.y * 0.0005;

  Camera.set (vector3d_t (Camera.pos.x,height*1.1,Camera.pos.z),
              Camera.dir,
              Camera.up
             );  

  glTexEnvi             (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL); 

  render->Draw          (Camera);

  DrawFog               ();
  Rain                  ();

//  glTexEnvi             (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL); 

  glColor3f (0,1,0);    OutText  (10,30,"FPS: %4.1f",fps);
  glColor3f (0,1,1);    OutText  (10,45,"TPS: %u",tps);
  glColor3f (1,0,1);    OutText  (10,60,1,"Light count: %u",lightCount);

  glColor3f (1,1,1);    OutText  (SCREEN_WIDTH*0.7,30,1, "glFinish:      %u",RENDER_GL_FINISH);
  glColor3f (1,1,1);    OutText  (SCREEN_WIDTH*0.7,45,1, "GL_FOG:        %u",glIsEnabled (GL_FOG));
  glColor3f (1,1,1);    OutText  (SCREEN_WIDTH*0.7,60,1, "GL_TEXTURE_2D: %u",glIsEnabled (GL_TEXTURE_2D));
  glColor3f (1,1,1);    OutText  (SCREEN_WIDTH*0.7,75,1, "Sky:           %u",render->GetState (RENDER_SKY_BIT));
  glColor3f (1,1,1);    OutText  (SCREEN_WIDTH*0.7,90,1, "Shadows:       %u",render->GetState (RENDER_SHADOWS_BIT));
  glColor3f (1,1,1);    OutText  (SCREEN_WIDTH*0.7,105,1,"Free look:     %u",free_look);
  glColor3f (1,1,1);    OutText  (SCREEN_WIDTH*0.7,120,1,"Cull face:     %u",cull_face);

//  if (!(frame_cnt % 25))
//    Kadr25 ();
  
  if (RENDER_GL_FINISH)  glFinish ();
  else                   glFlush  ();

  OGL_SwapBuffers       ();   
}

void     Reshape  (int width,int height)
{
  glViewport       (0,0,width,height);
  glMatrixMode     (GL_PROJECTION);
  glLoadIdentity   ();

  gluPerspective   (RENDER_VIEW_ANGLE*57.3f,(float)width/(float)height,1.0,RENDER_VIEW_DEPTH*0.95);

  glMatrixMode     (GL_MODELVIEW);
  glLoadIdentity   ();

  glEnable         (GL_DEPTH_TEST); 
  glEnable         (GL_LIGHTING);
  glEnable         (GL_CULL_FACE);
  glEnable         (GL_TEXTURE_2D);

  glFrontFace      (GL_CCW);
  glCullFace       (GL_CW);
//  glFrontFace      (GL_CW);
//  glCullFace       (GL_CCW);

  glClearColor     (0.1,0.2,0.3,0.5);
  glClear          (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  GLfloat          col [4] = {0.2,0.2,0.21,1};

//  glFogf           (GL_FOG_MODE,GL_LINEAR);
  glFogf           (GL_FOG_MODE,GL_EXP2);
//  glFogf           (GL_FOG_DENSITY,0.005);
  glFogf           (GL_FOG_DENSITY,0.0005);
  glFogf           (GL_FOG_START,RENDER_VIEW_DEPTH*0.25);
  glFogf           (GL_FOG_END,RENDER_VIEW_DEPTH*0.75);
  glFogfv          (GL_FOG_COLOR,col);

  glBlendFunc      (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

void  Idle  ()
{
  static clock_t last  = 0;
  static int     total = 0;

  SetDayTime ();
//  AnimModels ();
  Move ();
     
  total++;
  frame_cnt++;

  if (clock () - last > CLK_TCK >> 1)
  {
    int pgcount = render->GetPgCount () + test_pgcount;

    fps         = float(total*CLK_TCK)/float(clock()-last);
    tps         = float(pgcount*CLK_TCK)/float(clock()-last);    

//    dout<<"FPS: "<<fps<<" TPS: "<<tps<<endl;

    last  = clock ();
    total = 0;
    test_pgcount = 0;
  }

  Draw ();
}

void     DrawFog    ()
{
  
}

void     Rain       ()
{
  const  int        N   = 5000;
  static vector3d_t rain  [N];
  static BOOL       init = FALSE;
  static float      speed = 3;
  vector3d_t        dir  = normalize (vector3d_t (0.1,-1,0));

  if (!init)
  {  
    vector3d_t d = box.max - box.min;

    d           *= 3;

    for (int i=0;i<N;i++)
    {
      rain [i] = vector3d_t (randomfl()*d.x,randomfl()*d.y,randomfl()*d.z);
    }    

    init = TRUE;
  }  
  else
  {
    vector3d_t d = box.max - box.min;

    d               *= 3;
    vector3d_t step  = dir * speed;

    for (int i=0;i<N;i++)
    {
      rain [i] += step;

      if (rain [i].y < 0)
        rain [i] = vector3d_t (randomfl()*d.x,randomfl()*d.y,randomfl()*d.z);
    }    
  }

  glPushAttrib (GL_ENABLE_BIT);

  glEnable   (GL_COLOR_MATERIAL);
  glEnable   (GL_BLEND);
//  glDisable  (GL_LIGHTING);
  glDisable  (GL_TEXTURE_2D);
  glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glColor4f  (0.4,0.8,1,0.5);

  glBegin    (GL_LINES);
    for (int i=0;i<N;i++)
    {
      glVertex3fv ((GLfloat*)&rain [i]);
      glVertex3f  (rain [i].x+dir.x,rain [i].y+dir.y,rain [i].z+dir.z);
    }
  glEnd      ();

  glPopAttrib ();

  static clock_t last = 0;
  static clock_t pause = 0;
  static BOOL    state = 0;
  static LIGHTSRC lt = -1, lt1 = -1;
  static lightsrc_t light,light1;
  static count      = 0;

  if (!state && clock () - last > pause)
  {
    vector3d_t d = box.max - box.min;

    memset (&light,0,sizeof (light));

//    light.light.diffuse    = vector3d_t (0,0,0);
//    light.light.specular   = vector3d_t (1,1,1);
//    light.light.ambient    = vector3d_t (1,1,1);
    light.light.diffuse    = vector3d_t (0.3,0,0);
    light.light.specular   = vector3d_t (0,0.3,0.8);
    light.light.ambient    = vector3d_t (0.2,0.2,0.2);
    light.light.shiness    = 100.0;
    light.light.angle      = 3.14f / 2.0f;
    light.light.pos        = vector3d_t (d.x/2,250,d.z/2);
//    light.light.pos        = vector3d_t (randomfl ()*d.x,300,randomfl ()*d.z);
    light.light.dir        = vector3d_t (0,-1,0);
    light.light.farlight   = TRUE;
    light.shiness          = 1.0;

    sky->SetShine (0.8);
    lt                     = render->GetLightEnv ()->NewLight (light,TLE_GLOBAL_LIGHT|TLE_ENABLE_LIGHT);
//    lt1                    = render->GetLightEnv ()->NewLight (light1,TLE_GLOBAL_LIGHT|TLE_ENABLE_LIGHT);
    last                   = clock ();
    state                  = 1;
    count                  = random (8);  

    shadow   = render->GetShadowEnv()->CreateShadow (lt,mdl [4]);

    render->GetShadowEnv()->EnableShadow (shadow);
//    render->GetShadowEnv()->ShadowClean  (shadow);
    render->GetShadowEnv()->refresh      (shadow);
  }
  else if (state && clock () - last > CLK_TCK >> 4)
  {
    if (state < count)
    {
      light.shiness = randomfl ();
      render->GetLightEnv ()->ChangeLight (lt,light);
      sky->SetShine (randomfl ());
      state++;
    }
    else
    {
      render->GetShadowEnv()->DeleteShadow (shadow);
      render->GetLightEnv ()->DeleteLight (lt);
      sky->SetShine (0.1);
      state = 0;
//      pause = random (CLK_TCK*8);
      pause = random (CLK_TCK*2);
    }

    last   = clock ();
  }

  test_pgcount += N;
}

void     Kadr25 ()
{

}

void     AnimModels ()
{   
  me->ChangeProperties (mdl [7],EFFECT_TICK);
}

void     SetDayTime ()
{
  static  float   time1 = 6;
  static  float   step  = 0.01f;
  static  clock_t last  = 0;

  if (clock () - last > CLK_TCK >> 4)
  {
    time1 += step;

    if (time1 >= 24.0f) time1 = 0.0f;

    sky->refresh (time1);

    static clock_t last1 = 0;
    static clock_t last2 = 0;

    if (clock () - last1 > CLK_TCK )
    {
      render->GetShadowEnv()->refresh (shadow);
      last1 = clock ();
    }  

    last = clock ();
  }
}

void Move ()
{
  if (kpress [VK_UP] || kpress [VK_W])
      eye.set (eye.pos + eye.dir*STEP,eye.dir,eye.up);

  if (kpress [VK_DOWN] || kpress [VK_S])
      eye.set (eye.pos - eye.dir*STEP,eye.dir,eye.up);

  if (kpress [VK_A])
      eye.set (eye.pos + matrix_t::rotate (vector3d_t (0,1,0),3.14f/2.0f)*eye.dir*STEP,eye.dir,eye.up);  

  if (kpress [VK_D])
     eye.set (eye.pos + matrix_t::rotate (vector3d_t (0,1,0),-3.14f/2.0f)*eye.dir*STEP,eye.dir,eye.up);  

  if (kpress [VK_LEFT])
   eye.set (eye.pos,matrix_t::rotate (vector3d_t (0,1,0),TURN_ANGLE)*eye.dir,eye.up);

  if (kpress [VK_RIGHT])
   eye.set (eye.pos,matrix_t::rotate (vector3d_t (0,1,0),-TURN_ANGLE)*eye.dir,eye.up);


  if (kpress [VK_ESCAPE])
     W32_PostQuitMessage (0);  

  vector3d_t pos = eye.pos;

  if         (pos.x < box.min.x) pos.x = box.min.x;
  else if    (pos.x > box.max.x) pos.x = box.max.x;
  if         (pos.y < box.min.y) pos.y = box.min.y;
  else if    (pos.y > box.max.y) pos.y = box.max.y;
  if         (pos.z < box.min.z) pos.z = box.min.z;
  else if    (pos.z > box.max.z) pos.z = box.max.z;

  eye.set (pos,eye.dir,eye.up);
}

void MoveInputClient::event        (const KeyEvent& event)
{
  switch (event.msg)
  {
    case KeyEvent::KE_DOWN:
    {
      kpress [event.key] = TRUE;

      switch (event.key)
      {
        case VK_OEM_3:
          input().SetFocus (InputTranslator::ID_KEYBOARD,consoleInput);
          render->GetConsole ()->ChangeMode ();
          break;
        case VK_1:
          RENDER_GL_FINISH = !RENDER_GL_FINISH;
          break;
        case VK_2:
          if (glIsEnabled (GL_FOG))  glDisable (GL_FOG);
          else                       glEnable  (GL_FOG);
          break;
        case VK_3:
          if (glIsEnabled (GL_TEXTURE_2D))  glDisable (GL_TEXTURE_2D);
          else                              glEnable  (GL_TEXTURE_2D);
          break;
        case VK_4:
          if (render->GetState (RENDER_SHADOWS_BIT))  render->ChangeState (RENDER_SHADOWS_BIT,FALSE);
          else                                        render->ChangeState (RENDER_SHADOWS_BIT,TRUE);
          break;
        case VK_5:
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
          break;
        case VK_6:
          if (render->GetState (RENDER_CLEAR_DEPTH_BUFFER_BIT) )
            render->ChangeState (RENDER_CLEAR_DEPTH_BUFFER_BIT,FALSE);
          else  
            render->ChangeState (RENDER_CLEAR_DEPTH_BUFFER_BIT,TRUE);
          break;  
        case VK_7:
          free_look = !free_look; 
          break;
        case VK_8:
           cull_face = !cull_face;
           if (!cull_face)
           {
             glFrontFace      (GL_CCW);
             glCullFace       (GL_CW);
           }  
           else
           {    
             glFrontFace      (GL_CW);
             glCullFace       (GL_CCW);
           }
          break;  
        case VK_9:
        {
          lightsrc_t light;

          memset (&light,0,sizeof (light));

          light.light.diffuse    = vector3d_t (randomfl(),randomfl(),randomfl());
          light.light.specular   = vector3d_t (randomfl(),randomfl(),randomfl());
          light.light.ambient    = vector3d_t (randomfl(),randomfl(),randomfl());
          light.light.shiness    = randomfl ();
          light.light.angle      = 3.14f / 3.0f;
          light.light.pos        = eye.pos;
          light.light.dir        = eye.dir;
          light.light.farlight   = FALSE;
          light.shiness          = randomfl ();

    //      li [lightCount++]      = render->GetLightEnv ()->NewLight (light,TLE_GLOBAL_LIGHT|TLE_ENABLE_LIGHT);
          li [lightCount++]      = render->GetLightEnv ()->NewLight (light);
          break;
        }
      }
      break;
    }
    case KeyEvent::KE_UP:
      kpress [event.key] = FALSE;
      break;
    default:
      break;
  }
}

void ConsoleInputClient::event        (const KeyEvent& event)
{
  switch (event.msg)
  {
    case KeyEvent::KE_LOOP:
    case KeyEvent::KE_DOWN:
    {
      switch (event.key)
      {
        case VK_PRIOR:      con->ScrollUp   ();     return;
        case VK_NEXT:       con->ScrollDown ();     return;
        case VK_UP:         comLine->prevcmd ();   return;
        case VK_DOWN:       comLine->nextcmd ();   return;
        case VK_LEFT:       comLine->prev ();      return;
        case VK_RIGHT:      comLine->next ();      return;
        case VK_HOME:       comLine->home ();      return;
        case VK_END:        comLine->end  ();      return;
        case VK_DELETE:     comLine->del  ();      return;
        case VK_ESCAPE:
        case VK_OEM_3:
          render->GetConsole ()->ChangeMode ();
          input().SetFocus (InputTranslator::ID_KEYBOARD,moveInput);
          return;
        case VK_SHIFT:
        case VK_CONTROL:
          return;
        default:
          break;
      }

      comLine->put (event.simb);
    }
    case KeyEvent::KE_UP:
    default:
      break;
  }
}

void MoveInputClient::event        (const MouseEvent& event)
{
  switch (event.msg)
  {
    case MouseEvent::ME_MOVE:
    {
      static state = FALSE;

      if (state)
      {
        state = FALSE;
        return;
      }

      float       xOffs  = 2.0f*float(event.x-X_CENTER)/float(SCREEN_WIDTH);
      float       yOffs  = 2.0f*float(event.y-Y_CENTER)/float(SCREEN_HEIGHT);

      input().SetMousePos (X_CENTER,Y_CENTER);

      eye.set (eye.pos,normalize (matrix_t::rotate (vector3d_t (0,1,0),-xOffs)*eye.dir),eye.up);

      vector3d_t  right  = eye.up ^ eye.dir;

      eye.set (eye.pos,normalize (matrix_t::rotate (right,yOffs)*eye.dir),eye.up);

      state       = TRUE;

      break;
    }  
    default:
      break;
  }
}

float    GetHeight     (const vector3d_t& v,LevelLoader& loader)
{
  if (free_look)
    return v.y;

  return loader->mLandMap->CalcMaxHeight (v);
}

void     CmdQuit    (VarPool&,uint,ParamStack&,ostream&,DWORD)
{
  W32_PostQuitMessage (0);
}