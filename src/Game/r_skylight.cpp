#include "pch.h"
#include "r_skylight.h"
#include "r_skybox.h"

#include <gl\gl.h>

static int   amb [24][4] = {
          {54, 83, 147, 1},
          {54, 83, 147, 1},
          {87, 89, 110, 1},
          {122, 81, 68, 1},
          {133, 96, 85, 1},
          {135, 117, 94, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {135, 117, 94, 1},
          {133, 96, 85, 1},
          {122, 81, 68, 1},
          {87, 89, 110, 1},
          {54, 83, 147, 1}
};

static int sunLight [24][4] = {
           {63, 93, 157, 1},
           {63, 93, 157, 1},
           {92, 122, 165, 1},
           {171, 131, 119, 1},
           {255, 140, 104, 1},
           {255, 198, 129, 1},
           {255, 248, 207, 1},
           {255, 255, 255, 1},
           {255, 255, 255, 1},
           {255, 255, 255, 1},
           {255, 255, 255, 1},
           {255, 255, 255, 1},
           {255, 255, 255, 1},
           {255, 255, 255, 1},
           {255, 255, 255, 1},
           {255, 255, 255, 1},
           {255, 255, 255, 1},
           {255, 255, 255, 1},
           {255, 248, 207, 1},
           {255, 202, 136, 1},
           {255, 123, 82, 1},
           {166, 128, 116, 1},
           {92, 122, 165, 1},
           {63, 93, 157, 1}
};           
    
static int sky [24][4] = {
      {0, 0, 118, 1},
      {0, 0, 118, 1},
      {76, 78, 97, 1},
      {151, 80, 58, 1},
      {196, 89, 56, 1},
      {175, 150, 126, 1},
      {138, 176, 177, 1},
      {53, 182, 198, 1},
      {46, 181, 217, 1},
      {46, 181, 217, 1},
      {46, 181, 217, 1},
      {46, 181, 217, 1},
      {46, 181, 217, 1},
      {46, 181, 217, 1},
      {46, 181, 217, 1},
      {46, 181, 217, 1},
      {46, 181, 217, 1},
      {103, 181, 183, 1},
      {174, 168, 157, 1},
      {210, 154, 101, 1},
      {210, 95, 36, 1},
      {136, 81, 81, 1},
      {76, 78, 97, 1},
      {0, 0, 118, 1}
};
  
void    SkyLight::refresh     (float hour)
{
  D_Try
  {
    if (hour >= 24.0f) hour = 24.0f;
    if (hour <  0.0f)  hour = 0.0f;

    vector3d_t  x, y, z;

    int     index  = hour;

    lerp (x,hour - float (index),mSunLight [index],mSunLight [(index+1)%24]);
    lerp (y,hour - float (index),mSkyColor [index],mSkyColor [(index+1)%24]);
    lerp (z,hour - float (index),mAmbient  [index],mAmbient  [(index+1)%24]);

    lightsrc_t light;

    if (!mLightEnv->GetLight (mLight,light))
      return;

    vector3d_t c          = (mBBox.max + mBBox.min) / 2.0f;
    float      angle      = mKoef * (hour - mMorning);

    light.shiness         = mSunShine;
    light.light.diffuse   = x;
    light.light.specular  = x;
    light.light.ambient   = z;
    light.light.pos       = mRot * (vector3d_t (cos (angle),sin (angle),0) * mSunDist + c);
    light.light.dir       = normalize (c - light.light.pos);

    mLightEnv->ChangeLight (mLight,light);

    y                    *= mSunShine;

    mSkyBox->SetSkyColor   (y);
    glClearColor           (y.x,y.y,y.z,1);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"SkyLight: Error at refresh "<<endl;
    #endif
  }
}
        
void    SkyLight::SetSkyColor (int hour,const color3d_t* col,int num)
{
  D_Try
  {
    if (hour < 0 || hour + num >= 24)
      return;

    memcpy (mSkyColor + hour,col,num * sizeof (color3d_t));
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"SkyLight: Error at SetSkyColor"<<endl;
    #endif
  }
}

void    SkyLight::SetSunLight (int hour,const color3d_t* col,int num)
{
  D_Try
  {
    if (hour < 0 || hour + num >= 24)
      return;

    memcpy (mSunLight + hour,col,num * sizeof (color3d_t));
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"SkyLight: Error at SetSunLight"<<endl;
    #endif
  }
}

void    SkyLight::SetAmbient  (int hour,const color3d_t* col,int num)
{
  D_Try
  {
    if (hour < 0 || hour + num >= 24)
      return;

    memcpy (mAmbient + hour,col,num * sizeof (color3d_t));
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"SkyLight: Error at SetAmbient"<<endl;
    #endif
  }
}

void    SkyLight::lerp (vector3d_t& c,float t,const vector3d_t& a,const vector3d_t& b)
{
  c = a * (1.0f-t) + b * t;
}

void    SkyLight::SetSunLimit (float hour1,float hour2,float angle)
{
  mMorning = hour1;
  mRot     = matrix_t::rotate (vector3d_t (1,0,0),angle);
  mKoef    = 3.14f / (hour2 - hour1);
}

SkyLight::SkyLight (Pool* _Pool,TileLightEnv* lenv,SkyBoxRender* sbox,const boundbox_t& bbox)
         : MemObject (_Pool),
           mSkyColor (NULL),
           mSunLight (NULL),
           mAmbient  (NULL),
           mLightEnv (lenv),
           mLight    (-1),
           mSkyBox   (sbox),
           mBBox     (bbox),
           mMorning  (6.0f),
           mKoef     (3.14f / 12.0f),
           mSunDist  (length (bbox.max - bbox.min) / 2.0f),
           mRot      (1),
           mSunShine (1)
{
  D_Try
  {
    mSkyColor = (color3d_t*)GetPool()->alloc (sizeof (color3d_t) * 24);
    mSunLight = (color3d_t*)GetPool()->alloc (sizeof (color3d_t) * 24);
    mAmbient  = (color3d_t*)GetPool()->alloc (sizeof (color3d_t) * 24);

    if (!mAmbient || !mSkyColor || !mSunLight)
    {
      #ifdef DEBUG
        dout<<"SkuLight: No enough memory "<<endl;
      #endif
      return;
    }

    for (int i=0;i<24;i++)
    {
      mSkyColor [i] = vector3d_t (sky [i][0],sky [i][1],sky [i][2]);
      mSunLight [i] = vector3d_t (sunLight [i][0],sunLight [i][1],sunLight [i][2]);
      mAmbient  [i] = vector3d_t (amb [i][0],amb [i][1],amb [i][2]);

      mSkyColor [i]/= 255.0f;
      mSunLight [i]/= 255.0f;
      mAmbient  [i]/= 255.0f;
    }  

    lightsrc_t src;

    src.shiness         = 1;
    src.light.angle     = 3.14 / 2.0f;
    src.light.farlight  = TRUE;
    src.light.pos       = (mBBox.max + mBBox.min) / 2.0f;

    mLight              = mLightEnv->NewLight (src,TLE_GLOBAL_LIGHT|TLE_ENABLE_LIGHT);

    refresh (12);
  }   
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"SkyLight: Error at construct "<<endl;
    #endif
  }
}

SkyLight::~SkyLight ()
{
  D_Try
  {
    if (mSunLight) GetPool()->free (mSunLight);
    if (mSkyColor) GetPool()->free (mSkyColor);
    if (mAmbient)  GetPool()->free (mAmbient);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"SkyLight: Error at destruct "<<endl;
    #endif
  }
}
/*
    vector3d_t v   = normalize (mLightDesc [i].light.light.pos-mCamera.pos ());
    float      ang = mCamera.dir () & v;

    if (ang >= 0)
    {
      glEnable       (GL_BLEND);
      glDisable      (GL_CULL_FACE);
      glEnable       (GL_COLOR_MATERIAL);
      glDepthMask    (GL_FALSE);
      glDisable      (GL_DEPTH_TEST);

      glPushMatrix   ();
      glLoadIdentity ();
      glMatrixMode   (GL_PROJECTION);
      glPushMatrix   ();
      glLoadIdentity ();
      glOrtho        (0,200,0,200,-1,1);
      glColor4f      (1,1,1,ang);
      glBegin        (GL_QUADS);
        glVertex2i   (0,0);
        glVertex2i   (200,0);
        glVertex2i   (200,200);
        glVertex2i   (0,200);
      glEnd          ();
      glPopMatrix    ();
      glMatrixMode   (GL_MODELVIEW);
      glPopMatrix    ();
      glDisable      (GL_BLEND);
      glEnable       (GL_CULL_FACE);
      glDisable      (GL_COLOR_MATERIAL);
      glDepthMask    (GL_TRUE);
      glEnable       (GL_DEPTH_TEST);
    }
*/