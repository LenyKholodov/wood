#include <kernel\app.h>
#include <kernel\fswindow.h>
#include <kernel\smpwindow.h>
#include <graph\oglview.h>
#include <graph\utils.h>
#include <graph\camera.h>
#include <misc.h>
#include <gl\glu.h>
#include <fixed\tables.h>

#include "g_level.h"
#include "r_sbuffer.h"

const int       SCREEN_WIDTH  = 1024;
const int       SCREEN_HEIGHT = 768;

const float     TURN_ANGLE    = 3.14f / 72.0f;
const float     STEP          = 1.0f;
const float     ANGLE         = 45 / 57.3f;
const Angle     FX_ANGLE      = float2angle (ANGLE);
const float     DEPTH         = 1500.0f;
const int       MAX_NODES     = 8192;
const int       MAX_LINES     = 1024;
const int       MAX_VIS_NODES = 2048;

const uint16    VK_A          = 0x41;
const uint16    VK_W          = 0x41 + 'W' - 'A';
const uint16    VK_S          = 0x41 + 'S' - 'A';
const uint16    VK_D          = 0x41 + 'D' - 'A';

const uint16    VK_1          = 0x41 + '1' - 'A';
const uint16    VK_2          = 0x41 + '2' - 'A';

const int       X_CENTER      = SCREEN_WIDTH / 2;
const int       Y_CENTER      = SCREEN_HEIGHT / 2;
const float     X_SENS        = -.002;
const float     Y_SENS        = .002;

void     Draw     (HDC);
void     Reshape  (int,int,HDC);
void     Idle     ();
int      Keyb     (Window*,UINT,WORD,DWORD,DWORD);
void     Move     ();

void     DrawNet  ();
void     drawBox  (const boundbox_t&);
void     DrawNode (int,int,float,float,int,float);

void     InsertNode (int,int,int);
void     InsertNode (int,int);
void     ResetNodes ();

void     SetMaterial (DWORD,int);

float    GetAngle   (float,float);

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
SBuffer*      sbuffer;
int           mode = -1;
int           hf   = 0;
int           pgcnt = 0;

float           xangle        = 0;
float           yangle        = 0;
int             kpress [1024] = {0};

Camera            eye      (vector3d_t(0,3.5f,30),
                            vector3d_t(0,0,-1),     
                            vector3d_t(0,1,0)
                           ); 

int main (int,char* [])
{
  M_Load (M_GetPool());
  F_Load (M_GetPool());
  Application* app = A_GetApplication ();  

  FullScreenWindow* wnd = new (app->pool())
                      FullScreenWindow (app->pool(),app->instance (),rect_t (0,0,SCREEN_WIDTH,SCREEN_HEIGHT),"Test");
  wnd->ShowCursor (FALSE);
  OpenGLView    ogl  (wnd,NULL,Draw,Reshape);  

  GameLevel     level  (M_GetPool (),
                        "park.map",
                        LL_ALL
                       );
          
  sbuffer        = new (M_GetPool ()) SBuffer (M_GetPool (),MAX_VIS_NODES);
  lev            = &level;
  size           = lev->mLandMap->width * lev->mLandMap->height;
  step           = lev->mLandMap->step;
  mapWidth       = lev->mLandMap->width;
  mapHeight      = lev->mLandMap->height;
  box            = lev->mLandMap->bbox;
  eye.set (lev->mLandMap->bbox.max / 2.0f,eye.dir (), eye.up());

  lev->mLandMeshRender->SetEnvFn   (SetMaterial,NULL,NULL);

  mode = 0;

  wnd->InsertHandler (WM_KEYDOWN,Keyb);
  wnd->InsertHandler (WM_KEYUP,Keyb);
  wnd->InsertHandler (WM_MOUSEMOVE,Keyb);

  MainLoop (Idle);  

  delete wnd;

  return EXIT_SUCCESS;
}

void     Draw2D     (HDC hdc)
{
  glClear (GL_COLOR_BUFFER_BIT);

  DrawNet ();    

  float dirx = eye.dir ().x,
        diry = eye.dir ().z;  
  float _div = 1.0f/sqrt (dirx*dirx + diry*diry);
  dirx      *= _div;
  diry      *= _div;
  float sx   = eye.pos ().x / step - dirx,
        sy   = eye.pos ().z / step - diry;
//  float sx   = eye.pos ().x / step,
//        sy   = eye.pos ().z / step;
  float len  = 0;
  float dlen = 2.0f*sin (ANGLE);
  float dx1  = dirx*cos(ANGLE)-diry*sin(ANGLE),
        dy1  = dirx*sin(ANGLE)+diry*cos(ANGLE),
        dx2  = dirx*cos(-ANGLE)-diry*sin(-ANGLE),
        dy2  = dirx*sin(-ANGLE)+diry*cos(-ANGLE),
        x1   = sx,
        y1   = sy,
        x2   = sx,
        y2   = sy;

  int   depth= abs(DEPTH / (cos (ANGLE) * step)) + 2;
  
        //Флаг обработки тайла

//  float delta [4] = {dx1*step,dy1*step,dx2*step,dy2*step};
//  htree->set  (vector3d_t (sx * step,eye.pos().y,sy * step),delta);

  float length = DEPTH / cos (ANGLE);
  float _dx1  = dirx*cos(ANGLE)-diry*sin(ANGLE),
        _dy1  = dirx*sin(ANGLE)+diry*cos(ANGLE),
        _dx2  = dirx*cos(-ANGLE)-diry*sin(-ANGLE),
        _dy2  = dirx*sin(-ANGLE)+diry*cos(-ANGLE);

  float v [4] = {eye.pos().x+_dx1*length,eye.pos().z+_dy1*length,
                 eye.pos().x+_dx2*length,eye.pos().z+_dy2*length
                };

  sbuffer->set (vector3d_t (sx * step,eye.pos().y,sy*step),GetAngle(dirx,diry),v);

  for (int i=0;i<depth;i++,x1+=dx1,y1+=dy1,len+=dlen)  
  {
    float x     = x1,
          y     = y1;              
              
    for (float l=0;l<len;l+=1.0f,x+=diry,y-=dirx)        
    {
      if (x >= 0 && y >= 0 && x < mapWidth && y < mapHeight)
      {
        int index = int(y)*mapWidth+int(x);
        DrawNode (x,y,(lev->mLandMap->map [index].box.min.y)/box.max.y,
                  (lev->mLandMap->map [index].box.max.y)/box.max.y,                        
                  sbuffer->insert (index,i,lev->mLandMap->map [index].box),
                  float(i)/float(8)
                 );        
      }           
    }
  }

/*  float length = DEPTH / cos (ANGLE);
  float _dx1  = dirx*cos(ANGLE)-diry*sin(ANGLE),
        _dy1  = dirx*sin(ANGLE)+diry*cos(ANGLE),
        _dx2  = dirx*cos(-ANGLE)-diry*sin(-ANGLE),
        _dy2  = dirx*sin(-ANGLE)+diry*cos(-ANGLE);*/

  glColor3f  (0,1,0);

  glBegin (GL_LINE_LOOP);
    glVertex2f (eye.pos().x,eye.pos().z);
    glVertex2f (eye.pos().x+_dx1*length,eye.pos().z+_dy1*length);
    glVertex2f (eye.pos().x+_dx2*length,eye.pos().z+_dy2*length);
  glEnd   ();

  glFinish ();

  SwapBuffers (hdc);    
}

void     Draw3D     (HDC hdc)
{  
  ResetNodes ();

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix ();

  gluLookAt    (eye.pos().x,eye.pos().y,eye.pos().z,
                eye.dir().x+eye.pos().x,eye.dir().y+eye.pos().y,eye.dir().z+eye.pos().z,
                0,1,0
               );

  frustum_t   frust;
  vertex_t*   vertex = lev->mLandMap->vertex;
  vector3d_t* normal = lev->mLandMap->normal;
      
  CalcGLFrustum     (frust);

  float fldirx = eye.dir ().x,
        fldiry = eye.dir ().z;  
  float _div = 1.0f/sqrt (fldirx*fldirx + fldiry*fldiry);
  fldirx      *= _div;
  fldiry      *= _div;

  Fixed  dirx  = float2fixed (fldirx),
         diry  = float2fixed (fldiry);      

  Fixed sx   = float2fixed (eye.pos ().x / step) - dirx,
        sy   = float2fixed (eye.pos ().z / step) - diry;

  Fixed len  = 0;
  Fixed dlen = sin (FX_ANGLE)<<1;
  Fixed dx1  = mul (dirx,cos(FX_ANGLE)) - mul (diry,sin(FX_ANGLE)),
        dy1  = mul (dirx,sin(FX_ANGLE)) + mul (diry,cos(FX_ANGLE)),
        dx2  = mul (dirx,cos((Angle)-FX_ANGLE)) - mul (diry,sin((Angle)-FX_ANGLE)),
        dy2  = mul (dirx,sin((Angle)-FX_ANGLE)) + mul (diry,cos((Angle)-FX_ANGLE)),
        x1   = sx,
        y1   = sy;
  Fixed w    = int2fixed (mapWidth),
        h    = int2fixed (mapHeight);

  int   depth= abs(DEPTH / (cos (ANGLE) * step)) + 2; 

  if (sx >= 0 && sy >= 0 && sx < w && sy < h)
    InsertNode (fixed2int(sx),fixed2int(sy),0); 

/*  float delta [4] = {fixed2float(dx1)*step,
                     fixed2float(dy1)*step,
                     fixed2float(dx2)*step,
                     fixed2float(dy2)*step};

  htree->set  (eye.pos(),delta);*/

  float length = DEPTH / cos (ANGLE);
  float _dx1  = fldirx*cos(ANGLE)-fldiry*sin(ANGLE),
        _dy1  = fldirx*sin(ANGLE)+fldiry*cos(ANGLE),
        _dx2  = fldirx*cos(-ANGLE)-fldiry*sin(-ANGLE),
        _dy2  = fldirx*sin(-ANGLE)+fldiry*cos(-ANGLE);

  float v [4] = {eye.pos().x+_dx1*length,eye.pos().z+_dy1*length,
                 eye.pos().x+_dx2*length,eye.pos().z+_dy2*length
                };

//  sbuffer->set (eye.pos(),GetAngle(fldirx,fldiry),v);
  sbuffer->set (vector3d_t (eye.pos().x-fldirx*step,eye.pos().y,eye.pos().z-fldiry*step),
                GetAngle(fldirx,fldiry),v);

               //TILE flag

  int total = 0;

  for (int i=0;i<depth;i++,x1+=dx1,y1+=dy1,len+=dlen)
  {
    Fixed x     = x1,
          y     = y1;              
              
    for (Fixed l=0;l<len;l+=FX_ONE,x+=diry,y-=dirx)
    {
      if (x >= 0 && y >= 0 && x < w && y < h)
        nodes [i].data [nodes [i].count++] = fixed2int(y)*mapWidth+fixed2int(x); //TABLE
    }  

    for (int j=0;j<nodes [i].count;j++)
    {
      if (IsInBound (lev->mLandMap->map [nodes [i].data [j]].box,6,frust.clip))
      {
        if (sbuffer->insert (nodes [i].data [j],
                         i,
                         lev->mLandMap->map [nodes [i].data [j]].box))
              lev->mLandMeshRender->Draw (nodes [i].data [j],i);
        total++;
      }  
    }  
  }

  int        count  = 0;
  visnode_t* vis    = sbuffer->get (count);

//  dout<<count<<" "<<total<<endl;

//  lev->mLandMeshRender->Draw (vis,count);
  lev->mLandMeshRender->Flush();

  glPopMatrix ();

  glFinish ();

  SwapBuffers (hdc);   
}

void    Draw (HDC hdc)
{
  if (mode == -1) return;

  if (mode) Draw2D (hdc);
  else      Draw3D (hdc);
}

float    GetAngle   (float x,float y)
{
  if (x <= 0)   return acos (y);
  else          return -acos (y);
}

void     DrawNet  ()
{
  glColor3f   (0,0,1);
  glLineWidth (1);
  
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

void     DrawNode (int x,int y,float c,float c1,int c2,float c3)
{
  if (!hf)
  {
    if (c2) glColor3f (c,0,0);
    else    glColor3f (c1,0,0);
  }
  else
    glColor3f (0,0,c3);
    
  glLineWidth (3);

  glBegin (GL_QUADS);
    glVertex2f (float (x)*step+1,float(y)*step+1);
    glVertex2f (float (x+1)*step-1,float(y)*step+1);
    glVertex2f (float (x+1)*step-1,float(y+1)*step-1);
    glVertex2f (float (x)*step+1,float(y+1)*step-1);
  glEnd   ();

  if (!c2)
  {
    if (hf) glColor3f (0,0,0);
    else    glColor3f (0,1,0);
    glBegin (GL_LINE_LOOP);
      glVertex2f (float (x)*step+1,float(y)*step+1);
      glVertex2f (float (x+1)*step-1,float(y)*step+1);
      glVertex2f (float (x+1)*step-1,float(y+1)*step-1);
      glVertex2f (float (x)*step+1,float(y+1)*step-1);
    glEnd   ();
  }

  glLineWidth (1);
}

void     InsertNode (int x,int y,int level)
{
  nodes [level].data [nodes [level].count++] = y*mapWidth+x;
}

void     InsertNode (int pos,int level)
{
  nodes [level].data [nodes [level].count++] = pos;
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

  gluPerspective   (ANGLE*57.3f,(float)width/(float)height, 1.0 ,500.0f);

  glMatrixMode     (GL_MODELVIEW);
  glLoadIdentity   ();

  glEnable         (GL_DEPTH_TEST); 
//  glEnable         (GL_COLOR_MATERIAL);  
  glEnable         (GL_LIGHTING);
  glEnable         (GL_LIGHT0);

  glColorMaterial  (GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
//  glColorMaterial  (GL_FRONT_AND_BACK,GL_DIFFUSE);

  glClearColor     (0.1,0.2,0.3,1);
  glClear          (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable         (GL_FOG);

  GLfloat          col [4] = {0.8,0.8,0.8,1};

  glFogf           (GL_FOG_MODE,GL_LINEAR);
  glFogf           (GL_FOG_DENSITY,0.1);
  glFogf           (GL_FOG_START,DEPTH*0.1);
  glFogf           (GL_FOG_END,DEPTH*0.95);
  glFogfv          (GL_FOG_COLOR,col);

  light_t          light;

  light.diffuse    = vector3d_t (0.8);
  light.specular   = vector3d_t (0.5);
  light.ambient    = vector3d_t (0.8);
  light.shiness    = 4;
  light.angle      = 3.14f;
  light.pos        = vector3d_t (0);
  light.dir        = vector3d_t (0);

  SetLight (0,light);

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
  {
    kpress [key] = TRUE;
  }  
  else if (msg == WM_KEYUP && key < 1024)
  {
    kpress [key] = FALSE;  

    if (key == VK_2)
    {
      hf = hf == 0 ? 1 : 0;
    }

    if (key == VK_1)
    {
      mode = mode == 0 ? 1 : 0;      

      if (!mode)
      {  
        glMatrixMode     (GL_PROJECTION);
        glLoadIdentity   ();

        gluPerspective   (ANGLE*57.3f,(float)SCREEN_WIDTH/(float)SCREEN_HEIGHT, 1.0 ,500.0f);

        glMatrixMode     (GL_MODELVIEW);
        glLoadIdentity   ();

        glEnable         (GL_DEPTH_TEST); 
        glEnable         (GL_LIGHTING);
      }
      else
      {
        glMatrixMode     (GL_PROJECTION);
        glLoadIdentity   ();
        gluOrtho2D       (0,0,mapWidth*step,mapHeight*step);
        glTranslatef     (-0.5,0.5,0);
        glScalef         (1.0f/(mapWidth*step),-1.0f/(mapHeight*step),1);

        glMatrixMode     (GL_MODELVIEW);
        glLoadIdentity   ();

        glDisable        (GL_DEPTH_TEST); 
        glDisable        (GL_LIGHTING);

      }
    }
  }
 
  return 0;
}

void  Idle  ()
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
    dout<<"TPS: "<<float(pgcnt*CLK_TCK)/float(clock()-last)<<endl;

    last  = clock ();
    pgcnt = 0;
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

  vector3d_t pos = eye.pos ();

  if         (pos.x < box.min.x) pos.x = box.min.x;
  else if    (pos.x > box.max.x) pos.x = box.max.x;
  if         (pos.y < box.min.y) pos.y = box.min.y;
  else if    (pos.y > box.max.y) pos.y = box.max.y;
  if         (pos.z < box.min.z) pos.z = box.min.z;
  else if    (pos.z > box.max.z) pos.z = box.max.z;

  eye.set (pos,eye.dir(),eye.up());
}

void     SetMaterial (DWORD,int mat)
{
//  lev->mTextures->bind (mat);

  if (mat != -1)    
  {
    material_t& m = lev->mTextures->GetMaterial (mat);

//    glColorMaterial  (GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
//    glColor3fv       ((GLfloat*)&m.diffuse);
//    glColorMaterial  (GL_FRONT_AND_BACK,GL_AMBIENT);
//    glColor3fv       ((GLfloat*)&m.ambient);
//    glColorMaterial  (GL_FRONT_AND_BACK,GL_SPECULAR);
//    glColor3fv       ((GLfloat*)&m.specular);
    SetMaterial (m);
  }  
  else
  {
    glColorMaterial  (GL_FRONT_AND_BACK,GL_DIFFUSE);
    glColor3f  (0,1,0);
  }  
}