#ifndef __RENDER_TREE_RENDER__
#define __RENDER_TREE_RENDER__

#include <pool.h>
#include "r_matrender.h"

class    TreeEnv;
class    TreeMap;
class    TreePrecompile;
class    TreeMeshCache;
class    TreeVertexCache;
class    SBuffer;
class    MatDictionary;
struct   treemapnode_t;

////////////////////////////////////////////////////////////////////////////////
///Рендер деревьев
////////////////////////////////////////////////////////////////////////////////
class    TreeRender: public MaterialRender
{
  public:       
                TreeRender  (Pool*,MainRender*,TreeEnv&);
      virtual   ~TreeRender ();

////////////////////////////////////////////////////////////////////////////////
///Отрисовка
////////////////////////////////////////////////////////////////////////////////
           void         AddTile         (int x,int y,int detail = 0);
           void         Draw            ();

           void         SetScale        (const vector3d_t& v) { mScale = v; }

  protected:
    virtual void         SetVertexBuffer (mesh_t*);
    virtual void         reset           ();
    virtual BOOL         ChangeState     (DWORD);
    virtual void         DrawAlpha       (DWORD,const Camera&);
            void         ReindexMat      (MatDictionary&);

  private:
    TreeMap&            mTreeMap;
    TreeMeshCache*      mTreeCache;
    TreeVertexCache*    mVertexCache;
    uint                mWidth, mHeight;
    int                 mFirstTexture;  
    vector3d_t          mScale;
};

#endif