#include "pch.h"
#include "r_landscape.h"
#include "r_vbuffer.h"
#include "r_sbuffer.h"
#include "g_lndcache.h"
#include "g_loader.h"
#include "r_main.h"

#include <misc.h>
#include <graph\utils.h>
#include <string.h>

#include <graph\glext.h>

/////////////////////////////interface for vertex buffer cache class////////////

class        LandVertexCache: public VertexCache
{
  public:
             LandVertexCache   (Pool*,VertexBuffer*,GameLevel*,uint _VBCacheSize);
     virtual ~LandVertexCache  ();

     virtual bool valid      () const { return VertexCache::valid () && IsValid (mFile); }

  protected:
   virtual    vcnode_t* LoadData        (int index);
   static     uint      LoadHeader      (BinFile*);

  protected:
        BinFile*        mFile;
        ulong           mOffs;
}; 


/////////////////////implementation of class LandMeshRender/////////////////////

uint         LandMeshRender::GetMatCount  (MainRender* mr)
{
  BinFile* file = mr->GetLoader().level.CreateLump (LL_LAND_MAT,GameLevel::OPEN_RO);

  if (!IsValid (file))
  {
    D_Message ("LandMeshRender: No chunk with land material names");
    return 1;
  }

  landmatheader_t hdr;

  file->read (&hdr,sizeof (hdr));

  delete file;

  return hdr.count + 1;
}

void         LandMeshRender::ReindexMat      ()
{
  D_Try
  {
    MaterialRender::ReindexMat ();

    BinFile* file = mainrender()->GetLoader().level.CreateLump (LL_LAND_MAT,GameLevel::OPEN_RO);

    if (!IsValid (file))
    {
      D_Message ("LandMeshRender: No chunk with land material names");
      return;
    }

    landmatheader_t hdr;

    file->read (&hdr,sizeof (hdr));

    char*       str = (char*)alloc (hdr.strsize);
    mMatReindex     = (int*)alloc  ((hdr.count + 1)* sizeof (int));

    if (!str || !mMatReindex)
    {
      delete file;
      if (str) free (str);
      if (mMatReindex) free (mMatReindex);
      return;
    }

    file->read (str,hdr.strsize);

    char*    x   = str;

    for (int i=0;i<hdr.count+1;i++,x+=strlen(x)+1)
      mMatReindex [i+1] = mMatEnv->SearchMat (x);
    mMatReindex [0] = mMatReindex [hdr.count];
    
    delete file;
    free (str);  

         //create material group

//    mMatGroup       = mMatEnv->CreateMatGroup (mMatReindex+1,hdr.count-1,RENDER_LAND_TEXT_CACHE);
    mMatGroup       = mMatEnv->CreateMatGroup (mMatReindex,hdr.count,RENDER_LAND_TEXT_CACHE);
  }
  D_Exc ("LandMeshRender: Error at ReindexMat");
} 

void         LandMeshRender::AddTile         (int x,int y,int grnode,int detail)
{
  D_Try
  {
    const grtile_t& tile     = mMap->tile (grnode);
    int             count    = tile.node.chunk [GRC_STRIP].count;
    mesh_t*         mesh     = (mesh_t*)mCache->lock (tile,GRC_STRIP,this); 

    if (count && mesh)  
      AddMesh (mesh,tile.index,count); //!!!!!
  }
  D_Exc ("LandMeshRender: Error at AddTile");
} 

void         LandMeshRender::AddTile          (visnode_t* list,int count)
{
  D_Try
  {
    for (int i=0;i<count;i++,list++)      
      AddTile (list->x,list->y,list->index,list->detail);
  }
  D_Exc ("LandMeshRender: Error at Draw");
}

void         LandMeshRender::DrawShadow      (const frustum_t& frustum,visnode_t* list,int count)
{
  D_Try
  {
    for (int i=0;i<count;i++,list++)      
    {
//      boundbox_t& box = mMap->map [list->index].box;

//      if (IsInBound (box,6,frustum.clip))
      {
        const grtile_t& tile     = mMap->tile (list->index);
        int             num      = tile.node.chunk [GRC_STRIP].count;
        mesh_t*         mesh     = (mesh_t*)mCache->get (tile,GRC_STRIP,this);

        if (mesh)        
          MaterialRender::DrawShadow (mesh,num);
      }
    }
  }
  D_Exc ("LandMeshRender:: Error at DrawShadow");
}

void LandMeshRender::reset ()
{
  mCache->reset ();
}

void LandMeshRender::Draw  ()
{
  if (mFog)
  {
    glEnableClientState (GL_FOG_COORDINATE_ARRAY_EXT);
    float fogColor[4] = {0.8f, 0.8f, 0.8f, 1.0f};

//    glEnable(GL_FOG);             
    glFogi(GL_FOG_MODE, GL_LINEAR); 
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_START, 0.0);   
    glFogf(GL_FOG_END, 50.0);  

    glFogi(GL_FOG_COORDINATE_SOURCE_EXT, GL_FOG_COORDINATE_EXT);

  }  

  MaterialRender::Draw  ();

  glDisableClientState (GL_FOG_COORDINATE_ARRAY_EXT);
}

void LandMeshRender::SetVertexBuffer (mesh_t* mesh)
{
  int     count = 0;
  int     base  = mVertexCache->bind  (mesh->vbuffer,&count);

  if (!mFog || count > mFogCount)
  {
    float* fog = (float*)alloc (sizeof (float) * (mFogCount = count));

    if (!fog)
    {
      glDisableClientState (GL_FOG_COORDINATE_ARRAY_EXT);
      return;
    }  

    if (!mFog)
      glEnableClientState    (GL_FOG_COORDINATE_ARRAY_EXT);
    else
      free (mFog);

    glFogCoordPointerEXT   (GL_FLOAT,0,mFog = fog);
  }

  if (mCurBase != base)
  {
    mCurBase         = base;
    vertex_t* vertex = mVertexCache->vertex (base);
    float*    fog    = mFog;

    for (int i=0;i<count;i++,vertex++,fog++)
      *fog = vertex->x * vertex->z * 0.0005;
//      *fog = vertex->y * 0.2;
  }

//  if (mCurBase != base)
//    mCurBase = base;

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

LandMeshRender::LandMeshRender  
 ( Pool*          _Pool,
   MainRender*    _MRender,
   int            _ListNum
 )
               : MaterialRender (_Pool,_MRender,GetMatCount (_MRender),_ListNum),
                 mMap           (_MRender->GetLoader ()->mLandMap),
                 mCache         (_MRender->GetLoader ()->mLandMap->cache ()),
                 mVertexCache  (NULL),
                 mFog          (NULL),
                 mFogCount     (0), mCurBase (-1)
{
  D_Try
  {
    ReindexMat ();

    mVertexCache = ext_new
                    LandVertexCache (GetPool(),
                                     _MRender->GetLoader ()->mVertexBuffer,
                                     &_MRender->GetLoader ().level,
                                     256000
                                    );

    if (!IsValid (mVertexCache))
    {
      D_Message ("LandMeshRender: No enough memory for vertex buffer")
      return;
    }
  }
  D_Exc ("LandMeshRender: Error at construct");
}

LandMeshRender::~LandMeshRender ()
{
  D_Try
  {
    if (mVertexCache)    delete mVertexCache;
    if (mFog)            free (mFog);
    if (mMatGroup != -1) mMatEnv->DeleteMatGroup (mMatGroup);
  }
  D_Exc ("LandMeshRender: Error at destruct");
}

////////////////implementation of special VertexCache////////////////////////////

LandVertexCache::vcnode_t* LandVertexCache::LoadData        (int index)
{
  D_Try
  {
    vertexbuffer_t vb;
    ulong          offs = 0;

    mFile->seek (mOffs + sizeof (ulong) * index);
    mFile->read (&offs,sizeof (ulong));
    mFile->seek (offs);
    mFile->read (&vb,sizeof (vertexbuffer_t));

    vcnode_t* node = AllocChunk (vb.count);

    if (!node)
      return NULL;

    vb.pos         = 0;
    vb.count       = node->vb->count;
    vb.first       = node->vb->base;
    vb.vertex      = node->vb->vertex;
    vb.normal      = node->vb->normal;
    vb.tcoord      = node->vb->tcoord;

    mFile->read (vb.vertex,sizeof (vertex_t) * vb.count);
    mFile->read (vb.normal,sizeof (vector3d_t) * vb.count);
    mFile->read (vb.tcoord,sizeof (textcoord_t) * vb.count);

    return node;
  }
  D_ExcRet ("LandVertexCache: Error at LoadData",NULL);
}

uint      LandVertexCache::LoadHeader      (BinFile* file)
{
  if (!IsValid (file))
  {
    D_Message ("LandVertexCache: Error at LoadHeader");
    return 0;
  }

  vbheader_t hdr;

  file->read (&hdr,sizeof (hdr));  

  delete file;

  return hdr.count;
}

LandVertexCache::LandVertexCache  (Pool* _Pool,VertexBuffer* vb,GameLevel* level,uint _VBCacheSize)
               : VertexCache  (_Pool,vb,_VBCacheSize,
                               LoadHeader (level->CreateLump (LL_VERTEX_BUFFER,GameLevel::OPEN_RO))
                              ),
                 mOffs        (0),
                 mFile        (level->CreateLump (LL_VERTEX_BUFFER,GameLevel::OPEN_RO))
{
  D_Try
  {
    vbheader_t hdr;

    mFile->read (&hdr,sizeof (hdr));

    mOffs   = hdr.offs;
  }
  D_Exc ("LandVertexCache: Error at construct");
}

LandVertexCache::~LandVertexCache  ()
{
  if (mFile) delete mFile;  
} 
