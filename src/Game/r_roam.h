#ifndef __RENDER_ROAM_LANDSCAPE__
#define __RENDER_ROAM_LANDSCAPE__

#include "g_map.h"
#include "r_render.h"
#include <binfile.h>
#include <fixed\fixed.h>

#define  HM_WIDTH 2048

class    HMLandscapeRender: public Render
{
  public:
                 HMLandscapeRender   (Pool*,MainRender*,const char*);
     virtual     ~HMLandscapeRender ();

     virtual     void     Draw      ();

  private:
//                 void     DrawTile  (Fixed x,Fixed y,Fixed w,int levels);
                 void     DrawTile  (int x,int y,int w,int levels);

  private:
    const  tileinfo_t*  mInfo;
    uchar               mHeightMap [HM_WIDTH][HM_WIDTH];  
    int                 mZoom;
    float               mStep;  //duplicate map
    MATERIAL            mMat;
    MATGROUP            mMatGroup;
};

#endif