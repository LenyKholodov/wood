#include "pch.h"
#include "cv_grmap.h"
#include "cv_utils.h"

#include "g_level.h"

#include <graph\striper.h>
#include <graph\utils.h>
#include <gl\gl.h>
#include <string.h>

#define ConMessage(x) 
#define RetMessage(x) 
#define ClearMessage
#ifdef CONSOLE_MESSAGES
  #undef  ConMessage       
  #undef  RetMessage       
  #undef  ClearMessage
  #define ConMessage(x)  {cout<<x<<endl;dout<<x<<endl;}
  #define ClearMessage  { cout<<"\r"<<endl; for (int i=0;i<79;i++) cout<<'\r'; }
  #define RetMessage(x)  cout<<"\r"<<x;
#endif

////////////////////implementation of class GrMapConv///////////////////////////

struct  mapnode_t
{
  FacetRing     fring;
  PolyRing      pring;
  float         height;
  boundbox_t    box;
  int           vbuffer;
};

static int   mod3 [6] = {0,1,2,0,1,2};

struct  point_t
{
  float x,y;
};

float   outCode (float x,float y,float x1,float y1,float x2,float y2)
{
  int code = 0;

  if (x < x1) code |= 1;
  if (y < y1) code |= 2;
  if (x > x2) code |= 4;
  if (y > y2) code |= 8;

  return code;
}

void    GrMapConv::calcCross  (int findex,float step)
{
  const facet_t& facet = mFacet [findex];
  const vertex_t v [3] = { mVertex [facet.index[0]],
                           mVertex [facet.index[1]],
                           mVertex [facet.index[2]]
                         };

  const point_t  p [3] = { {v [0].x / step, v [0].z / step},
                           {v [1].x / step, v [1].z / step},
                           {v [2].x / step, v [2].z / step}
                         };        
  float minx = p [0].x,
        miny = p [0].y,
        maxx = p [0].x,
        maxy = p [0].y;

  for (int i=1;i<3;i++)
  {
    if      (p [i].x < minx) minx = p [i].x;
    else if (p [i].x > maxx) maxx = p [i].x;
    if      (p [i].y < miny) miny = p [i].y;
    else if (p [i].y > maxy) maxy = p [i].y;
  }

  int   iminx = minx,
        iminy = miny,
        imaxx = maxx,
        imaxy = maxy;

  if (iminy == imaxy && iminx == imaxx)
  {
    mNode [iminy*mWidth+iminx].fring.insert (findex);    
    return;
  }

  for (int y=iminy;y<=imaxy;y++)
  {
    int     pos = y * mWidth + iminx;
    float   xf  = float (iminx),
            yf  = float (y);

    for (int x=iminx;x<=imaxx;x++,pos++,xf+=step)
    {
      for (int i=0;i<3;i++)
      {
        int code1   = outCode (p [mod3 [i]].x,p [mod3 [i]].y,x,y,x+1,y+1);
        int code2   = outCode (p [mod3 [i+1]].x,p [mod3 [i+1]].y,x,y,x+1,y+1);

        if ((code1 & code2) == 0) 
        {
          mNode [pos].fring.insert (findex);
          break;
        }       
      }  
    }
  }        
} 

void    GrMapConv::compile  ()
{
  boundbox_t box;
  float      w;
  float      minHeight;

      //Вычисляем описывающий ящик и приводим его к виду box.min = vec3 (0)

  calcBound (box,w);

  for (int i=0;i<mVCount;i++)
    mVertex [i]    -= box.min;

  box.max   -= box.min;
  box.min   -= box.min;

      //просчёт параметров             

  w         /= float(mGrad);
  minHeight  = box.min.y;
  mWidth     = (box.max.x - box.min.x) / w + 1;
  mHeight    = (box.max.z - box.min.z) / w + 1;
  mSize      = mWidth * mHeight;

     //тайловое разбиение сцены             

  mNode      = new mapnode_t [mSize];

  ConMessage ("]Calc cross width net...");

  for (i=0;i<mFCount;i++)
    calcCross (i,w);

  int index = 0;

  for (int y=0;y<mHeight;y++)
  {
    float yf = float(y)*w,
          xf = 0;
          
    for (int x=0;x<mWidth;x++,xf+=w,index++)
    {
      boundbox_t& box = mNode [index].box;

      buildBound (index,box,mNode [index].height);
      mNode [index].height -= minHeight;    

      box.min.x            = xf;
      box.min.z            = yf;
      box.max.x            = xf + w;
      box.max.z            = yf + w;
    }
  }

  box.max.x  = float(mWidth)*w;
  box.max.z  = float(mHeight)*w;

  mBox       = box;
  mStep      = w;

  int size   = ConvertArrays (mVCount,&mVertex,&mNormal,&mTextCoord,
                              mFacet,mFCount,
                              GetPool(),
                              TRUE
                             );

  ConMessage ("]Add "<<size-mVCount<<" vertexes ");

  mVCount        = size;
  int* vbmirror  = (int*)alloc (sizeof (int) * mVCount);

  if (vbmirror)
  {
    if (!mVBCacheWidth)  mVBCacheWidth  = mWidth;
    if (!mVBCacheHeight) mVBCacheHeight = mHeight;

    for (y=0;y<mHeight;y+=mVBCacheHeight)
    {
      for (int i=0;i<mVCount;vbmirror [i++] = -1);

      int ye = (y+mVBCacheHeight < mHeight) ? y+mVBCacheHeight : mHeight-1;

      for (int x=0;x<mWidth;x+=mVBCacheWidth)
      {
        int xe = (x+mVBCacheWidth < mWidth) ? y+mVBCacheWidth : mWidth-1;

        SplitVBLump (x,y,xe,ye,vbmirror);
      }
    }

    free (vbmirror);
  }
  else
    D_Message ("GrMapConv: No enough memory for create vertex buffers");
}

void    GrMapConv::SplitVBLump (int x1,int y1,int x2,int y2,int* mirror)
{
  int  vcount  = 0;    
  int  vbindex = mVBuffer.count ();

  for (int y=y1;y<=y2;y++)
  {
    for (int x=x1;x<=x2;x++)
    {
      int                 tile   = y * mWidth + x;
      FacetRing::iterator iter   = mNode [tile].fring;
      int                 count  = mNode [tile].fring.count ();

      mNode [tile].vbuffer       = vbindex;

      for (int i=0;i<count;i++,iter++)
      {
        facet_t facet = mFacet [iter.data()];

        for (int j=0;j<3;j++)   
        {
          if (mirror [facet.index [j]] == -1)
            mirror [facet.index [j]] = vcount++;

          facet.index [j] = mirror [facet.index [j]];
        }

        mNode [tile].pring.insert (facet);
      }
    }
  } 

  if (vcount)
  {
    vertexbuffer_t vbuffer;

    vbuffer.count   = vbuffer.pos = vcount;
    vbuffer.first   = 0;
    vbuffer.vertex  = (vertex_t*)alloc (sizeof (vertex_t) * vcount);
    vbuffer.normal  = (vector3d_t*)alloc (sizeof (vector3d_t) * vcount);
    vbuffer.tcoord  = (textcoord_t*)alloc (sizeof (textcoord_t) * vcount);

    if (!vbuffer.vertex || !vbuffer.normal || !vbuffer.tcoord)
    {  
      if (vbuffer.vertex) free (vbuffer.vertex);
      if (vbuffer.normal) free (vbuffer.normal);
      if (vbuffer.tcoord) free (vbuffer.tcoord);

      vbuffer.vertex = NULL;
      vbuffer.normal = NULL;
      vbuffer.tcoord = NULL;

      D_Message ("GrMapConv: No enough memory for vertex buffer");

      for (int y=y1;y<=y2;y++)
      {
        int pos = y * mWidth;
        for (int x=x1;x<=x2;x++)
          mNode [pos + x].vbuffer = -1;
      }    

      return;
    }

    for (int i=0;i<mVCount;i++)
      if (mirror [i] != -1)
      {
        vbuffer.vertex [mirror [i]] = mVertex    [i];
        vbuffer.normal [mirror [i]] = mNormal    [i];
        vbuffer.tcoord [mirror [i]] = mTextCoord [i];
      }

    mVBuffer.insert (vbuffer);
  }
}

void    GrMapConv::calcBound   (boundbox_t& box,float& w)
{
  vector3d_t c (.0f);
  vector3d_t dim (.0f);

  for (int i=0;i<mVCount;i++)
    c += mVertex [i];

  c /= float (mVCount);

  for (i=0;i<mVCount;i++)
  {
    vector3d_t d = abs (c - mVertex [i]);

    if (d.x > dim.x) dim.x = d.x;
    if (d.y > dim.y) dim.y = d.y;
    if (d.z > dim.z) dim.z = d.z;
  }    

  box.min     = c - dim;
  box.max     = c + dim;
  w           = dim.x < dim.z ? dim.x * 2.0f : dim.z * 2.0f;
}

void    GrMapConv::buildBound (int index,boundbox_t& box,float& height)
{
     //Не совсем корректное построение
     //Нужно производить отсечение

  FacetRing&          fring = mNode [index].fring;
  FacetRing::iterator iter  = fring;
  int                 count = fring.count ();
  vector3d_t          c (.0f);
  vector3d_t          dim (.0f);

  for (int i=0;i<count;i++,iter++)
    for (int j=0;j<3;j++)
      c += mVertex [mFacet [iter.data ()].index [j]];
 
  c /= float (count * 3);

  for (i=0;i<count;i++,iter++)
    for (int j=0;j<3;j++)
    {
      vector3d_t d = abs (c - mVertex [mFacet [iter.data ()].index [j]]);

      if (d.x > dim.x) dim.x = d.x;
      if (d.y > dim.y) dim.y = d.y;
      if (d.z > dim.z) dim.z = d.z;
    }  
  
  box.min   = c - dim;
  box.max   = c + dim;
  height    = c.y - dim.y;  
}

void   GrMapConv::convert (Object3D& obj)
{  
  D_Try
  {
    ConMessage ("]Convert from 3DS...");

    mObj3d = &obj;

    if (!ConvertObject3D (obj,
                     GetPool (),
                     mVCount,&mVertex,
                     mFCount,&mFacet,
                     mNCount,&mNormal
                    ))
    {
      cout<<"GrMapConv: Error at convert model"<<endl;
      return;
    }    

    ConMessage ("Total info:");
    ConMessage ("\t\tVertexes :\t"<<mVCount);
    ConMessage ("\t\tFaces    :\t"<<mFCount);
    ConMessage ("\t\tMaterials:\t"<<obj.count (Object3D::MATERIAL));
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {           
    D_Message ("GrMapConv: Error at convert (const Object3D&)");

    if (mNormal)    free (mNormal);
    if (mTextCoord) free (mTextCoord);
    if (mVertex)    free (mVertex); 
    if (mFacet)     free (mFacet);

    mFacet      = NULL;
    mVertex     = NULL;
    mNormal     = NULL;
    mTextCoord  = NULL;
  }
}

int    GrMapConv::computeFacets (mapnode_t& mapnode,char* data,nodeinfo_t& node,int* material)
{
  PolyRing::iterator  iter  = mapnode.pring;
  int                 count = mapnode.pring.count ();
  int                 mat   = 0;

  facetpatch_t* patch       = (facetpatch_t*)data;
  facet_t*      f           = (facet_t*)patch->data;

  patch->size               = sizeof (facetpatch_t) + count * sizeof (facet_t);
  patch->vbuffer            = mapnode.vbuffer;
  patch->count              = count;

  if (mapnode.vbuffer == -1 || patch->size > TEMP_DATA_SIZE)
  {
    node.chunk [GRC_FACET_PATCH].offs   = 0;
    node.chunk [GRC_FACET_PATCH].count  = 0; 
    node.chunk [GRC_FACET_PATCH].first  = 0;
    node.chunk [GRC_FACET_PATCH].size   = 0; 

    if (patch->size > TEMP_DATA_SIZE)
      dout<<"GrMapConv: size > TEMP_DATA_SIZE. Call to DadyCool"<<endl;

    return -1;
  }  

  for (int i=0;i<count;i++,iter++,f++)
  {
    facet_t& facet = *f = iter.data ();

    for (int j=mat;j && facet.texture!=material [j-1];j--);

    if  (!j)
    {
      if (mat < 0x10000)  material [mat++] = facet.texture;
      else    
        dout<<"GrMapConv: Too many materials in 1 tile. Call to DadyCool "<<endl; 
    } 
  }  

  node.chunk [GRC_FACET_PATCH].offs   = 0;
  node.chunk [GRC_FACET_PATCH].count  = 1; 
  node.chunk [GRC_FACET_PATCH].first  = 0;
  node.chunk [GRC_FACET_PATCH].size   = patch->size; 

  return mat;
}

size_t  GrMapConv::buildNode  
 ( char*        data,
   nodeinfo_t&  node,
   mapnode_t&   mapnode,
   int&         meshIndex
 )
{
  static int          material [0x10000];
  int                 mat       = computeFacets (mapnode,data,node,material);
  size_t              size      = node.chunk [GRC_FACET_PATCH].size;  

  if (mat == -1)
  {
    node.chunk [GRC_STRIP].offs    = 0;
    node.chunk [GRC_STRIP].first   = 0;
    node.chunk [GRC_STRIP].count   = 0;
    node.chunk [GRC_STRIP].size    = 0;    

    return 0;
  }

             //Compute material-meshes

  PolyRing::iterator iter      = mapnode.pring;
  int                count     = mapnode.pring.count ();

  size_t             meshSize  = 0;
  int                fstMesh   = meshIndex;
  mesh_t*            mesh      = (mesh_t*)(data + size);

  if (count)
  {
    for (int i=0;i<mat;i++)
    {
      PolyRing     src (GetPool());

      if (size + sizeof (mesh_t) > TEMP_DATA_SIZE) //!!!!
      {
        dout<<"GrMapConv: size < TEMP_DATA_SIZE. Call to DadyCool"<<endl;
        return 0;
      }
 
      for (int j=0;j<count;j++,iter++)
        if (iter.data ().texture == material [i])
          src.insert (iter.data ());

      if (!CreateMesh (mesh,src,mHWVertexCacheSize,mStripEnable,TRUE,GetPool()))
      {
        mesh->type      = GL_TRIANGLES;
        mesh->material  = -1;
        mesh->index     = 0;
        mesh->count     = 0;
        mesh->size      = sizeof (mesh_t);
        mesh->vbuffer   = mapnode.vbuffer;

        D_Message ("GrMapConv: Error at CreateMesh");
      }

      mesh->material  = material [i];

      meshSize       += mesh->size;
      size           += mesh->size;

      meshIndex++;

      mesh            = (mesh_t*)((char*)mesh+mesh->size);
    }
  }
  else
  {
    if (size + sizeof (mesh_t) > TEMP_DATA_SIZE)
    {
      dout<<"GrMapConv: size < TEMP_DATA_SIZE. Call to DadyCool"<<endl;
      return 0;
    }

    mesh->type      = GL_TRIANGLES;
    mesh->material  = -1;
//    mesh->index     = -1;
    mesh->index     = 0;
    mesh->count     = 0;
    mesh->size      = sizeof (mesh_t);
    mesh->vbuffer   = -1;

    meshSize       += mesh->size;    
    size           += mesh->size;
  }

  node.chunk [GRC_STRIP].offs    = node.chunk [GRC_FACET_PATCH].size;
  node.chunk [GRC_STRIP].first   = count ? fstMesh : -1;
  node.chunk [GRC_STRIP].count   = mat;
  node.chunk [GRC_STRIP].size    = meshSize;

  return size;
}

void    GrMapConv::saveMap    (GameLevel* level)
{
  nodeinfo_t*   nodeInfo  = new nodeinfo_t [mSize];
  tileinfo_t*   info      = new tileinfo_t [mSize];

  saveMeshes (level,info,nodeInfo);

  grmapheader_t    map;

  ConMessage ("]Save graph map...");

  grtile_t*     tile      = new grtile_t [mSize];
  BinFile*      file      = level->CreateLump (LL_LANDSCAPE,GameLevel::OPEN_RW);

  map.width               = mWidth;
  map.height              = mHeight; 
  map.vCount              = mVCount;
  map.fCount              = mFCount;
  map.bbox                = mBox;
  map.step                = mStep;

  map.cacheWidth          = mCacheWidth  > 0 ? mCacheWidth  : mWidth;
  map.cacheHeight         = mCacheHeight > 0 ? mCacheHeight : mHeight;

  for (int i=0;i<mSize;i++)
  {
    tile [i].box    = mNode [i].box;
    tile [i].tile   = info [i];
    tile [i].node   = nodeInfo [i];
    tile [i].index  = i;
  }  

  file->write (&map,sizeof (map));
  file->write (tile,sizeof (grtile_t) * mSize);

  delete    file;

  delete [] tile;
  delete [] info;
  delete [] nodeInfo;
}

void    GrMapConv::saveMeshes (GameLevel* level,tileinfo_t* info,nodeinfo_t* nodeInfo)
{
  ConMessage ("]Save land mesh...");

  grcacheheader_t  cache;
  BinFile*         file      = level->CreateLump (LL_LAND_MESH,GameLevel::OPEN_RW);
  char*            data      = new char [TEMP_DATA_SIZE];
  int              meshIndex = 0;
  size_t           minMesh   = TEMP_DATA_SIZE,
                   maxMesh   = 0;
  float            perPos    = 0;
  uint             pgcount   = 0;

  file->write  (&cache,sizeof (grcacheheader_t));

  for (int i=0;i<mSize;i++,pgcount+=mNode [i].pring.count ());
 
  for (i=0;i<mSize;i++,perPos+=mNode [i].pring.count())
  {
    RetMessage ("]Process: "<<100.0f*perPos/float (pgcount)<<" %");

    size_t meshSize   = buildNode (data,nodeInfo [i],mNode [i],meshIndex);

    info [i].size     = meshSize;
    info [i].offs     = file->getpos ();
    info [i].coord.y  = i / mWidth;
    info [i].coord.x  = i % mWidth;

    if (meshSize < minMesh) minMesh = meshSize;
    if (meshSize > maxMesh) maxMesh = meshSize;

    file->write (data,meshSize);
  }      

  cache.mapSize     = mWidth * mHeight;
  cache.minMeshSize = minMesh;
  cache.maxMeshSize = maxMesh;
  cache.meshNum     = meshIndex;
  cache.tablesOffs  = file->getpos ();
  cache.vCount      = mVBuffer.count ();

  file->write (info,mWidth * mHeight * sizeof (tileinfo_t));
  file->reset ();
  file->write (&cache,sizeof (cache));

  delete file;   
  delete [] data;  
}

void    GrMapConv::saveSkyBox (GameLevel* level)
{
  BinFile* file = level->CreateLump (LL_SKY_BOX,GameLevel::OPEN_RW);

  file->write (mTextNames,sizeof (textmat_t) * 6);

  delete file;
}

void    GrMapConv::saveVB     (GameLevel* level)
{
  ClearMessage;
  ConMessage ("]Save vertex buffers...");

  vbheader_t       vb;
  BinFile*         file  = level->CreateLump (LL_VERTEX_BUFFER,GameLevel::OPEN_RW);
  VBRing::iterator iter  = mVBuffer;
  int              count = mVBuffer.count ();
  ulong*           offs  = (ulong*)alloc (sizeof (ulong) * count);

  vb.vbSize              = 0;
  vb.offs                = sizeof (vbheader_t);
  vb.count               = count;

  file->write (&vb,sizeof (vbheader_t));
  file->write (offs,sizeof (ulong) * count);

  for (int i=0;i<count;i++,iter++)
  {
    offs [i] = file->getpos ();
    file->write (&iter,sizeof (vertexbuffer_t));
    file->write (iter.data ().vertex,iter.data ().count * sizeof (vertex_t));
    file->write (iter.data ().normal,iter.data ().count * sizeof (vector3d_t));
    file->write (iter.data ().tcoord,iter.data ().count * sizeof (textcoord_t));
  }  

  file->seek  (sizeof (vbheader_t));
  file->write (offs,sizeof (ulong) * count);

  free (offs);

  delete file;
}

void    GrMapConv::saveMat    (GameLevel* level)
{
  ConMessage ("]Save land materials...");

  landmatheader_t matHdr;
  BinFile*        file       = level->CreateLump (LL_LAND_MAT,GameLevel::OPEN_RW);

  size_t          matStrSize = 0;
  uint            matCount   = mObj3d->count (Object3D::MATERIAL)+1;

  file->write (&matHdr,sizeof (matHdr));

  for (int i=0;i<matCount;i++)
  {
    textmat_t tm  = mObj3d->GetMaterial (i);
    int       len = strlen (tm.name) + 1; 

    matStrSize   += len;

    file->write (tm.name,len);
  }          

  file->reset ();

  matHdr.count   = matCount;
  matHdr.strsize = matStrSize;   

  file->write (&matHdr,sizeof (matHdr));

  delete file;
}

void    GrMapConv::Save  (GameLevel* level)
{
  D_Try
  {
    saveMap     (level);
    saveVB      (level);
    saveSkyBox  (level);
    saveMat     (level);  
  }
  D_Exc ("GrMapConv: Error at Save (BinFile*)");
}

void    GrMapConv::SetCacheInfo (int _CacheWidth,int _CacheHeight)
{
  mCacheWidth   = _CacheWidth;
  mCacheHeight  = _CacheHeight;
}

void    GrMapConv::SetVBInfo     (int _Width,int _Height)
{
  mVBCacheWidth   = _Width;
  mVBCacheHeight  = _Height;  
}

void    GrMapConv::SetGrad       (int _Grad)
{
  mGrad = _Grad ? _Grad : 8;
}

void    GrMapConv::SetSkyTexture (int index,const char* name)
{
  if (index >= 0 && index < 6)
    strcpy (mTextNames [index].text,name);
}

GrMapConv::GrMapConv  (Pool* pool,Object3D& obj)
          : MemObject (pool),
            mVBuffer  (pool),
            mWidth    (0), mHeight   (0), mSize     (0),
            mFCount   (0), mVCount   (0), mNCount   (0),
            mVertex   (NULL), mTextCoord (NULL), mNormal (NULL),
            mFacet    (NULL), mNode     (NULL),
            mCacheWidth (0), mCacheHeight (0),
            mObj3d (NULL),
            mStripEnable (FALSE), mHWVertexCacheSize (16),
            mGrad        (16),     mVBCacheWidth (0), mVBCacheHeight (0)
{
  M_PushPool (pool);

  D_Try
  {
    textmat_t t;

    for (int i=0;i<6;mTextNames [i++] = t);

    convert (obj);
  }
  D_Exc ("GrMapConv: Error at construct");

  M_PopPool ();
}

GrMapConv::~GrMapConv ()
{
  D_Try
  {
    ConMessage ("]Destruct...");

    if (mNormal)        free (mNormal);
    if (mTextCoord)     free (mTextCoord);
    if (mVertex)        free (mVertex);
    if (mFacet)         free (mFacet);
//    if (mNode)          delete [] mNode;
    
       //delete mVBuffers

//    for (int i=0;i<mVBNum;i++)
//    {
//      delete [] mVBuffer [i].vertex;
//      delete [] mVBuffer [i].normal;
//      delete [] mVBuffer [i].tcoord;
//    }   

//    delete  []  mVBuffer;
  }
  D_Exc ("GrMapConv: Error at destruct");
}
