#include "pch.h"
#include "r_treeenv.h"
#include "r_main.h"
#include "g_loader.h"

void       TreeEnv::SetSBuffer       (const SBuffer* sb)
{
  mRender.SetSBuffer ((SBuffer*)sb);
}

void       TreeEnv::SetFrustum       (const frustum_t& f)
{
  mRender.SetFrustum (f);
}

void       TreeEnv::SetCamera        (const Camera& cam)
{
  mRender.SetCamera (cam);
}

void       TreeEnv::bind             (MainRender* render)
{
  mRender.bind (render);  
}

void       TreeEnv::insert           (int x,int y,int tile,int detail)
{
  D_Try
  {
    mRender.AddTile (x,y,detail);
  }
  D_Exc ("TreeEnv: Error at insert");
}

void       TreeEnv::reset            ()
{  }

void       TreeEnv::render           ()
{
  mRender.Draw ();
//  mKeeper.mTreeMap.reset ();
}
                                                   
//TreeEnv::TreeEnv  (Pool* _Pool,LevelLoader& loader,size_t _MeshCacheSize,uint _VertexCacheSize)
TreeEnv::TreeEnv  (Pool* _Pool,MainRender* mr,size_t _MeshCacheSize,uint _VertexCacheSize)
        : MemObject (_Pool),
          mLoader (mr->GetLoader()),
          mKeeper (_Pool,mr->GetLoader(),_MeshCacheSize,_VertexCacheSize),
//          mKeeper (_Pool,loader,RENDER_TREES_MESH_CACHE_SIZE,RENDER_TREES_VERTEX_CACHE_SIZE),
          mRender (_Pool,mr,*this)
{
  if (!IsValid (&mKeeper) || !IsValid (&mRender))
  {
    D_Message ("TreeEnv: No enough memory for sys data");
    return;
  }

  mKeeper.mTreeMeshCache.BindRender (&mRender);
}

TreeEnv::~TreeEnv ()
{
}