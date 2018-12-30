#include "r_roam.h"
#include "r_main.h"
#include "g_loader.h"
#include "g_lndcache.h"

#include <gl\gl.h>

void     HMLandscapeRender::DrawTile  (int x,int y,int w,int levels)
{
  if (x+w >= HM_WIDTH  || y+w >= HM_WIDTH )
    return ;

  float kx = 1.0f/float (mZoom*mainrender()->GetLandMap()->width),
        ky = 1.0f/float (mZoom*mainrender()->GetLandMap()->height);

  if (levels == 1 || w <= 2)
  {
    glBegin (GL_TRIANGLE_FAN);
      glTexCoord2f (float (x+(w>>1))*kx,float (y+(w>>1))*ky);
      glVertex3i   (x+(w>>1),mHeightMap [y+(w>>1)][x+(w>>1)],y+(w>>1));

      glTexCoord2f (float (x)*kx,float (y+w)*ky);
      glVertex3i   (x,mHeightMap [y+w][x],y+w);

      glTexCoord2f (float (x)*kx,float (y)*ky);
      glVertex3i   (x,mHeightMap [y][x],y);

      glTexCoord2f (float (x+w)*kx,float (y)*ky);
      glVertex3i   (x+w,mHeightMap [y][x+w],y);

      glTexCoord2f (float (x+w)*kx,float (y+w)*ky);
      glVertex3i   (x+w,mHeightMap [y+w][x+w],y+w);

      glTexCoord2f (float (x)*kx,float (y+w)*ky);
      glVertex3i   (x,mHeightMap [y+w][x],y+w);
    glEnd   ();
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

void     HMLandscapeRender::Draw      ()
{
  int        count;
  visnode_t* vis = mSBuffer->get (count);
  float      k   = mStep / float (mZoom);

  glPushAttrib   (GL_ENABLE_BIT);

  glDisable      (GL_CULL_FACE);
//  glDisable      (GL_TEXTURE_2D);
//  glPolygonMode  (GL_FRONT_AND_BACK,GL_LINE);
  glPushMatrix   ();
  glScalef       (k,0.05,k);
//  glColor3f      (1,1,1);
  glDisable      (GL_LIGHTING);
  glTexEnvi      (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

  mainrender()->GetMatEnvX()->bind (mMat);

  for (int i=0;i<count;i++,vis++)
  {
    const tileinfo_t& info = mInfo [vis->index];
    int               l    = 8 - vis->detail;

    if (l <= 0) l = 1;

    DrawTile (info.coord.x*mZoom,info.coord.y*mZoom,mZoom,4);
//    DrawTile (info.coord.x*mZoom,info.coord.y*mZoom,mZoom,2);
  }

  glPopMatrix    ();
  glPolygonMode  (GL_FRONT_AND_BACK,GL_FILL);
  glPopAttrib    ();
}

HMLandscapeRender::HMLandscapeRender   (Pool* pool,MainRender* mr,const char* raw_file)
                  : Render (pool,mr),
                    mInfo  (&mr->GetLoader()->mLandCache->GetTileInfo (0)),
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

  strcpy (m.text.diffuse,"hm_color1.tga");
  strcpy (m.name,"hm_texture");

  mMat      = mr->GetMatEnvX()->AddMat ("hm_texture",m);
  mMatGroup = mr->GetMatEnvX()->CreateMatGroup (&mMat);

  delete file;
}

HMLandscapeRender::~HMLandscapeRender ()
{  
  mainrender()->GetMatEnvX()->DeleteMatGroup (mMatGroup);
}



#ifndef __RENDER_ROAM_LANDSCAPE__
#define __RENDER_ROAM_LANDSCAPE__

#include "g_map.h"
#include "r_render.h"
#include <binfile.h>

#define  HM_WIDTH 512

class    HMLandscapeRender: public Render
{
  public:
                 HMLandscapeRender   (Pool*,MainRender*,const char*);
     virtual     ~HMLandscapeRender ();

     virtual     void     Draw      ();

  private:
                 void     DrawTile  (int x,int y,int w,int levels);

  private:
    const  tileinfo_t*  mInfo;
    char                mHeightMap [HM_WIDTH][HM_WIDTH];  
    int                 mZoom;
    float               mStep;  //duplicate map
    MATERIAL            mMat;
    MATGROUP            mMatGroup;
};

#endif