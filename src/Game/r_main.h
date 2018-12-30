#ifndef __MAIN_RENDER__
#define __MAIN_RENDER__

#include <graph\utils.h>
#include "camera.h"
#include <graph\textenv.h>
#include <misc.h>

#include "g_level.h"

#include "r_landscape.h"
#include "r_sbuffer.h"
#include "r_lenv.h"
#include "r_mdlenv.h"
#include "r_matenvx.h"
#include "r_shadow.h"
#include "r_skybox.h"
//#include "r_treeenv.h"
#include "r_predrawwait.h"
#include "g_grcon.h"
#include "r_alphamesh.h"
#include "r_hmrender.h"

const    int    RENDER_DEFAULT_FLAG             = 0xFFFFFFFF;
const    int    RENDER_SHADOWS_BIT              = 1;
const    int    RENDER_SKY_BIT                  = 2;
const    int    RENDER_CLEAR_COLOR_BUFFER_BIT   = 4;
const    int    RENDER_CLEAR_DEPTH_BUFFER_BIT   = 8;

////////////////////////////////////////////////////////////////////////////////
///Главная система рендеринга
////////////////////////////////////////////////////////////////////////////////
class    MainRender: public MemObject
{
  friend class Render;
  public:
                        MainRender  (Pool*,LevelLoader&);
      virtual           ~MainRender ();

      virtual  bool     valid       () const;         

////////////////////////////////////////////////////////////////////////////////
///Отрисовка
////////////////////////////////////////////////////////////////////////////////
     virtual   void     Draw        (const Camera&);
     virtual   void     DrawSky     (const Camera&);

               void     Draw2DLand  (const Camera&); //for debug version

////////////////////////////////////////////////////////////////////////////////
///Predraw 
////////////////////////////////////////////////////////////////////////////////
                void    AddPredraw  (const PredrawWait*);
                void    DelPredraw  (const PredrawWait*);
      virtual   void    Predraw     ();

////////////////////////////////////////////////////////////////////////////////
///Bind/Unbind/Enable/Disable system
////////////////////////////////////////////////////////////////////////////////
                void    BindConsole   (ConsoleBase&,int font =0,int = 80,int = 25);
               
////////////////////////////////////////////////////////////////////////////////
///Утильё
////////////////////////////////////////////////////////////////////////////////
    const frustum_t&        GetFrustum     () const { return mFrustum;  }
          LandscapeMap*     GetLandMap     () const { return mMap; }
          LandMeshRender*   GetLandRender  () const { return mLandRender; }
          TileLightEnv*     GetLightEnv    () const { return mLightEnv; }
          ModelEnv*         GetModelEnv    () const { return mModelEnv; }
          ShadowEnv*        GetShadowEnv   () const { return mShadowEnv; }
          SkyBoxRender*     GetSkyBox      () const { return mSkyBox; }
//          TreeEnv*          GetTreeEnv     () const { return mTreeEnv; }
          OGLConsole*       GetConsole     () const { return mConsole; }
          LevelLoader&      GetLoader      () const { return mLoader; }
          AlphaMeshRender*  GetAlpha       () const { return mAlphaRender; }
          MatEnvX*          GetMatEnvX     () const { return mMatEnvX; }

          uint              GetPgCount     ();
          BOOL              GetState       (uint flag) const { return (mEnable & flag) != 0; }
          void              ChangeState    (uint flag,BOOL state);
          uint              GetFrame       () const { return mFrame; }

  public:
          BOOL              SetMaterial    (int,int); 
          BOOL              ChangeEnv      (uint,uint);

  protected:
               void     DrawLandscape (const Camera&);
               float    GetAngle      (float,float);

               void     DrawShadows   ();
               void     ResetSBuffer  (const Camera&);

  protected:  
        LevelLoader&            mLoader;
        LandscapeMap*           mMap;
        GameLevel*              mLevel;
        SBuffer*                mSBuffer;        
        LandMeshRender*         mLandRender;        
        FlagBit*                mFlag;
        TileLightEnv*           mLightEnv;
        ModelEnv*               mModelEnv;
  //      TreeEnv*                mTreeEnv;
        ShadowEnv*              mShadowEnv;
        SkyBoxRender*           mSkyBox;
        OGLConsole*             mConsole;
        AlphaMeshRender*        mAlphaRender;
        MatEnvX*                mMatEnvX;
        HMapRender*             mHMLandRender;

        RingTmpl<PredrawWait*>  mPredraw;

        frustum_t               mFrustum;               
        uint                    mEnable;
        uint                    mFrame;          
        BOOL                    mChangeDepthState;
};

#endif