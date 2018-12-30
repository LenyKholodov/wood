#include "pch.h"
#include "r_main.h"
#include "g_loader.h"

#include <fixed\tables.h>
#include <stdarg.h>
#include <misc.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <graph\utils.h>

bool     MainRender::valid       () const
{
  return  mLoader.valid () && IsValid (mMap) && IsValid (mLevel) && IsValid (mSBuffer) &&
          IsValid (mMatEnvX) && IsValid (mLandRender) && IsValid (mFlag) && 
          IsValid (mLightEnv) && IsValid (mModelEnv) && // IsValid (mTreeEnv) &&
          IsValid (mShadowEnv) && IsValid (mSkyBox) && IsValid (mAlphaRender) &&
          IsValid (&mPredraw);
}

uint     MainRender::GetPgCount  ()
{
  return mLandRender->GetTPS () + 
         mModelEnv->GetPgCount () + 
         mShadowEnv->GetPgCount () +
         mSkyBox->GetPgCount () +
//         mTreeEnv->GetPgCount () + 
         mHMLandRender->GetTPS ();
}

void     MainRender::ChangeState (uint flag,BOOL state)
{
  mEnable &= (0xFFFFFFFF ^ flag);
  mEnable |= state ? flag : 0;

  if (flag == RENDER_CLEAR_DEPTH_BUFFER_BIT) mChangeDepthState = TRUE;
}

void     MainRender::BindConsole   (ConsoleBase& base,int font,int w,int h)
{
  D_Try
  {
    if (!mConsole)  
    {
      mConsole = ext_new OGLConsole (GetPool(),base,font,w,h);

      if (mConsole)
        base.bind  (mConsole);
    }
  }
  D_Exc ("MainRender: Error at BindConsole");
}

float    MainRender::GetAngle   (float x,float y)
{
  if (x <= 0)   return acos (y);
  else          return -acos (y);
}

void     MainRender::DrawShadows   ()
{
  D_Try
  {
    if (mEnable & RENDER_SHADOWS_BIT)
    {
      int         count;
      visnode_t*  vis   = mSBuffer->get (count);

      mShadowEnv->Draw (count,vis);
    }  
  }
  D_Exc ("MainRender: Error at DrawShadows");
}

void     MainRender::Draw        (const Camera& camera)
{
  D_Try
  {
//    glPushMatrix        ();

    glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); //!!

    Predraw             ();

    glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); //!!

    Camera eye    = camera;

    glMatrixMode        (GL_MODELVIEW);
    glLoadIdentity      ();
    gluLookAt           (eye.pos.x,eye.pos.y,eye.pos.z,
                         eye.dir.x+eye.pos.x,eye.dir.y+eye.pos.y,eye.dir.z+eye.pos.z,
                         eye.up.x,eye.up.y,eye.up.z
                        );

    CalcGLFrustum       (mFrustum);
    ResetSBuffer        (eye);
    
    mLightEnv->Refresh      ();
    mAlphaRender->SetCamera (eye);
//    mTreeEnv->SetCamera     (eye);
//    mTreeEnv->SetFrustum    (mFrustum);
//    mTreeEnv->SetSBuffer    (mSBuffer);
    mLightEnv->SetCamera    (eye);
    mLightEnv->SetFrustum   (mFrustum);

    mModelEnv->SetCamera  (eye);
    mModelEnv->SetFrustum (mFrustum);
    mModelEnv->SetSBuffer (mSBuffer);
    mHMLandRender->SetCamera  (eye);
    mHMLandRender->SetFrustum (mFrustum);
    mHMLandRender->SetSBuffer (mSBuffer);

    DrawSky             (eye);

    mHMLandRender->Draw ();

    DrawLandscape       (eye);

    mModelEnv->render   ();
    mModelEnv->reset    ();
    
//    mTreeEnv->render    ();
    mLightEnv->Draw     ();
    mAlphaRender->Draw  ();

    DrawShadows         ();

    if (mConsole) mConsole->Draw ();

//    glPopMatrix         ();

    mFrame++;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
//    #ifdef DEBUG
      dout<<"MainRender: Error at Draw "<<endl;
//      Debug::Print (dout);
//    #endif
  }
}

void     MainRender::ResetSBuffer  (const Camera& eye)
{
  float    length   = RENDER_VIEW_DEPTH;
  float    fl_angle = RENDER_VIEW_ANGLE;
  float    step     = mMap->step;
  float    fldirx   = eye.dir.x,
           fldiry   = eye.dir.z;  
  float    _div     = 1.0f/sqrt (fldirx*fldirx + fldiry*fldiry);

  fldirx           *= _div;
  fldiry           *= _div;

  float    _dx1     = fldirx*cos(fl_angle)-fldiry*sin(fl_angle),
           _dy1     = fldirx*sin(fl_angle)+fldiry*cos(fl_angle),
           _dx2     = fldirx*cos(-fl_angle)-fldiry*sin(-fl_angle),
           _dy2     = fldirx*sin(-fl_angle)+fldiry*cos(-fl_angle);

  float v [4] = {eye.pos.x+_dx1*length,eye.pos.z+_dy1*length,
                 eye.pos.x+_dx2*length,eye.pos.z+_dy2*length
                };

  mSBuffer->set (vector3d_t (eye.pos.x-fldirx*step,eye.pos.y,eye.pos.z-fldiry*step),
                GetAngle(fldirx,fldiry),v);
}

void     MainRender::DrawLandscape (const Camera& eye)
{
  float    step     = mMap->step;
  int      mapWidth = mMap->width, 
           mapHeight= mMap->height;
  float    fldirx   = eye.dir.x,
           fldiry   = eye.dir.z;  
  float    _div     = 1.0f/sqrt (fldirx*fldirx + fldiry*fldiry);

  fldirx           *= _div;
  fldiry           *= _div;

  Fixed    dirx     = float2fixed (fldirx),
           diry     = float2fixed (fldiry);      

  Fixed    sx       = float2fixed (eye.pos.x / step) - dirx,
           sy       = float2fixed (eye.pos.z / step) - diry;
  float    fl_angle = RENDER_VIEW_ANGLE;
  Angle    fx_angle = float2angle (fl_angle);

  Fixed    len      = 0;
  Fixed    dlen     = sin (fx_angle) << 1;
  Fixed    dx1      = mul (dirx,cos(fx_angle)) - mul (diry,sin(fx_angle)),
           dy1      = mul (dirx,sin(fx_angle)) + mul (diry,cos(fx_angle)),
           x1       = sx,
           y1       = sy;
  Fixed    w        = int2fixed (mapWidth),
           h        = int2fixed (mapHeight);

  int      depth    = fabs(RENDER_VIEW_DEPTH * cos (fl_angle) / step) + 1; 

/*  float    length   = RENDER_VIEW_DEPTH;
  float    _dx1     = fldirx*cos(fl_angle)-fldiry*sin(fl_angle),
           _dy1     = fldirx*sin(fl_angle)+fldiry*cos(fl_angle),
           _dx2     = fldirx*cos(-fl_angle)-fldiry*sin(-fl_angle),
           _dy2     = fldirx*sin(-fl_angle)+fldiry*cos(-fl_angle);

  float v [4] = {eye.pos.x+_dx1*length,eye.pos.z+_dy1*length,
                 eye.pos.x+_dx2*length,eye.pos.z+_dy2*length
                };

  mSBuffer->set (vector3d_t (eye.pos.x-fldirx*step,eye.pos.y,eye.pos.z-fldiry*step),
                GetAngle(fldirx,fldiry),v);*/

  if (sx >= 0 && sy >= 0 && sx < w && sy < h)
  {
    int    index        = mMap->index (fixed2int (sx),fixed2int (sy));
    const  boundbox_t& box   = mMap->tile (index).box;

//    mTreeEnv->insert   (fixed2int (sx),fixed2int (sy),0); //index for change env
    mModelEnv->insert (fixed2int (sx),fixed2int (sy),index,0);

    if (IsInBound (box,6,mFrustum.clip))
    {
      if (mSBuffer->insert (fixed2int(sx),fixed2int(sy),index,0,box))
         mLandRender->AddTile (fixed2int (sx),fixed2int (sy),index,0);
    }  

    mFlag->set (index);
  }

  for (int i=0;i<depth;i++,x1+=dx1,y1+=dy1,len+=dlen)
  {
    Fixed x     = x1,
          y     = y1;              
    BOOL  flag  = FALSE;

    if (abs (len) < FX_ONE)
      continue;
              
    for (Fixed l=0;l<len;l+=FX_ONE,x+=diry,y-=dirx)
    {
      if (x >= 0 && y >= 0 && x < w && y < h)
      {
        int index         = mMap->index (fixed2int (x),fixed2int (y));
        const  boundbox_t& box = mMap->tile (index).box;
        flag              = TRUE;        

        if (!mFlag->check (index))
        {          
//          mTreeEnv->insert   (fixed2int (x),fixed2int (y),i); //index for change env
          mModelEnv->insert (fixed2int (x),fixed2int (y),index,i);

          if (IsInBound (box,6,mFrustum.clip))
          {
            if (mSBuffer->insert (fixed2int(x),fixed2int(y),index,i,box))
               mLandRender->AddTile (fixed2int (x),fixed2int (y),index,i);
          }                      
 
          mFlag->set  (index);
        }  
      }
    }  

    if (!flag)
      break;
  }

  mLandRender->Draw  ();
  mFlag->reset       ();
}

void     MainRender::AddPredraw  (const PredrawWait* predraw)
{ 
  mPredraw.insert ((PredrawWait*)predraw);
}

void     MainRender::DelPredraw  (const PredrawWait* predraw)
{                                                   
  mPredraw.erase  ((PredrawWait*)predraw);
}

void     MainRender::Predraw     ()
{  
  RingTmpl<PredrawWait*>::iterator iter  = mPredraw;
  int                              count = mPredraw.count ();

  for (int i=0;i<count;i++,iter++)
    iter->predraw ();     
}

void     MainRender::DrawSky     (const Camera& eye)
{
  if (mChangeDepthState)
  {
    if (mEnable & RENDER_CLEAR_DEPTH_BUFFER_BIT)
    {
      glDepthRange  (0,1);
      glDepthFunc   (GL_LESS);
      glClearDepth  (1);
    }
    else
    {
      if (mFrame & 1) glClearDepth (0);
      else            glClearDepth (1);

      glClear       (GL_DEPTH_BUFFER_BIT);
    }  

    mChangeDepthState = FALSE;  
  }

  if (GetState (RENDER_CLEAR_COLOR_BUFFER_BIT) && GetState (RENDER_CLEAR_DEPTH_BUFFER_BIT)) 
    glClear       (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  else
  {
    if (GetState (RENDER_CLEAR_COLOR_BUFFER_BIT))   glClear (GL_COLOR_BUFFER_BIT);
    if (GetState (RENDER_CLEAR_DEPTH_BUFFER_BIT))   glClear (GL_DEPTH_BUFFER_BIT);  
    else
    {
      if (mFrame & 1)
      {
        glDepthRange  (1.0,0.5);
        glDepthFunc   (GL_GREATER);
      }
      else
      {
        glDepthRange  (0.0,0.499999);
        glDepthFunc   (GL_LESS);
      }
    }
  }

  if (mEnable & RENDER_SKY_BIT) mSkyBox->Draw (eye);
}

void     MainRender::Draw2DLand  (const Camera& eye)
{
  glPushAttrib    (GL_ALL_ATTRIB_BITS);

  glClearColor    (0.2,0.2,0.2,0);
  DrawSky         (eye);

  float    step     = mMap->step;
  int      mapWidth = mMap->width, 
           mapHeight= mMap->height;

  glMatrixMode     (GL_PROJECTION);
  glPushMatrix     ();
  glLoadIdentity   ();
  gluOrtho2D       (0,0,mapWidth*step,mapHeight*step);
  glTranslatef     (-0.5,0.5,0);
  glScalef         (1.0f/(mapWidth*step),-1.0f/(mapHeight*step),1);

  glMatrixMode     (GL_MODELVIEW);
  glLoadIdentity   ();

  glDisable        (GL_DEPTH_TEST); 
  glDisable        (GL_LIGHTING);

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

  glColor3f (0,1,0);

  glBegin (GL_LINE_LOOP);
    glVertex2f (0,0);
    glVertex2f (step,0);
    glVertex2f (step,step);
    glVertex2f (0,step);
  glEnd   ();

  float    fldirx   = eye.dir.x,
           fldiry   = eye.dir.z;  
  float    _div     = 1.0f/sqrt (fldirx*fldirx + fldiry*fldiry);

  fldirx           *= _div;
  fldiry           *= _div;

  Fixed    dirx     = float2fixed (fldirx),
           diry     = float2fixed (fldiry);      

  Fixed    sx       = float2fixed (eye.pos.x / step) - dirx,
           sy       = float2fixed (eye.pos.z / step) - diry;
  float    fl_angle = RENDER_VIEW_ANGLE;
  Angle    fx_angle = float2angle (fl_angle);

  Fixed    len      = 0;
  Fixed    dlen     = sin (fx_angle) << 1;
  Fixed    dx1      = mul (dirx,cos(fx_angle)) - mul (diry,sin(fx_angle)),
           dy1      = mul (dirx,sin(fx_angle)) + mul (diry,cos(fx_angle)),
           dx2      = mul (dirx,cos((Angle)-fx_angle)) - mul (diry,sin((Angle)-fx_angle)),
           dy2      = mul (dirx,sin((Angle)-fx_angle)) + mul (diry,cos((Angle)-fx_angle)),
           x1       = sx,
           y1       = sy,
           x2       = sx,
           y2       = sy;
  Fixed    w        = int2fixed (mapWidth),
           h        = int2fixed (mapHeight);

  int      depth    = abs(RENDER_VIEW_DEPTH / (cos (fl_angle) * step)) + 2; 

  for (int i=0;i<depth;i++,x1+=dx1,y1+=dy1,x2+=dx2,y2+=dy2,len+=dlen)
  {
    Fixed x     = x1,
          y     = y1;
    Fixed dx    = diry,
          dy    = -dirx;
    BOOL  flag  = FALSE;

    if (abs (len) < FX_ONE)
      continue;
              
    for (Fixed l=0;l<len;l+=FX_ONE,x+=dx,y+=dy)
    {
      if (x >= 0 && y >= 0 && x < w && y < h)        
      {
        flag = TRUE;
        glColor3f (1,0,0);
        glBegin (GL_LINE_LOOP);
          glVertex2f ((float)(fixed2int (x))*step,(float)(fixed2int(y))*step);
          glVertex2f ((float)(fixed2int (x+FX_ONE))*step,(float)(fixed2int(y))*step);
          glVertex2f ((float)(fixed2int (x+FX_ONE))*step,(float)(fixed2int(y+FX_ONE))*step);
          glVertex2f ((float)(fixed2int (x))*step,(float)(fixed2int(y+FX_ONE))*step);
        glEnd   ();        
      }
    }  

    if (!flag)
      break;
  }

  glPopAttrib      ();
  glMatrixMode     (GL_PROJECTION);
  glPopMatrix      ();
}


static float        mat [] = {0,0,0,1,1,1,0,0,0,0,0,0,0,0};
static material_t   ___DefaultMRMaterial= *(material_t*)mat;

BOOL     MainRender::SetMaterial (int mat,int mode)
{
  if (mode & RENDER_MAT_SET)    mMatEnvX->SetMat (mat); 
  if (mode & RENDER_TEXT_SET)   return mMatEnvX->bind   (mat);
  else                          mMatEnvX->unbind ();

  return TRUE;
}

BOOL     MainRender::ChangeEnv   (uint index,uint _InterruptDrawing)
{
  return mLightEnv->SetLightEnv (index,_InterruptDrawing);
} 

MainRender::MainRender  (Pool* _Pool,LevelLoader& loader)
           : MemObject    (_Pool),
             mPredraw     (_Pool),
             mLoader      (loader),
             mLevel       (&loader.level),
             mEnable      (RENDER_DEFAULT_FLAG),
             mFrame       (0),
             mChangeDepthState (FALSE),
             mMap         (loader->mLandMap),
             mSBuffer     (NULL),
             mLandRender  (NULL),
             mFlag        (NULL),
             mLightEnv    (NULL),
             mShadowEnv   (NULL),
             mModelEnv    (NULL),
             mSkyBox      (NULL),
//             mTreeEnv     (NULL),
             mConsole     (NULL),
             mAlphaRender (NULL),
             mMatEnvX     (NULL),
             mHMLandRender(NULL)
{
  D_Try
  {
           //init render system         

    if (!IsValid (mMap))
    {
      #ifdef DEBUG
        dout<<"MainRender: map = NULL "<<endl;
      #endif
      return;
    }

    mMatEnvX     = new (GetPool()) MatEnvX         (GetPool(),3*1024*1024,"materials",128);

    if (!IsValid (mMatEnvX))
    {
      D_Message ("MainRender: matenvx = NULL");
      return;
    }

    float depth = sqrt (sqr (mMap->width) + sqr (mMap->height));

    if (RENDER_VIEW_DEPTH/mMap->step < depth) 
      depth = RENDER_VIEW_DEPTH/mMap->step;

    uint visnum     = (depth * depth) * tan (RENDER_VIEW_ANGLE);
    mSBuffer        = ext_new SBuffer (GetPool (),visnum);

    mLandRender     = ext_new LandMeshRender (GetPool(),
                                          this,      
                                          RENDER_MESH_NUM
                                         ); 

    if (!mSBuffer || !mLandRender)
    {
      #ifdef DEBUG
        dout<<"MainRender: No enough memory "<<endl;
      #endif
      return;      
    }

    mFlag      = ext_new FlagBit (GetPool(),mMap->width*mMap->height);
    mLightEnv  = ext_new TileLightEnv (GetPool(),mMap,64); //!!!!
    mShadowEnv = ext_new ShadowEnv (GetPool(),this,loader,RENDER_SHADOW_NUM); //!!!!
    mSkyBox    = ext_new SkyBoxRender (GetPool(),RENDER_VIEW_DEPTH*0.85,&loader.level,mMatEnvX);
//    mTreeEnv   = new (GetPool()) TreeEnv      (GetPool(),this,
//                                            RENDER_TREES_MESH_CACHE_SIZE,
//                                            RENDER_TREES_VERTEX_CACHE_SIZE
//                                           );
    mModelEnv  = ext_new ModelEnv     (GetPool(),mMap,
                                            RENDER_MODEL_DATA_NUM,
                                            RENDER_MODEL_NUM,
                                            RENDER_VIS_MODEL_NUM,
                                            RENDER_MODEL_DATA_CACHE_SIZE,
                                            RENDER_MODEL_POOL_SIZE
                                           );
    mAlphaRender = ext_new AlphaMeshRender (GetPool(),RENDER_ALPHA_MESH_NUM);
    mHMLandRender = ext_new HMapRender (GetPool(),this);

    if (!IsValid (mFlag) || !IsValid (mLightEnv) || !IsValid (mShadowEnv) || 
        !IsValid (mSkyBox) || !IsValid (mModelEnv) || //!IsValid (mTreeEnv) ||
        !IsValid (mAlphaRender) || !IsValid (mHMLandRender)
       )
    {
      D_Message ("MainRender: No enough memory");
      return;
    }

//    mTreeEnv->bind  (this);
    mModelEnv->bind (this);
    mLightEnv->bind (this);
  }
  D_Exc ("MainRender: Error at construct");
}

MainRender::~MainRender ()
{
  D_Try
  {
    if (mSBuffer)                delete mSBuffer;
    if (mLandRender)             delete mLandRender;
    if (mFlag)                   delete mFlag;
    if (mLightEnv)               delete mLightEnv;
    if (mShadowEnv)              delete mShadowEnv;
    if (mSkyBox)                 delete mSkyBox;
//    if (mTreeEnv)                delete mTreeEnv;
    if (mModelEnv)               delete mModelEnv;
    if (mConsole)                delete mConsole;
    if (mAlphaRender)            delete mAlphaRender;
    if (mMatEnvX)                delete mMatEnvX;
    if (mHMLandRender)           delete mHMLandRender;
  }
  D_Exc ("MainRender: Error at destruct");
}
