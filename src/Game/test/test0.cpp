#include <kernel\app.h>
#include <kernel\fswindow.h>
#include <kernel\smpwindow.h>
#include <graph\oglview.h>
#include <engine\utils.h>
#include <engine\camera.h>
#include <misc.h>
#include <gl\glu.h>

#include "g_level.h"
#include "r_htree.h"

const int       SCREEN_WIDTH  = 1024;
const int       SCREEN_HEIGHT = 768;

const float     TURN_ANGLE    = 3.14f / 72.0f;
const float     STEP          = 1.0f;
const float     ANGLE         = 45 / 57.3f;
const float     DEPTH         = 300.0f;
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

int      start    (Application*,char*);
void     Draw     (HDC);
void     Reshape  (int,int,HDC);
void     Idle     (Application*);
int      Keyb     (Window*,UINT,WORD,DWORD,DWORD);
void     Move     ();

void     DrawNet  ();
void     drawBox  (const boundbox_t&);
void     DrawNode (int,int,float,float,int,float);

void     InsertNode (int,int,int);
void     InsertNode (int,int);
void     ResetNodes ();

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
HeightTree*   htree;
int           mode = 0;
int           hf   = 0;

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
          
  htree          = new (&pool) HeightTree (&pool,MAX_VIS_NODES);
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

  float delta [4] = {dx1*step,dy1*step,dx2*step,dy2*step};
  htree->set  (vector3d_t (sx * step,eye.pos().y,sy * step),delta);

  for (int i=0;i<depth;i++,x1+=dx1,y1+=dy1,len+=dlen)  
  {
    float x     = x1,
          y     = y1;              
              
    for (float l=0;l<len;l+=1.0f,x+=diry,y-=dirx)        
    {
      if (x >= 0 && y >= 0 && x < mapWidth && y < mapHeight)
      {
        int index = int(y)*mapWidth+int(x);
        DrawNode (x,y,(lev->mGraphMap->map [index].box.min.y)/box.max.y,
                  (lev->mGraphMap->map [index].box.max.y)/box.max.y,                        
                  htree->insert (index,i,lev->mGraphMap->map [index].box),
                  float(i)/float(8)
                 );        
      }           
    }
  }

  htree->reset ();

  float length = DEPTH / cos (ANGLE);
  float _dx1  = dirx*cos(ANGLE)-diry*sin(ANGLE),
        _dy1  = dirx*sin(ANGLE)+diry*cos(ANGLE),
        _dx2  = dirx*cos(-ANGLE)-diry*sin(-ANGLE),
        _dy2  = dirx*sin(-ANGLE)+diry*cos(-ANGLE);

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
  reset ();        
  ResetNodes ();

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix ();

  gluLookAt    (eye.pos().x,eye.pos().y,eye.pos().z,
                eye.dir().x+eye.pos().x,eye.dir().y+eye.pos().y,eye.dir().z+eye.pos().z,
                0,1,0
               );

  frustum_t frust;
  vertex_t* vertex = lev->mGraphMap->vertex;
      
  CalcGLFrustum     (frust);

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

  if (sx >= 0 && sy >= 0 && sx < mapWidth && sy < mapHeight)
    InsertNode (sx,sy,0); 

  float delta [4] = {dx1*step,dy1*step,dx2*step,dy2*step};

  htree->set  (vector3d_t (sx * step,eye.pos().y,sy * step),delta);

               //TILE flag

  int total = 0;

  for (int i=0;i<depth;i++,x1+=dx1,y1+=dy1,len+=dlen)
  {
    float x     = x1,
          y     = y1;              
              
    for (float l=0;l<len;l+=1.0f,x+=diry,y-=dirx)
    {
      if (x >= 0 && y >= 0 && x < mapWidth && y < mapHeight)
        nodes [i].data [nodes [i].count++] = int (y)*mapWidth+int(x);
    }  

    for (int j=0;j<nodes [i].count;j++)
    {
      htree->insert (nodes [i].data [j],i,lev->mGraphMap->map [nodes [i].data [j]].box);
      total++;
    }  
  }

  int        count  = 0;
  visnode_t* vis    = htree->get (count);

//  dout<<count<<" "<<total<<endl;

  glBegin (GL_TRIANGLES);

  for (i=0;i<count;i++)
  {
    int inode = vis [i].index;

    if (IsInBound (lev->mGraphMap->map [inode].box,6,frust.clip))
    {
      grnode_t&     node   = lev->mGraphCache->index (inode);
      facetpack_t*  facet  = node.facet;    

      for (int k=0;k<node.count;facet++,k++)
      {
        if (!check (facet->index))  
        {
           if (facet->facet.texture != -1)
           glColor3fv                  ((GLfloat*)&lev->mTextEnv->GetMaterial (facet->facet.texture).diffuse);

           glNormal3fv   ((GLfloat*)&facet->facet.n);
      
           for (int j=0;j<3;j++)
           {                
             glVertex3fv   ((GLfloat*)(vertex + facet->facet.index [j]));
             glTexCoord2fv ((GLfloat*)&facet->facet.tindex [j]);
           }            

           set (facet->index);
        }  
      }
    }
  }

  glEnd ();

  glPopMatrix ();

  glFinish ();

  SwapBuffers (hdc);    
}

void    Draw (HDC hdc)
{
  if (mode) Draw2D (hdc);
  else      Draw3D (hdc);
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
  glEnable         (GL_COLOR_MATERIAL);  
  glEnable         (GL_LIGHTING);
  glEnable         (GL_LIGHT0);

  glColorMaterial  (GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);

  glClearColor     (1,1,1,1);
  glClear          (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable         (GL_FOG);

  GLfloat          col [4] = {0.8,0.8,0.8,1};

  glFogf           (GL_FOG_MODE,GL_LINEAR);
  glFogf           (GL_FOG_DENSITY,0.1);
  glFogf           (GL_FOG_START,DEPTH*0.1);
  glFogf           (GL_FOG_END,DEPTH*0.95);
  glFogfv          (GL_FOG_COLOR,col);

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

  vector3d_t pos = eye.pos ();

  if         (pos.x < box.min.x) pos.x = box.min.x;
  else if    (pos.x > box.max.x) pos.x = box.max.x;
  if         (pos.y < box.min.y) pos.y = box.min.y;
  else if    (pos.y > box.max.y) pos.y = box.max.y;
  if         (pos.z < box.min.z) pos.z = box.min.z;
  else if    (pos.z > box.max.z) pos.z = box.max.z;

  eye.set (pos,eye.dir(),eye.up());
}
