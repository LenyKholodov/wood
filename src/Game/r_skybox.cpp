#include "pch.h"
#include "r_skybox.h"
#include "r_matenvx.h"
#include "g_level.h"

#include <gl\gl.h>
#include <binfile.h>
#include <misc.h>
#include <graph\utils.h>
#include "camera.h"

void        SkyBoxRender::SetSkyColor  (const vector3d_t& color)
{
  mSkyColor = color;
}

void        SkyBoxRender::BindTexture  (uint i)
{
  mMatEnv->bind    (mTexture [i]);

//  glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//  glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexEnvi        (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
}

void        SkyBoxRender::Draw (const Camera& eye)
{
  D_Try
  {
    static material_t _DefMat;
    ::SetMaterial (_DefMat);

    const vector3d_t& pos     = eye.pos;
    float             width   = mDist * 2,
                      height  = mDist * 2,
                      length  = mDist * 2;
    float             x       = pos.x - mDist,
                      y       = pos.y - mDist,
                      z       = pos.z - mDist;

    glDisable  (GL_CULL_FACE);
    glDisable  (GL_LIGHTING);

    glColor3fv ((GLfloat*)&mSkyColor);

    BindTexture (BACK_ID);

    glBegin (GL_QUADS);              
      glTexCoord2f (0.0f, 1.0f); glVertex3f (x,         y,          z);
      glTexCoord2f (0.0f, 0.0f); glVertex3f (x,         y + height, z);
      glTexCoord2f (1.0f, 0.0f); glVertex3f (x + width, y + height, z); 
      glTexCoord2f (1.0f, 1.0f); glVertex3f (x + width, y,          z);
    glEnd ();

    BindTexture (FRONT_ID);

    glBegin (GL_QUADS);      
      glTexCoord2f (1.0f, 1.0f); glVertex3f (x,         y,          z + length);
      glTexCoord2f (1.0f, 0.0f); glVertex3f (x,         y + height, z + length);
      glTexCoord2f (0.0f, 0.0f); glVertex3f (x + width, y + height, z + length); 
      glTexCoord2f (0.0f, 1.0f); glVertex3f (x + width, y         , z + length);
    glEnd ();

    BindTexture (BOTTOM_ID);

    glBegin (GL_QUADS);              
      glTexCoord2f (1.0f, 1.0f); glVertex3f (x,         y, z);
      glTexCoord2f (1.0f, 0.0f); glVertex3f (x,         y, z + length);
      glTexCoord2f (0.0f, 0.0f); glVertex3f (x + width, y, z + length); 
      glTexCoord2f (0.0f, 1.0f); glVertex3f (x + width, y, z);
    glEnd ();

    BindTexture (TOP_ID);

    glBegin (GL_QUADS);              
      glTexCoord2f (1.0f, 0.0f); glVertex3f (x,         y + height, z);
      glTexCoord2f (1.0f, 1.0f); glVertex3f (x,         y + height, z + length);
      glTexCoord2f (0.0f, 1.0f); glVertex3f (x + width, y + height, z + length); 
      glTexCoord2f (0.0f, 0.0f); glVertex3f (x + width, y + height, z);
    glEnd ();

    BindTexture (LEFT_ID);

    glBegin (GL_QUADS);              
      glTexCoord2f (1.0f, 1.0f); glVertex3f (x, y,          z);
      glTexCoord2f (0.0f, 1.0f); glVertex3f (x, y,          z + length);
      glTexCoord2f (0.0f, 0.0f); glVertex3f (x, y + height, z + length); 
      glTexCoord2f (1.0f, 0.0f); glVertex3f (x, y + height, z);
    glEnd ();

    BindTexture (RIGHT_ID);

    glBegin (GL_QUADS);
      glTexCoord2f (0.0f, 1.0f); glVertex3f (x + width, y,          z);
      glTexCoord2f (1.0f, 1.0f); glVertex3f (x + width, y,          z + length);
      glTexCoord2f (1.0f, 0.0f); glVertex3f (x + width, y + height, z + length); 
      glTexCoord2f (0.0f, 0.0f); glVertex3f (x + width, y + height, z);
    glEnd ();    

    glEnable (GL_CULL_FACE);
    glEnable (GL_LIGHTING);

    mPgCount += 12;
  }
  D_Exc ("SkyBoxRender: Error at Draw");
}

SkyBoxRender::SkyBoxRender  (Pool* _Pool,float r,GameLevel* lev,MatEnvX* me)
       : MemObject (_Pool),
         mMatEnv   (me),
         mDist     (sqrt (sqr (r))/2.0f),
         mPgCount  (0),
         mSkyColor (0.7), mMatGroup (-1)
{
  D_Try
  {
    BinFile* file  = lev->CreateLump (LL_SKY_BOX,GameLevel::OPEN_RO);

    if (!IsValid (file))
    {
      D_Message ("SkyBoxRender: Error at load sky box");
      return;      
    }

    textmat_t tm [6];

    file->read (tm,sizeof (textmat_t) * 6);

    delete file;
    mTexture [0] = mMatEnv->CreateMat (matdesc_t (tm [0].text),&mMatGroup,6);
    mTexture [1] = mMatEnv->CreateMat (matdesc_t (tm [1].text),&mMatGroup);
    mTexture [2] = mMatEnv->CreateMat (matdesc_t (tm [2].text),&mMatGroup);
    mTexture [3] = mMatEnv->CreateMat (matdesc_t (tm [3].text),&mMatGroup);
    mTexture [4] = mMatEnv->CreateMat (matdesc_t (tm [4].text),&mMatGroup);
    mTexture [5] = mMatEnv->CreateMat (matdesc_t (tm [5].text),&mMatGroup);

/*    for (int i=0;i<6;i++)
    {
      matdesc_t m;

      m.color = tm [i].m;
      strcpy (m.text.diffuse,tm [i].text);
      strcpy (m.name,tm [i].name);

      mTexture [i] = mMatEnv->AddMat (tm [i].name,m);
    }

    mMatGroup  = mMatEnv->CreateMatGroup (mTexture,6,6);*/
  }
  D_Exc ("SkyBoxRender: Error at construct");
}

SkyBoxRender::~SkyBoxRender ()
{
  for (int i=0;i<6;i++)  mMatEnv->DelMat (mTexture [i]);
//  mMatEnv->DeleteMatGroup (mMatGroup);
}
