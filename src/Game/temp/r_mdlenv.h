#ifndef __RENDER_MODEL_ENV__
#define __RENDER_MODEL_ENV__

#include "r_mdlkeeper.h"
#include "g_map.h"

const    int    MODEL_KEEPERS_NUM = 512;
class    SBuffer;

////////////////////////////////////////////////////////////////////////////////
///Окружение моделей (относится к LevelLoader)
////////////////////////////////////////////////////////////////////////////////
class ModelEnv: public MemObject
{
  friend class ModelKeeper;
  public:
                ModelEnv  (Pool*,
                           LandscapeMap*,
                           uint   _ModelNum,
                           uint   _VisNum
                          );
     virtual    ~ModelEnv ();

////////////////////////////////////////////////////////////////////////////////
///Работа с моделью
////////////////////////////////////////////////////////////////////////////////          
     MODEL      CreateModel      (int _ModelType,...);
     MODEL      DuplicateModel   (const MODEL);
     void       DeleteModel      (MODEL);
     void       ChangeProperties (MODEL,...);

     void       SetPosition      (MODEL,const vector3d_t&);
     void       SetTransMatrix   (MODEL,const matrix_t&);
     void       SetBoundBox      (MODEL,const boundbox_t&);

////////////////////////////////////////////////////////////////////////////////
///Присоединение рендеров
////////////////////////////////////////////////////////////////////////////////
     BOOL       BindKeeper       (int _ModelType,ModelKeeper*);
     void       UnbindKeeper     (int _ModelType);
     BOOL       IsPresent        (int _ModelType);

     void       SetEnvFn         (SetMaterialFn,ChangeEnvFn,DWORD);

////////////////////////////////////////////////////////////////////////////////
///Работа с MainRender
////////////////////////////////////////////////////////////////////////////////
//     void       insert           (int x,int y,int detail = 0);
     void       insert           (const SBuffer&,int tile,int detail = 0);
     void       reset            ();

     void       render           ();
     void       set              (const frustum_t&);

     void       SetMaterial      (int);
     void       ChangeEnv        (uint tile,uint _InterruptDrawing = 0);

  protected:
////////////////////////////////////////////////////////////////////////////////
///Работа с памятью
////////////////////////////////////////////////////////////////////////////////
     void*         AllocModelData   (void**,size_t);
     void          FreeModelData    (void**);

     void          InsertModel      (MODEL);
     void          ExtractModel     (MODEL);

     static BOOL   MoveData         (void* _From,void* _To,DWORD);

     struct     mdlnode_t
     {
       model_t        model;
       mdlnode_t*     next;
     };

     struct memnode_t
     {
       void**         node;
       char           data [];
     };     

  protected:
     LandscapeMap*    mLandMap;
     mdlnode_t*       mModelNode;
     mdlnode_t*       mFirstFree;
     DefragPool*      mDefragPool;

     ModelKeeper*     mKeeper [MODEL_KEEPERS_NUM];
     frustum_t        mFrustum;     

     MODEL*           mMap;

     modelnode_t*     mVisNode;
     modelnode_t*     mKprDraw [MODEL_KEEPERS_NUM];
     int              mVisPos, mVisNum;

     SetMaterialFn    mSetMaterial;
     ChangeEnvFn      mChangeEnv;
     DWORD            mParam;
};

#endif