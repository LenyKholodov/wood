#ifndef __RENDER_SKY_BOX__
#define __RENDER_SKY_BOX__

#include "defs.h"
#include <pool.h>

class    MatEnvX;
class    GameLevel;
class    Camera;

////////////////////////////////////////////////////////////////////////////////
///Рендер неба
////////////////////////////////////////////////////////////////////////////////
class    SkyBoxRender: public MemObject
{
  public:
//                        SkyBoxRender  (Pool*,const boundbox_t&,GameLevel*,MatEnv*);
                        SkyBoxRender  (Pool*,float dist,GameLevel*,MatEnvX*);
            virtual     ~SkyBoxRender ();
            
                        enum {
                          BACK_ID = 0,
                          FRONT_ID,
                          BOTTOM_ID,
                          TOP_ID,
                          LEFT_ID,
                          RIGHT_ID
                        };

            void        Draw         (const Camera&);
            void        SetSkyColor  (const vector3d_t&);
            uint        GetPgCount   () { int cnt = mPgCount; mPgCount = 0; return cnt; }

  protected:
            void        BindTexture  (uint);
  
  protected:
        MatEnvX*   mMatEnv;
        MATGROUP   mMatGroup;
        uint       mPgCount;
        int        mTexture [6];
        float      mDist;
        vector3d_t mSkyColor;
};

#endif