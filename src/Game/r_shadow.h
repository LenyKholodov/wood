#ifndef __RENDER_SHADOW_ENVIRONMENT__
#define __RENDER_SHADOW_ENVIRONMENT__

#include <graph\grtypes.h>
#include <pool.h>

#include "defs.h"
#include "r_predrawwait.h"

class    ModelKeeper;
class    TileLightEnv;
class    MainRender;
class    LandscapeMap;
class    LandscapeCache;
class    LevelLoader;

const float SHADOW_MAX_DEPTH            = float(0x10000);
const int   SHADOW_TEXTURE_WIDTH        = 1024;
const float SHADOW_GROUND_DIST          = 16;
const int   SHADOW_MAX_MODELS           = 8;

////////////////////////////////////////////////////////////////////////////////
///Рендер теней
////////////////////////////////////////////////////////////////////////////////
class    ShadowEnv: public MemObject, public PredrawWait
{
  public:
                ShadowEnv  (Pool*,MainRender*,LevelLoader&,int _ShadowNum);
    virtual     ~ShadowEnv ();

    virtual bool   valid   () const { return mShadowPool.valid () && mTexture != NULL; }

////////////////////////////////////////////////////////////////////////////////
///Создание теневой пары
////////////////////////////////////////////////////////////////////////////////
    SHADOW      CreateShadow    (LIGHTSRC,MODEL = -1);
    void        DeleteShadow    (SHADOW);

////////////////////////////////////////////////////////////////////////////////
///Присоединение моделей к теневой паре
////////////////////////////////////////////////////////////////////////////////
    BOOL        InsertModel     (SHADOW,MODEL);
    void        ExtractModel    (SHADOW,MODEL);

////////////////////////////////////////////////////////////////////////////////
///Объявление тени видимой
////////////////////////////////////////////////////////////////////////////////
    void        EnableShadow    (SHADOW);
    void        DisableShadow   (SHADOW);
    void        ClearAll        ();
    void        reset           () { ClearAll (); }

////////////////////////////////////////////////////////////////////////////////
///Отрисовка
////////////////////////////////////////////////////////////////////////////////
    void        Draw            (int,visnode_t*);

////////////////////////////////////////////////////////////////////////////////
///Обновление тексуры теневой пары
////////////////////////////////////////////////////////////////////////////////
    void        refresh         (SHADOW);

////////////////////////////////////////////////////////////////////////////////
///Свойства тени
////////////////////////////////////////////////////////////////////////////////
    void        ShadowClean     (SHADOW,int num = 0);

////////////////////////////////////////////////////////////////////////////////
///Доступ
////////////////////////////////////////////////////////////////////////////////
    uint        GetPgCount      () { uint cnt = mTriCount; mTriCount = 0; return cnt; }        

  protected:
        struct shadow_t
        {
          matrix_t      transform;
          frustum_t     frustum;

          MODEL         model [SHADOW_MAX_MODELS];
          LIGHTSRC      light;
          int           mCount;

          BOOL          enabled;
          shadow_t*     prev;
          shadow_t*     next;

          int           texture;

          int           clean;
        };  

        void    RenderShadow      (shadow_t&,int texture);
        void    CalcBound         (MODEL*,int,boundsphere_t&);
        void    CleanShadow       (shadow_t&,int num);

  private:
    virtual void predraw          ();

  private: 
        ClPool<shadow_t>    mShadowPool;
        RingTmpl<SHADOW>    mWaitRedraw;
        MainRender*         mRender;
        LandscapeCache*     mLandCache;
        LandscapeMap*       mLandMap;
        shadow_t*           mFirst;
        uint*               mTexture, mShadowNum;
        uint                mTriCount;
};

#endif