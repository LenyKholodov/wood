#include "pch.h"
#include "r_roam.h"
#include "r_main.h"
#include "g_loader.h"
#include "g_lndcache.h"

#include <gl\gl.h>
#include <fixed\tables.h>
#include <misc.h>

/*#define NewVertex(x,y)  {                       \
  float xfl = fixed2float ((x)),                  \
        yfl = fixed2float ((y));                  \
                                                \
  glTexCoord2f (yfl*ky,xfl*kx);                 \
  glVertex3i   (xfl,mHeightMap [fixed2int((y))][fixed2int((x))],yfl);         \
} */
#define NewVertex(x,y)  {                       \
  float xfl = x,                  \
        yfl = y;                  \
                                                \
  glTexCoord2f (yfl*ky,xfl*kx);                 \
  glVertex3i   (xfl,mHeightMap [y][x],yfl);         \
} 

void     HMLandscapeRender::DrawTile  (int x,int y,int w,int levels)
{
  if (x+w >= HM_WIDTH ||  y+w >= HM_WIDTH)
    return ;

  float kx = 1.0f/float (HM_WIDTH),
        ky = 1.0f/float (HM_WIDTH);

  if (levels == 1 || w <= 2)
  {
    glBegin (GL_TRIANGLE_FAN);
      NewVertex (x+(w>>1),y+(w>>1));
      NewVertex (x,y+w);
      NewVertex (x,y);
      NewVertex (x+w,y);
      NewVertex (x+w,y+w);
      NewVertex (x,y+w);
    glEnd   ();
  }
  else 
  {
    if (w&1)
    {
      int w1 = w>>1;        

      DrawTile (x,y,w1+1,levels-1);
      DrawTile (x+w1,y,w1+1,levels-1);
      DrawTile (x,y+w1,w1+1,levels-1);
      DrawTile (x+w1,y+w1,w1+1,levels-1);
    }
    else
    {
      int w1 = w>>1;        

      DrawTile (x,y,w1,levels-1);
      DrawTile (x+w1,y,w1,levels-1);
      DrawTile (x,y+w1,w1,levels-1);
      DrawTile (x+w1,y+w1,w1,levels-1);
    }
  }
}

void     HMLandscapeRender::Draw      ()
{
  int        count;
  LandscapeMap* mMap = mainrender()->GetLandMap ();
  visnode_t* vis = mSBuffer->get (count);
  float      big = mMap->width > mMap->height ? mMap->width : mMap->height;
  float      k   = mStep / float (mZoom);
  float      kx  = float (HM_WIDTH)/float (mMap->width),
             ky  = float (HM_WIDTH)/float (mMap->height),
             ks  = kx > ky ? kx : ky;

  glPushAttrib   (GL_ENABLE_BIT);

  glDisable      (GL_CULL_FACE);
//  glDisable      (GL_TEXTURE_2D);
//  glPolygonMode  (GL_FRONT_AND_BACK,GL_LINE);
  glPushMatrix   ();
//  glScalef       (k,0.05,k);
  glScalef       (1.0f/kx*mStep,0.15,1.0f/ky*mStep);
  glDisable      (GL_LIGHTING);
  glTexEnvi      (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

  mainrender()->GetMatEnvX()->bind (mMat);

  int fx_ks = ks;

/*  for (int i=0;i<count;i++,vis++)
  {
    const tileinfo_t& info = mInfo [vis->index];
    int               l    = 8 - vis->detail;
    l                      = l < 0 ? 1 : l;                      

    DrawTile (info.coord.x*fx_ks,
              info.coord.y*fx_ks,
              fx_ks,
              6
             );
  }*/


  for (int i=0;i<mMap->width*mMap->height;i++)
  {
    const tileinfo_t& info = mInfo [i];

    DrawTile (info.coord.x*fx_ks,
              info.coord.y*fx_ks,
              fx_ks,
              4
             );

  }

  glPopMatrix    ();
  glPolygonMode  (GL_FRONT_AND_BACK,GL_FILL);
  glPopAttrib    ();
}

HMLandscapeRender::HMLandscapeRender   (Pool* pool,MainRender* mr,const char* raw_file)
                  : Render (pool,mr),
                    mInfo  (&mr->GetLoader()->mLandMap->cache ()->GetTileInfo (0)),
                    mZoom  (32),
                    mStep  (mr->GetLandMap ()->step),
                    mMat   (-1), mMatGroup (-1)
{
  BinFile* file = F_OpenFile (raw_file,BF_OPEN_RO);

  if (!IsValid (file))
  {
    D_Message ("HMLandscapeRender: Error at open raw file: '"<<raw_file<<"'");
    return;
  }

  file->read (mHeightMap,HM_WIDTH*HM_WIDTH);

  matdesc_t m;

  strcpy (m.text.diffuse,"hm_color.tga");
  strcpy (m.name,"hm_texture");

  mMat      = mr->GetMatEnvX()->AddMat ("hm_texture",m);
  mMatGroup = mr->GetMatEnvX()->CreateMatGroup (&mMat);

  delete file;
}

HMLandscapeRender::~HMLandscapeRender ()
{  
  mainrender()->GetMatEnvX()->DeleteMatGroup (mMatGroup);
}
