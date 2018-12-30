#include "pch.h"
#include "r_matrender.h"
#include "r_main.h"

#include <gl\gl.h>
#include <graph\glext.h>

struct   MaterialRender::meshnode_t
{
  int           tile;           //Индекс тайла
  DWORD         param;
  mesh_t*       mesh;           //Указатель на мэш
  meshnode_t*   next;           //Следующая запись
};

struct  MaterialRender::matnode_t
{
  int               next;       //Следующий материал
  meshnode_t*       firstMesh;  //Первый описатель мэша
  int               num;        //Количество описателей
};  

BOOL   MaterialRender::SetMaterial     (int mat,int mode)
{
  if (mMatReindex)
  {
    if (mode & RENDER_MAT_SET)    mMatEnv->SetMat (mMatReindex [mat-mFirstMat]); 
    if (mode & RENDER_TEXT_SET)   mMatEnv->bind   (mMatReindex [mat-mFirstMat]);
    else                          mMatEnv->unbind ();

    return TRUE;
  }
  
  return FALSE;  
}

matdesc_t&   MaterialRender::GetMaterial     (int mat)
{
  if (mMatReindex)  return mMatEnv->GetMat (mMatReindex [mat-mFirstMat]);
  else 
  {
    static matdesc_t error;
    return error;
  }
}

void   MaterialRender::ReindexMat      ()
{
  if (mMatReindex) free (mMatReindex); 
  mMatReindex = NULL;
  mMatEnv     = mainrender ()->GetMatEnvX ();
}

void   MaterialRender::Draw  ()
{
  D_Try
  {
    for (int mindex=mMatFirst;mindex != -1;mindex = mMat [mindex].next)
    {
      SetMaterial (mindex+mFirstMat);

      for (meshnode_t* mnode = mMat [mindex].firstMesh;mnode;mnode=mnode->next)
      {
        ChangeEnv     (mnode->tile);
        ChangeState   (mnode->param);
        DrawMesh      (mnode->mesh);
      }

      mMat [mindex].firstMesh = NULL;
      mMat [mindex].num       = 0;
    }

    Flush ();
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    D_Message ("MaterialRender: Error at Draw");

    mMeshPos  = 0;
    mMatFirst = -1;
  }
}

void     MaterialRender::Flush           ()
{
  D_Try
  {
    mMeshPos  = 0;
    mMatFirst = -1;

    reset ();
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    D_Message ("MaterialRender: Error at Flush");

    mMeshPos  = 0;
    mMatFirst = -1;
  }  
}

void  MaterialRender::AddMesh  (mesh_t* mesh,int* tiles,int cnt,DWORD param)
{
  for (int i=0;i<cnt;i++,mesh=(mesh_t*)((char*)mesh+mesh->size))
  {
    if (GetMaterial (mesh->material).color.blend)
      continue;

    if (mMeshPos >= mListNum)
      Draw ();

    int         mtr   = -mFirstMat + mesh->material;
    matnode_t&  mat   = mMat [mtr];  //include -1 material
    meshnode_t& mnode = mMeshNode [mMeshPos++];
    
    if (mat.firstMesh == NULL) 
    {
      mat.next        = mMatFirst;
      mMatFirst       = mtr;
      mat.firstMesh   = &mnode;
      mnode.tile      = tiles ? tiles [i] : -1;
      mnode.mesh      = mesh;
      mnode.next      = NULL;
      mnode.param     = param;
    }
    else
    {          
      mnode.tile      = tiles ? tiles [i] : -1;
      mnode.mesh      = mesh;
      mnode.next      = mat.firstMesh;
      mnode.param     = param;
      mat.firstMesh   = &mnode;
    }

    mat.num++;
  }  
}

void  MaterialRender::AddMesh  (mesh_t* mesh,int tile,int cnt,DWORD param)
{
  for (int i=0;i<cnt;i++,mesh=(mesh_t*)((char*)mesh+mesh->size))
  {
    if (GetMaterial (mesh->material).color.blend)
      continue;

    if (mMeshPos >= mListNum)
      Draw ();

    int         mtr   = -mFirstMat + mesh->material;
    matnode_t&  mat   = mMat [mtr];  //include -1 material
    meshnode_t& mnode = mMeshNode [mMeshPos++];
    
    if (mat.firstMesh == NULL) 
    {
      mat.next        = mMatFirst;
      mMatFirst       = mtr;
      mat.firstMesh   = &mnode;
      mnode.tile      = tile;
      mnode.mesh      = mesh;
      mnode.next      = NULL;
      mnode.param     = param;
    }
    else
    {          
      mnode.tile      = tile;
      mnode.mesh      = mesh;
      mnode.next      = mat.firstMesh;
      mnode.param     = param;
      mat.firstMesh   = &mnode;
    }

    mat.num++;
  }  
}

void  MaterialRender::DrawShadow  (mesh_t* mesh,int count,DWORD param)
{
  D_Try
  {
    for (int i=0;i<count;i++,mesh=(mesh_t*)((char*)mesh+mesh->size))
    {
      int           count = mesh->count;

      ChangeState         (param);
      DrawMesh            (mesh);

      mPgCount           += count / 3;
    }
  }
  D_Exc ("LandMeshRender:: Error at DrawShadow");
}

MaterialRender::MaterialRender  (Pool* _Pool,MainRender* mr,int _MatNum,int _MeshNum)
               : Render       (_Pool,mr),
                 mListNum     (_MeshNum),
                 mMatNum      (_MatNum+1),
                 mMeshNode    (NULL),
                 mMat         (NULL),
                 mMatFirst    (-1),
                 mMeshPos     (0),
                 mFirstMat    (-1),
                 mMatReindex  (NULL), mMatEnv (NULL), mMatGroup (-1)
{
  D_Try
  {
    mMeshNode  = (meshnode_t*)alloc  (mListNum * sizeof (meshnode_t));
    mMat       = (matnode_t*)alloc   (mMatNum * sizeof (matnode_t));

    if (!mMeshNode || !mMat)
    {
      D_Message ("LandMeshRender: No enough memory (alloc sys data)");
      return;
    }    

    memset  (mMat,0,mMatNum * sizeof (matnode_t));
  }
  D_Exc ("LandMeshRender: Error at construct");
}

MaterialRender::~MaterialRender ()
{
  D_Try
  {
    if (mMat)          free (mMat);
    if (mMeshNode)     free (mMeshNode);
    if (mMatReindex)   free (mMatReindex);
  }
  D_Exc ("LandMeshRender: Error at destruct");
}
