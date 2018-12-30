#ifndef __RENDER_MATERIAL_RENDER__
#define __RENDER_MATERIAL_RENDER__

#include <pool.h>
#include "r_render.h"

class     MatEnvX;
struct    matdesc_t;

////////////////////////////////////////////////////////////////////////////////
///Рендер мэшей по материалам (использует полиморфную функцию кэширования)
////////////////////////////////////////////////////////////////////////////////
class     MaterialRender: public Render
{
  public:
                        MaterialRender  (Pool*,
                                         MainRender*,
                                         int    _MatNum,
                                         int    _MeshNum
                                        );
           virtual      ~MaterialRender ();

////////////////////////////////////////////////////////////////////////////////
///Отрисовка
////////////////////////////////////////////////////////////////////////////////
           void         AddMesh         (mesh_t*,int* tiles = NULL,int = 1,DWORD param = 0);
           void         AddMesh         (mesh_t*,int tile,int = 1,DWORD param = 0);
   virtual void         DrawShadow      (mesh_t*,int = 1,DWORD param = 0);  //without clip-check
   virtual void         Draw            ();

////////////////////////////////////////////////////////////////////////////////
///Внешние функции изменения состояния
////////////////////////////////////////////////////////////////////////////////
           void         SetFirstMat     (int mat = -1) { mFirstMat = mat; }

           virtual bool valid           () const { return mMat != NULL && mMeshNode != NULL; }

  protected:
   virtual void         reset           () {}
   virtual BOOL         ChangeState     (DWORD) { return FALSE; }
           void         Flush           ();

           BOOL         SetMaterial     (int mat,int mode = RENDER_TEXT_SET | RENDER_MAT_SET);
           matdesc_t&   GetMaterial     (int mat);
           void         ReindexMat      ();
  
  private:
           struct       meshnode_t;
           struct       matnode_t;

    matnode_t*          mMat;
    meshnode_t*         mMeshNode;
    int                 mMatFirst, mMeshPos;

    int                 mMatNum, mListNum;

  protected:
    int                 mFirstMat;
    int*                mMatReindex;
    MatEnvX*            mMatEnv;    
    MATGROUP            mMatGroup;
};

#endif