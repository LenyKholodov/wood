#include "pch.h"
#include "r_treerender.h"
#include "r_treeenv.h"
#include "r_treekeeper.h"
#include "g_loader.h"
#include "r_main.h"

#include <gl\gl.h>

void      TreeRender::ReindexMat      (MatDictionary& dict)
{
  MaterialRender::ReindexMat ();  

  if (!dict.count())
    return ;

  int   count  = dict.count ();
  mMatReindex  = (int*)alloc (sizeof (int) * count);

  for (int i=0;i<count;i++)
    mMatReindex [i] = mMatEnv->SearchMat (dict.GetMaterial (i));

  mMatGroup = mMatEnv->CreateMatGroup (mMatReindex,dict.count(),RENDER_TREES_TEXT_CACHE);
}

void      TreeRender::AddTile     (int x,int y,int detail)
{
  int grnode = y*mWidth+x;

  for (treemapnode_t* node = mTreeMap.get (x,y);node;node=node->next)
  {
     //check for bound and sbuffer           

    multimesh_t* mmesh = (multimesh_t*)mTreeCache->lock (node->tree.tree);
    
    if (mmesh)
    {
      mesh_t* mesh  = mmesh->mesh();    
      int     count = mmesh->num;

      for (int i=0;i<count;i++,mesh=(mesh_t*)((char*)mesh+mesh->size))
        if (GetMaterial (mesh->material).color.blend)
        {
          AddAlpha (length (node->tree.pos-mCamera.pos()),(DWORD)node); 
          break;
        }  

      AddMesh (mmesh->mesh(),grnode,mmesh->num,(DWORD)node);
    }  

    mTreeCache->unlock (node->tree.tree);
  }
}

void         TreeRender::DrawAlpha       (DWORD param,const Camera&)
{
  treemapnode_t* node  = (treemapnode_t*)param;
  multimesh_t*   mmesh = (multimesh_t*)mTreeCache->lock (node->tree.tree);  
  
  if (mmesh)
  {
    mesh_t* mesh  = mmesh->mesh();
    int     count = mmesh->num;

    for (int i=0;i<count;i++,mesh=(mesh_t*)((char*)mesh+mesh->size))
      if (GetMaterial (mesh->material).color.blend)
      {
        SetMaterial  (mesh->material);
        glPushMatrix ();
        glTranslatef (node->tree.pos.x,node->tree.pos.y,node->tree.pos.z);
        glScalef     (mScale.x,mScale.y,mScale.z);
//        ChangeEnv    ();  ??
        DrawMesh     (mesh);
        glPopMatrix  ();
      }  
  }

  mTreeCache->unlock (node->tree.tree);
}

void         TreeRender::Draw            ()
{
  glPushMatrix         ();
  MaterialRender::Draw ();
  glPopMatrix          ();
}

BOOL         TreeRender::ChangeState     (DWORD param)
{
  glPopMatrix ();
  glPushMatrix ();  

  treemapnode_t* node = (treemapnode_t*)param;  

  glTranslatef (node->tree.pos.x,node->tree.pos.y,node->tree.pos.z);
  glScalef     (mScale.x,mScale.y,mScale.z);

  return TRUE;
}

void         TreeRender::SetVertexBuffer (mesh_t* mesh)
{
  mVertexCache->bind (mesh->vbuffer);
/*  int first = mVertexCache->set (mesh->vbuffer);

  if (mesh->index != first)
  {
    int    offs   = first - mesh->index;
    uint*  index  = (uint*)mesh->data;
    int    count  = mesh->count;

    for (int i=0;i<count;i++,*index+++=offs);

    mesh->index  = first;
    mesh->start += offs;
    mesh->end   += offs;
  }*/
}

void         TreeRender::reset           ()
{
  mTreeMap.reset    ();
  mTreeCache->reset ();
}

TreeRender::TreeRender  (Pool* _Pool,MainRender* mr,TreeEnv& env)
           : MaterialRender (_Pool,mr,env.mKeeper.mTreePrecompile.GetMatCount (),RENDER_TREES_IN_FRAME),
             mTreeMap   (env.mKeeper.mTreeMap),
             mTreeCache (&env.mKeeper.mTreeMeshCache),
             mVertexCache (&env.mKeeper.mTreeVertexCache),
             mWidth (env.mKeeper.mLandMap->width),
             mHeight (env.mKeeper.mLandMap->height),
             mFirstTexture (0),
             mScale (1)
{   
  if (mTreeCache) mTreeCache->SetFirstText (0);

  SetFirstMat (0);

  ReindexMat  (env.mKeeper.mTreePrecompile.GetMatDict ());
}

TreeRender::~TreeRender ()
{   
  if (mMatGroup != -1) mMatEnv->DeleteMatGroup (mMatGroup);
}
