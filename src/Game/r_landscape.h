#ifndef  __LANDSCAPE_MESH_RENDER__
#define  __LANDSCAPE_MESH_RENDER__

#include "g_map.h"
#include "r_matrender.h"

struct visnode_t;
class  LevelLoader;
class  LandscapeCache;
class  LandVertexCache;

////////////////////////////////////////////////////////////////////////////////
///Рендер ландшафта по мэшам
////////////////////////////////////////////////////////////////////////////////
class    LandMeshRender: public MaterialRender
{
  friend class LandscapeCache;
  friend class LandscapeNoCache;
  public:
                        LandMeshRender  (Pool*,
                                         MainRender*,
                                         int    _ListNum
                                        );
           virtual      ~LandMeshRender ();

////////////////////////////////////////////////////////////////////////////////
///Отрисовка
////////////////////////////////////////////////////////////////////////////////
    virtual void         DrawShadow      (const frustum_t&,visnode_t*,int = 1);
            void         AddTile         (visnode_t*,int = 1);
            void         AddTile         (int x,int y,int index,int detail = 0);

    virtual void         Draw            ();


  protected:
    virtual void         SetVertexBuffer (mesh_t*);
    virtual void         reset           ();
            void         ReindexMat      ();

  private:
    static  uint         GetMatCount     (MainRender*);
  
  private:
    LandscapeMap*       mMap;    
    LandscapeCache*     mCache;
    LandVertexCache*    mVertexCache;

    float*              mFog;
    uint                mFogCount;
    uint                mCurBase;
};

#endif