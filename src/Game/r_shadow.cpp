#include "pch.h"
#include "r_shadow.h"
#include "g_lndcache.h"
#include "g_loader.h"
#include "r_main.h"
#include "r_vbuffer.h"

#include <gl\gl.h>
#include <misc.h>
#include <ogl.h>

SHADOW      ShadowEnv::CreateShadow    (LIGHTSRC light,MODEL model)
{
  D_Try
  {
    if (light == -1 || model == -1)
      return -1;

    shadow_t* shadow = mShadowPool.alloc ();

    if (!shadow)
      return -1;

    shadow->light        = light;
    shadow->model [0]    = model;
    shadow->transform    = matrix_t (1);
    shadow->enabled      = FALSE;
    shadow->next         = NULL;
    shadow->prev         = NULL;
    shadow->mCount       = model != -1 ? 1 : 0;
    shadow->clean        = 0;

    return  mShadowPool.index (shadow);
  }
  D_ExcRet ("ShadowEnv: Error at CreateShadow",-1);
}

BOOL        ShadowEnv::InsertModel        (SHADOW si,MODEL model)
{
  D_Try
  {
    if (si < 0 || si >= mShadowPool.num ())
      return FALSE;

    shadow_t* shadow = mShadowPool.data (si);

    shadow->model [shadow->mCount++] = model;

    return TRUE;
  }
  D_ExcRet ("ShadowEnv: Error at InsertModel",FALSE);
}

void        ShadowEnv::ExtractModel        (SHADOW si,MODEL model)
{
  D_Try
  {
    if (si < 0 || si >= mShadowPool.num ())
      return ;

    shadow_t* shadow = mShadowPool.data (si);

    for (int i=0;i<SHADOW_MAX_MODELS;i++)
      if (shadow->model [i] == model && i < SHADOW_MAX_MODELS - 1)
      {
        memmove (shadow->model + i,shadow->model + i + 1,sizeof (MODEL) * (SHADOW_MAX_MODELS-i-1));
        shadow->mCount--;
      }  
  }
  D_Exc ("ShadowEnv: Error at ExtractModel");
}

void        ShadowEnv::DeleteShadow    (SHADOW si)
{
  D_Try
  {
    if (si < 0 || si >= mShadowPool.num ())
      return;

    shadow_t* shadow = mShadowPool.data (si);

    DisableShadow    (si);
    mShadowPool.free (shadow);
  }
  D_Exc ("ShadowEnv: Error at DeleteShadow");
}

void        ShadowEnv::EnableShadow  (SHADOW si)
{
  D_Try
  {
    if (si < 0 || si >= mShadowPool.num ())
      return;

    shadow_t* shadow = mShadowPool.data (si);

    shadow->prev     = NULL;
    shadow->next     = mFirst;
    shadow->enabled  = TRUE;

    if (mFirst) mFirst->prev     = shadow;

    mFirst           = shadow;
  }
  D_Exc ("ShadowEnv: Error at EnableShadow: "<<si);
}

void        ShadowEnv::DisableShadow  (SHADOW si)
{
  D_Try
  {
    if (si < 0 || si >= mShadowPool.num ())
      return;

    shadow_t* shadow = mShadowPool.data (si); 

    if (shadow->enabled)
    {
      if (shadow->prev) shadow->prev->next = shadow->next;
      else              mFirst             = shadow->next;

      shadow->prev    = shadow->next = NULL;
      shadow->enabled = FALSE;
    }  
  }
  D_Exc ("ShadowEnv: Error at DisableShadow: "<<si);
}

void        ShadowEnv::ShadowClean     (SHADOW si,int num)
{
  D_Try
  {
    if (si < 0 || si >= mShadowPool.num ())
      return;

    shadow_t* shadow = mShadowPool.data (si); 

    shadow->clean    = num;
  }
  D_Exc ("ShadowEnv: Error at ShadowClean: "<<si);
}

void        ShadowEnv::ClearAll    ()
{
  for (shadow_t* shadow=mFirst;shadow;shadow=shadow->next)
  {
    if (shadow->prev) shadow->prev->next = shadow->next;
    else              mFirst             = shadow->next;

    shadow->prev    = shadow->next = NULL;
    shadow->enabled = FALSE;
  }
}

void        ShadowEnv::Draw            (int num,visnode_t* vis)
{
  glPushAttrib (GL_ALL_ATTRIB_BITS);  

  D_Try
  {
    glEnable     (GL_CLIP_PLANE0);
    glEnable     (GL_CLIP_PLANE1);
    glEnable     (GL_CLIP_PLANE2);
    glEnable     (GL_CLIP_PLANE3);
    glEnable     (GL_CLIP_PLANE4);
    glEnable     (GL_CLIP_PLANE5);

    glDisable    (GL_LIGHTING);
    glEnable     (GL_TEXTURE_2D);
    glEnable     (GL_BLEND);
    glEnable     (GL_POLYGON_OFFSET_FILL);

    glEnable     (GL_TEXTURE_GEN_S);
    glEnable     (GL_TEXTURE_GEN_T);                                                        
    glEnable     (GL_TEXTURE_GEN_Q);                                                        

    glTexGenf    (GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenf    (GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenf    (GL_Q, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);  

//    glBlendFunc  (GL_SRC_COLOR,GL_ONE_MINUS_SRC_COLOR);
    glBlendFunc  (GL_ZERO,GL_ONE_MINUS_SRC_COLOR);

          //ВВести альфа-тест для корректного отображения нескольких теней!

    float           frsign = mRender->GetState (RENDER_CLEAR_DEPTH_BUFFER_BIT) ?
                             -1 :
                             (mRender->GetFrame () & 1) ? 1 : -1;
    int         si  = 0;
      
    for (shadow_t* shadow=mFirst;shadow;shadow=shadow->next,si++)
    {
      for (int k=0;k<6;k++)
      {
        plane_t& src      = shadow->frustum.clip [k];
        GLdouble plane [4] = {src.a,src.b,src.c,src.d};
        glClipPlane (GL_CLIP_PLANE0+k,plane);
      }
      
      const matrix_t& m    = shadow->transform;

      glTexGenfv      (GL_S, GL_OBJECT_PLANE, m[0]);
      glTexGenfv      (GL_T, GL_OBJECT_PLANE, m[1]);
      glTexGenfv      (GL_Q, GL_OBJECT_PLANE, m[3]);

      glPolygonOffset (1,SHADOW_GROUND_DIST*(si+1)*frsign);
      glBindTexture   (GL_TEXTURE_2D,shadow->texture);
//      glTexEnvi       (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE); //!!!

      mRender->GetLandRender()->DrawShadow (shadow->frustum,vis,num);
    }
  }
  D_Exc ("ShadowEnv: Error at Draw");

  glBindTexture   (GL_TEXTURE_2D,0);

  glPopAttrib ();
}

void        ShadowEnv::CalcBound         (MODEL* mdl,int cnt,boundsphere_t& sphere)
{
  if (cnt > 1)
  {
    vector3d_t      c (.0f);
    ModelEnv*       me = mRender->GetModelEnv ();
    boundsphere_t   b [SHADOW_MAX_MODELS];
    float           r = 0;

    for (int i=0;i<cnt;i++)
    {
      b [i]  = me->GetBound (mdl [i]).sphere;
      c     += b [i].center;
    }  
    
    c /= float (cnt);  

    for (i=0;i<cnt;i++)
    {
      float dist = length (c-b [i].center) + b [i].r;

      if (dist > r) r = dist;
    }

    sphere.center = c;
    sphere.r      = r;
  }
  else
    sphere = mRender->GetModelEnv ()->GetBound (mdl [0]).sphere;
}

void        ShadowEnv::RenderShadow    (shadow_t& shadow,int texture)
{
  lightsrc_t     light;    
  matrix_t&      m    = shadow.transform; m.identity ();
  frustum_t&     f    = shadow.frustum;
  boundsphere_t  b;

  CalcBound (shadow.model,shadow.mCount,b);

  if (!mRender->GetLightEnv ()->GetLight (shadow.light,light))
    return;

 if (length (light.light.pos - b.center) < b.r)
   return;

  vector3d_t& pos   = light.light.pos;
  vector3d_t  dir   = normalize (b.center-pos);
  float       angle = ::angle (dir,vector3d_t (0,0,-1));
  vector3d_t  rot   = normalize (dir ^ vector3d_t (0,0,-1));

  m                = matrix_t::rotate    (rot,angle);
  m               *= matrix_t::translate (-pos);  

  vector3d_t c     = m * b.center;
  float      r     = b.r * 1.1f;

  m.transpose ();

  glMatrixMode   (GL_PROJECTION);
  glPushMatrix   ();
  glLoadIdentity ();

  if (light.light.farlight)
  {
    float      len   = r;
    float      zmin  = fabs (c.z) - r,
               zmax  = SHADOW_MAX_DEPTH;

    glOrtho        (-len,len,-len,len,zmin,zmax);
  }  
  else  
  {
    float      len   = r * (1.0f - r/fabs (c.z));
    float      zmin  = fabs (c.z) - r,
               zmax  = SHADOW_MAX_DEPTH;

    glFrustum      (-len,len,-len,len,zmin,zmax);
  }  

  glMultMatrixf  ((GLfloat*)&m);                

  glGetFloatv    (GL_PROJECTION_MATRIX,(GLfloat*)&m);

  m.transpose    ();

  glViewport     (0,0,SHADOW_TEXTURE_WIDTH,SHADOW_TEXTURE_WIDTH);

  glDepthMask    (GL_FALSE); 
  glDisable      (GL_DEPTH_TEST);

  glMatrixMode   (GL_MODELVIEW);
  glLoadIdentity ();

  CalcFrustum    (m,f);

  glDisable        (GL_LIGHTING);
  glDisable        (GL_TEXTURE_2D);

  glClearColor     (0,0,0,0); 
  glClear          (GL_COLOR_BUFFER_BIT);
  glColor3f        (0.4,0.4,0.4); 

  for (int i=0;i<shadow.mCount;i++)
    mRender->GetModelEnv()->RenderShadow (shadow.model [i],light);

  glBindTexture    (GL_TEXTURE_2D,texture);
  glCopyTexImage2D (GL_TEXTURE_2D,
                    0,
                    GL_RGBA, 
                    0,
                    0,
                    SHADOW_TEXTURE_WIDTH,
                    SHADOW_TEXTURE_WIDTH,
                    0
                   );
  glTexParameteri  (GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri  (GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  m  = matrix_t::translate (vector3d_t (0.5)) * 
       matrix_t::scale (vector3d_t (0.25)) *
       m;

//  if (shadow.clean)
//    CleanShadow   (shadow,shadow.clean);
  CleanShadow   (shadow,2);

  glMatrixMode  (GL_PROJECTION);
  glPopMatrix   ();
  glMatrixMode  (GL_MODELVIEW);
}

void    ShadowEnv::CleanShadow       (shadow_t& shadow,int num)
{
  glDisable      (GL_CULL_FACE); //??
  glDepthMask    (GL_FALSE);
  glDisable      (GL_DEPTH_TEST);
  glEnable       (GL_TEXTURE_2D);

  glMatrixMode   (GL_PROJECTION);
  glLoadIdentity ();
  glOrtho        (0,200,0,200,-1,1);
  glMatrixMode   (GL_MODELVIEW);
  glLoadIdentity ();

//  glTexEnvi        (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

  for (int i=0;i<num;i++)
  {
    glBindTexture    (GL_TEXTURE_2D,shadow.texture);
    glClear          (GL_COLOR_BUFFER_BIT);
    glViewport       (0,0,SHADOW_TEXTURE_WIDTH>>1,SHADOW_TEXTURE_WIDTH>>1);

    glBegin        (GL_QUADS);
      glTexCoord2f (0,0);    glVertex2i   (0,0);
      glTexCoord2f (1,0);    glVertex2i   (200,0);
      glTexCoord2f (1,1);    glVertex2i   (200,200);
      glTexCoord2f (0,1);    glVertex2i   (0,200);
    glEnd          ();

    glBindTexture    (GL_TEXTURE_2D,mTexture [mShadowNum+1]);
    glCopyTexImage2D (GL_TEXTURE_2D,
                      0,
                      GL_RGBA, 
                      0,
                      0,
                      SHADOW_TEXTURE_WIDTH>>1,
                      SHADOW_TEXTURE_WIDTH>>1,
                      0
                     );
    glTexParameteri  (GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri  (GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glViewport       (0,0,SHADOW_TEXTURE_WIDTH,SHADOW_TEXTURE_WIDTH);
    glClear          (GL_COLOR_BUFFER_BIT);

    glBegin        (GL_QUADS);
      glTexCoord2f (0,0);    glVertex2i   (0,0);
      glTexCoord2f (1,0);    glVertex2i   (200,0);
      glTexCoord2f (1,1);    glVertex2i   (200,200);
      glTexCoord2f (0,1);    glVertex2i   (0,200);
    glEnd          ();

    glBindTexture    (GL_TEXTURE_2D,shadow.texture);
    glCopyTexImage2D (GL_TEXTURE_2D,
                      0,
                      GL_RGBA, 
                      0,
                      0,
                      SHADOW_TEXTURE_WIDTH,
                      SHADOW_TEXTURE_WIDTH,
                      0
                     );
    glTexParameteri  (GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri  (GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  }
}

void    ShadowEnv::predraw          ()
{
  RingTmpl<SHADOW>::iterator iter  = mWaitRedraw;
  int                        count = mWaitRedraw.count ();

  glPushAttrib   (GL_ALL_ATTRIB_BITS);

  D_Try
  {
//    for (int i=0;i<count;i++,mWaitRedraw.erase (iter))
    for (int i=0;i<count;i++)
    {
      shadow_t* shadow = mShadowPool.data (iter.data());

      RenderShadow (*shadow,shadow->texture);    

      mWaitRedraw.erase (iter);
    }  
  }
  D_Exc ("ShadowEnv: Error at RenderShadow");

  glPopAttrib ();

  mRender->DelPredraw  (this);
}

void    ShadowEnv::refresh (SHADOW si)
{
  D_Try
  {
    if (si < 0 || si >= mShadowPool.num () || !mRender->GetState (RENDER_SHADOWS_BIT))
      return;

    if (!mWaitRedraw.count ())
      mRender->AddPredraw (this);

    mWaitRedraw.insert (si);
  }
  D_Exc ("ShadowEnv: Error at refresh");
}

ShadowEnv::ShadowEnv  (Pool* _Pool,MainRender* _Render,LevelLoader& _Loader,int _ShadowNum)
          : MemObject (_Pool),
            mRender   (_Render),
            mWaitRedraw (_Pool),
            mLandMap  (_Loader->mLandMap),
            mLandCache (_Loader->mLandMap->cache ()),
            mShadowPool (_Pool,_ShadowNum),
            mTexture    (NULL),
            mTriCount   (0),
            mFirst      (NULL),
            mShadowNum  (_ShadowNum)
{
  D_Try
  {
    mTexture = (uint*)GetPool()->alloc ((mShadowNum + 1)* sizeof (uint));

    if (!mTexture || !IsValid (&mShadowPool))
    {
      D_Message ("ShadowEnv: No enough memory for sys data");
      return;
    }

    glGenTextures (mShadowNum+1,mTexture);

    for (int i=0;i<_ShadowNum;i++)
    {
      shadow_t* shadow  = mShadowPool.data (i);

      shadow->next      = shadow->prev = NULL;
      shadow->enabled   = FALSE;
      shadow->light     = -1;
      shadow->mCount    = 0;
      shadow->texture   = mTexture [i];
      
      for (int j=0;j<SHADOW_MAX_MODELS;shadow->model [j++] = -1);
    }          
  }
  D_Exc ("ShadowEnv: Error at construct");
}

ShadowEnv::~ShadowEnv ()
{
  D_Try
  {
    glDeleteTextures (mShadowNum+1,mTexture);
    free             (mTexture);
  }
  D_Exc ("ShadowEnv: Error at destruct");
}
