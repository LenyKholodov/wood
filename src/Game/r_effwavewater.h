#ifndef __RENDER_EFFECT_WAVE_WATER__
#define __RENDER_EFFECT_WAVE_WATER__

#include "r_mdleffect.h"
#include <time.h>

class    WaterEffect;

struct   watermodel_t
{
  WaterEffect*      effect;
};

enum    WaterEffectId {
  EFFECT_WAVE_WATER = 0
};

class   WaterEffect: public MemObject
{
  friend class WaterModelKeeper;
  public:
                WaterEffect  (Pool*,WaterModelKeeper*);
     virtual    ~WaterEffect ();

     virtual    void tick             () = 0;
     virtual    void draw             () = 0;

  protected:
                void SetTexture       (int&,ulong&,const textmat_t&);
                void AddPgCount       (uint cnt);
  protected:
     WaterModelKeeper* mKeeper;
};

////////////////////////////////////////////////////////////////////////////////
///Keeper для волнистой воды
////////////////////////////////////////////////////////////////////////////////
class    WaterModelKeeper: public EffectModelKeeper
{
  friend class WaterEffect;
  public:
                WaterModelKeeper  (Pool*,ModelEnv*,size_t _EffPoolSize = 0);
     virtual    ~WaterModelKeeper ();

  protected:
     virtual    void       tick       (model_t*);

     virtual    int        filetype      () const { return EFFECT_WATER_ID; }

////////////////////////////////////////////////////////////////////////////////
///Работа с моделью
////////////////////////////////////////////////////////////////////////////////
    virtual     BOOL       InitModel        (model_t*,MODELDATA,va_list);
    virtual     void       DoneModel        (model_t*);

////////////////////////////////////////////////////////////////////////////////
///Рендеринг
////////////////////////////////////////////////////////////////////////////////
    virtual void           render           (const modelnode_t*,const Camera&);
            void           SetTexture       (int&,ulong&,const textmat_t&);
            void           AddPgCount       (uint cnt) { mPgCount += cnt; }

  private:
        Pool*           mEffectPool;
        BOOL            mCreatePool;
};

////////////////////////////////////////////////////////////////////////////////
///Эффект колеблющейся воды
////////////////////////////////////////////////////////////////////////////////
class   WaveWaterEffect: public WaterEffect
{
  friend class WaterModelKeeper;
  public:
                WaveWaterEffect  (Pool*,
                                  const boundbox_t&,
                                  const char* texName,
                                  WaterModelKeeper*
                                 );
     virtual    ~WaveWaterEffect ();

     virtual    void tick             ();
     virtual    void draw             ();

  protected:
           void refresh          ();       

  protected:
        vertex_t*       mVertex;
        vector3d_t*     mDir;
        textcoord_t*    mTextCoord;
        textmat_t       mTextName;
        int             mTexture;
        ulong           mCRC;        
        boundbox_t      mBBox;
        float           mZAmp;

        clock_t         mLast;
};

#endif