#ifndef __RENDER_SKY_LIGHT__
#define __RENDER_SKY_LIGHT__

#include "r_lenv.h"

////////////////////////////////////////////////////////////////////////////////
///Описание небесных светил: солнце, луна
////////////////////////////////////////////////////////////////////////////////
class    SkyBoxRender;
class    SkyLight: public MemObject
{
  public:
                        SkyLight (Pool*,TileLightEnv*,SkyBoxRender*,const boundbox_t&);
         virtual        ~SkyLight ();

////////////////////////////////////////////////////////////////////////////////
///Обновление
////////////////////////////////////////////////////////////////////////////////
         void           refresh     (float hour);
         
         void           SetSkyColor (int hour,const color3d_t*,int num = 1);
         void           SetSunLight (int hour,const color3d_t*,int num = 1);
         void           SetAmbient  (int hour,const color3d_t*,int num = 1);

         void           SetShine    (float shine) { mSunShine = shine; }

         void           SetSunLimit (float hour1,float hour2,float angle);

         LIGHTSRC       GetLight    () const { return mLight; }

  protected:
         void           lerp        (vector3d_t&,float t,const vector3d_t&,const vector3d_t&);

  protected:
         color3d_t*     mSkyColor;
         color3d_t*     mAmbient;
         color3d_t*     mSunLight;

         LIGHTSRC       mLight;
         TileLightEnv*  mLightEnv;
         SkyBoxRender*  mSkyBox;
         boundbox_t     mBBox;
         float          mKoef, mMorning;
         matrix_t       mRot;
         float          mSunDist;
         float          mSunShine;
};

#endif