#define  DEBUG
#include <kernel\app.h>
#include <kernel\fswindow.h>
#include <kernel\smpwindow.h>
#include <graph\oglview.h>
#include <engine\utils.h>
#include <engine\camera.h>
#include <misc.h>
#include <gl\glu.h>

#include "g_level.h"

const int       SCREEN_WIDTH  = 1024;
const int       SCREEN_HEIGHT = 768;

const float     TURN_ANGLE    = 3.14f / 72.0f;
const float     STEP          = 1.0f;
const float     ANGLE         = 45 / 57.3f;
const float     DEPTH         = 200.0f;
const int       MAX_NODES     = 8192;
const int       MAX_LINES     = 1024;

const uint16    VK_A          = 0x41;
const uint16    VK_W          = 0x41 + 'W' - 'A';
const uint16    VK_S          = 0x41 + 'S' - 'A';
const uint16    VK_D          = 0x41 + 'D' - 'A';

const uint16    VK_1          = 0x41 + '1' - 'A';
const uint16    VK_2          = 0x41 + '1' - 'A';

const int       X_CENTER      = SCREEN_WIDTH / 2;
const int       Y_CENTER      = SCREEN_HEIGHT / 2;
const float     X_SENS        = -.002;
const float     Y_SENS        = .002;

int      start    (Application*,char*);
void     Draw     (HDC);
void     Reshape  (int,int,HDC);
void     Idle     (Application*);
int      Keyb     (Window*,UINT,WORD,DWORD,DWORD);
void     Move     ();
void     DrawNet  ();

void     InsertNode (int,int,int);
void     ResetNodes ();
void     Line       (int,int,int,int,int);
void     _Line      (int,int,int,int,int);

struct   xnode_t
{
  int   count;
  int   data [MAX_NODES];
};

int           size ;
HDC           hdc;
GameLevel*    lev;
float         step;
boundbox_t    box;
int           mapWidth, mapHeight;
xnode_t       nodes [MAX_LINES];

char*         flagTable;
char          mask  [8] = {1,2,4,8,16,32,64,128};
int           flagSize  = 0;

inline void   set   (int i) { flagTable [i>>3] |= mask [i&7];  }
inline BOOL   check (int i) { return flagTable [i>>3] & mask [i&7]; }
inline void   reset ()      { memset (flagTable,0,flagSize); }

float           xangle        = 0;
float           yangle        = 0;
int             kpress [1024] = {0};

A_Start (start,64000000);

Camera            eye      (vector3d_t(0,3.5f,30),
                            vector3d_t(0,0,-1),     
                            vector3d_t(0,1,0)
                           ); 

int      start (Application* app,char*)
{
  FloatPool     pool (M_GetPool (),64000000);

  M_PushPool (&pool);

  stdBinFile    file (M_GetPool (),"park.map",BinFile::OPEN_RO);
  GameLevel     level  (&pool,&file,40000000);

  lev            = &level;
  size           = lev->mGraphMap->width * lev->mGraphMap->height;
  flagTable      = new char [lev->mGraphMap->fCount / 8];
  flagSize       = lev->mGraphMap->fCount / 8;
  step           = lev->mGraphMap->step;
  mapWidth       = lev->mGraphMap->width;
  mapHeight      = lev->mGraphMap->height;
  box            = lev->mGraphMap->bbox;
  eye.set (lev->mGraphMap->bbox.max / 2.0f,eye.dir (), eye.up());

  FullScreenWindow* wnd = new (app->pool())
                      FullScreenWindow (app->pool(),app->instance (),rect_t (0,0,SCREEN_WIDTH,SCREEN_HEIGHT),"Test");
  wnd->ShowCursor (FALSE);
  OpenGLView    ogl  (wnd,NULL,Draw,Reshape);  

  wnd->InsertHandler (WM_KEYDOWN,Keyb);
  wnd->InsertHandler (WM_KEYUP,Keyb);
  wnd->InsertHandler (WM_MOUSEMOVE,Keyb);
  app->SetIdle       (Idle);

  int state = app->MainLoop ();

  delete wnd;

  PostQuitMessage (0);

  M_PopPool ();

  delete [] flagTable ;

  return state;
}

void     Draw     (HDC hdc)
{
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix ();

  gluLookAt    (eye.pos().x,eye.pos().y,eye.pos().z,
                eye.dir().x+eye.pos().x,eye.dir().y+eye.pos().y,eye.dir().z+eye.pos().z,
                0,1,0
               );

  DrawNet ();    

  int   sx   = eye.pos ().x / step,
        sy   = eye.pos ().z / step;
  float dirx = eye.dir ().x,
        diry = eye.dir ().z;  
  float _div = 1.0f/sqrt (dirx*dirx + diry*diry);
  dirx      *= _div;
  diry      *= _div;
  double xt1,yt1,xt2,yt2;
  float dx1  = dirx*cos(ANGLE)-diry*sin(ANGLE),
        dy1  = dirx*sin(ANGLE)+diry*cos(ANGLE),
        dx2  = dirx*cos(-ANGLE)-diry*sin(-ANGLE),
        dy2  = dirx*sin(-ANGLE)+diry*cos(-ANGLE),
        x1   = sx,
        y1   = sy,
        x2   = sx,
        y2   = sy;

  int   depth= abs(DEPTH / (cos (ANGLE) * step));

        //Флаг обработки тайла

  for (int i=0;i<depth;i++,x1+=dx1,y1+=dy1,x2+=dx2,y2+=dy2)
  {
    modf (x1,&xt1);
    modf (x2,&xt2);
    modf (y1,&yt1);
    modf (y2,&yt2);

    Line (xt1,yt1,xt2,yt2,i);
  }

  glPopMatrix ();

  glFinish ();

  SwapBuffers (hdc);    
}

void     DrawNet  ()
{
  glColor3f (0,0,1);
  
  for (int y=0;y<mapHeight;y++)
  {
    float yf = float (y) * step;
    float xf = 0;

    for (int x=0;x<mapWidth;x++,xf+=step)
    {
      glBegin (GL_LINE_LOOP);
        glVertex2f (float (x)*step,float(y)*step);
        glVertex2f (float (x+1)*step,float(y)*step);
        glVertex2f (float (x+1)*step,float(y+1)*step);
        glVertex2f (float (x)*step,float(y+1)*step);
      glEnd   ();
    }
  }
}

void _Line(int xs,int ys,int xe,int ye,int level)
{
  InsertNode (xs,ys,level);
  int dx=xe-xs;
  int dy=abs(ye-ys);
  if(dy<=dx)
  {
    int d,d1,d2=(dy-dx)<<1;
    d=(d1=dy<<1)-dx;
    if(ye>ys)
      for(;dx--;InsertNode(++xs,ys,level))
      {
        if(d>0)
        {
          d+=d2;
          ys++;
        }
        else
          d+=d1;
      }
    else
      for(;dx--;InsertNode(++xs,ys,level))
      {
              if(d>0)
        {
          d+=d2;
          ys--;
        }
        else
          d+=d1;
      }
  }
  else
  {
    int d,d1,d2=(dx-dy)<<1;
    d=(d1=dx<<1)-dy;
    if(ye>ys)
      for(;dy--;InsertNode(xs,++ys,level))
      {
        if(d>0)
        {
          d+=d2;
          xs++;
        }
        else
          d+=d1;
      }
    else
      for(;dy--;InsertNode(xs,--ys,level))
      {
        if(d>0)
        {
          d+=d2;
          xs++;
        }
        else
          d+=d1;
      }
  }
}

void Line(int xs,int ys,int xe,int ye,int level)
{
        if(xs>xe)
        {
                swap(xs,xe);
                swap(ys,ye);
        }
        if(xs>=mapWidth || xe<0)
                return;
        if(xs<0)
        {
                ys+=(long)(ye-ys)*(-xs)/(xe-xs);
                xs=0;
        }
        if(xe>=mapWidth)
        {
                ye+=(long)(ye-ys)*(mapWidth-1-xe)/(xe-xs);
                xe=mapWidth-1;

        }
        if(ys<0)
        {
                if(ye<0) return;
                xs+=(long)(xe-xs)*(-ys)/(ye-ys);
                if(xs>xe) return;
                ys=0;               
        }
        else
                if(ys>=mapHeight)
                {
                        if(ye>=mapHeight) return;
                        xs+=(long)(xe-xs)*(mapHeight-1-ys)/(ye-ys);
                        if(xs>xe)         return;
                        ys=mapHeight-1;
                }
        if(ye<0)
        {
                xe+=(long)(xe-xs)*(-ye)/(ye-ys);
                ye=0;
        }
        else
                if(ye>=mapHeight)
                {
                        xe+=(long)(xe-xs)*(mapHeight-1-ye)/(ye-ys);
                        ye=mapHeight-1;
                }

        _Line (xs,ys,xe,ye,level);
}

void     InsertNode (int x,int y,int level)
{
  glColor3f (1,0,0);

  glBegin (GL_QUADS);
    glVertex2f (float (x)*step,float(y)*step);
    glVertex2f (float (x+1)*step,float(y)*step);
    glVertex2f (float (x+1)*step,float(y+1)*step);
    glVertex2f (float (x)*step,float(y+1)*step);
  glEnd   ();
}

void     ResetNodes ()
{
  for (int i=0;i<MAX_LINES;nodes [i++].count = 0);
}

void     Reshape  (int width,int height,HDC _hdc)
{
  glViewport       (0,0,width,height);
  glMatrixMode     (GL_PROJECTION);
  glLoadIdentity   ();

  glOrtho2d        (-width,-height,width,height);

  glMatrixMode     (GL_MODELVIEW);
  glLoadIdentity   ();

  glClearColor     (1,1,1,1);
  glClear          (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  hdc = _hdc;
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

    SetCursorPos (X_CENTER,Y_CENTER);

    xangle     += xOffs;
    yangle     += yOffs;
    eye.set (eye.pos(),matrix_t::rotate (vector3d_t (0,1,0),-xOffs)*eye.dir (),eye.up());

    vector3d_t  right  = eye.up () ^ eye.dir();

    eye.set (eye.pos(),matrix_t::rotate (right,yOffs)*eye.dir (),eye.up());

    state       = TRUE;
  }
  else if (msg == WM_KEYDOWN && key < 1024)
    kpress [key] = TRUE;
  else if (msg == WM_KEYUP && key < 1024)
    kpress [key] = FALSE;  
 
  return 0;
}

void  Idle  (Application*)
{
  static clock_t time  = clock ();
  static clock_t last  = time;
  static int     total = 0;

  Move ();
  Draw (hdc);

  total++;

  if (clock () - last > CLK_TCK << 1)
  {
    dout<<"FPS: "<<float(total*CLK_TCK)/float(clock()-time)<<endl;
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
  {
    xangle -= TURN_ANGLE;
    eye.set (eye.pos(),matrix_t::rotate (vector3d_t (0,1,0),TURN_ANGLE)*eye.dir (),eye.up());
  }
  
  if (kpress [VK_RIGHT])
  {
    xangle += TURN_ANGLE;
    eye.set (eye.pos(),matrix_t::rotate (vector3d_t (0,1,0),-TURN_ANGLE)*eye.dir (),eye.up());
  }


  if (kpress [VK_ESCAPE])
      PostQuitMessage (0);  
}
