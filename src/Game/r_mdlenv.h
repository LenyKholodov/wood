#ifndef __RENDER_MODEL_ENV__
#define __RENDER_MODEL_ENV__

#include "r_mdlkeeper.h"
#include "r_mdldatacache.h"
#include "r_mdlmap.h"
#include "r_mdlpool.h"
#include "g_map.h"

const    int    MODEL_KEEPERS_NUM = 32;
class    SBuffer;

////////////////////////////////////////////////////////////////////////////////
///Окружение моделей (относится к LevelLoader)
////////////////////////////////////////////////////////////////////////////////
class ModelEnv: public MemObject
{
  friend class ModelKeeper;
  friend class ModelMap;
  public:
                ModelEnv  (Pool*,
                           LandscapeMap*,
                           uint   _ModelDataNum,
                           uint   _ModelNum,
                           uint   _VisNum,
                           size_t _DataCacheSize,
                           size_t _ModelPoolSize
                          );
     virtual    ~ModelEnv ();

     virtual bool valid   () const { return mModelDescPool.valid () &&
                                     mModelPool.valid () && IsValid (mModelMap) &&
                                     IsValid (mDataCache) && mModelPool.valid ();
                                   }

////////////////////////////////////////////////////////////////////////////////
///Работа с моделью
////////////////////////////////////////////////////////////////////////////////
     MODELDATA  LoadModelData    (int _ModelType,BOOL _AutoLoad,...);
     void       UnloadModelData  (MODELDATA);

     MODEL      CreateModel      (MODELDATA,...);
     MODEL      DuplicateModel   (const MODEL);
     void       DeleteModel      (MODEL);

     void       ChangeProperties (MODEL,...);
     void       SetPosition      (MODEL,const vector3d_t&);
     void       SetBoundBox      (MODEL,const boundbox_t&);

     const
     bound_t    GetBound         (MODEL);
     void       RenderShadow     (MODEL,const lightsrc_t&);

////////////////////////////////////////////////////////////////////////////////
///Присоединение рендеров
////////////////////////////////////////////////////////////////////////////////
     MODELKEEPER  BindKeeper       (ModelKeeper*);
     void         UnbindKeeper     (int _ModelType);
     BOOL         IsPresent        (int _ModelType);

////////////////////////////////////////////////////////////////////////////////
///Установка функций окружения
////////////////////////////////////////////////////////////////////////////////
     void       SetSBuffer       (const SBuffer*);
     void       SetFrustum       (const frustum_t&);
     void       SetCamera        (const Camera&);

////////////////////////////////////////////////////////////////////////////////
///Работа с MainRender
////////////////////////////////////////////////////////////////////////////////
     void       insert           (int x,int y,int tile,int detail = 0);
     void       reset            ();

     void       render           ();

     void       bind             (MainRender*);

////////////////////////////////////////////////////////////////////////////////
///Работа с ModelMap
////////////////////////////////////////////////////////////////////////////////
     void       InsertModel      (model_t*);
     void       ExtractModel     (model_t*);

////////////////////////////////////////////////////////////////////////////////
///Доступ
////////////////////////////////////////////////////////////////////////////////
     uint       GetPgCount       ();

  protected:
     void       DeleteModel      (model_t*);  
     ModelPool* GetModelPool     ()  { return &mModelPool; }     

     typedef Dict<int>  KDict;

  private:
     ClPool<model_t>    mModelDescPool;
     ModelPool          mModelPool;

     LandscapeMap*      mLandMap;
     ModelMap*          mModelMap;
     ModelDataCache*    mDataCache;
     MainRender*        mMainRender;

     ModelKeeper*       mKeeper [MODEL_KEEPERS_NUM];

     modelnode_t*       mVisNode;
     modelnode_t*       mKprDraw [MODEL_KEEPERS_NUM];
     int                mVisPos, mVisNum;

     SBuffer*           mSBuffer;
     frustum_t          mFrustum;     
     Camera             mCamera;
};

#endif