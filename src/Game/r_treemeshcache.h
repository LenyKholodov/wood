#ifndef __RENDER_TREE_MESH_CACHE__
#define __RENDER_TREE_MESH_CACHE__

#include <pool.h>

#include "r_vbuffer.h"

class   TreeRender;
class   TreePrecompile;
class   TreeEnv;

////////////////////////////////////////////////////////////////////////////////
///���஢���� ����-��襩
////////////////////////////////////////////////////////////////////////////////
class   TreeMeshCache: public DefragCache
{
  public:
                        TreeMeshCache  (Pool*,TreePrecompile&,size_t);
          virtual       ~TreeMeshCache ();

////////////////////////////////////////////////////////////////////////////////
///��ᮥ������� ७���
////////////////////////////////////////////////////////////////////////////////
          void          BindRender     (TreeRender* render) { mRender = render; }
          void          SetFirstText   (int text) { mFirstText = text; }

  protected:
          DefragCache::get;

   virtual dcachenode_t*   LoadData     (uint);
   virtual void            Flush        ();

  private:
    TreeRender*         mRender;        
    TreePrecompile&     mPrecompile;
    int                 mFirstText;
};

////////////////////////////////////////////////////////////////////////////////
///��� ���設 ��� ��ॢ쥢
////////////////////////////////////////////////////////////////////////////////
class   TreeVertexCache: public VertexCache
{
  public:
                        TreeVertexCache (Pool*,
                                         VertexBuffer*,
                                         TreePrecompile&,
                                         int vertexCount
                                        );
       virtual          ~TreeVertexCache ();

  protected:
    virtual vcnode_t*   LoadData        (int index); 

  private:
    TreePrecompile&     mPrecompile;
};

#endif