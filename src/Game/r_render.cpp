#include "pch.h"
#include "r_render.h"
#include "r_main.h"
#include <gl\gl.h>
#include <graph\glext.h>

BOOL   Render::ChangeEnv       (int tile,uint intrd)
{
  return mMainRender->ChangeEnv   (tile,intrd);
}

BOOL   Render::SetMaterial     (int mat,int mode)
{
  return mMainRender->SetMaterial (mat,mode);
}

void   Render::AddPredraw      ()
{
  mMainRender->AddPredraw (this);
}

void   Render::DelPredraw      ()
{
  mMainRender->DelPredraw (this);
}

void   Render::AddAlpha        (float dist,DWORD param)
{
  mMainRender->mAlphaRender->AddAlpha (dist,param,this);
}

void   Render::DrawMesh  (mesh_t* mesh)
{
  SetVertexBuffer     (mesh);

  glDrawRangeElements (mesh->type,
                       mesh->start,
                       mesh->end,
                       mesh->count,
                       mesh->elType,
                       mesh->data
                      );    

  mPgCount += mesh->count / 3;
}

Render::Render  (Pool* pool,MainRender* mr)
       : MemObject   (pool), 
         mMainRender (mr),
         mPgCount    (0), mSBuffer (NULL)
{    }