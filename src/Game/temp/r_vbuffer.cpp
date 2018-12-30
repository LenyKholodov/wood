//////////////////////implementation of class VertexLump////////////////////////

BOOL    VertexLump::set             (int index)
{
  if (mCurBuf != index)
    if (index >= 0 && index < mVBNum)
    {  
//      if (mCurBuf != -1) mVBuffer->unlock ();

      uint lockSize = 0;

      if (mCache)
      {
        vertexbuffer_t* vb   = (vertexbuffer_t*)mCache->data (index);

        if (vb)
        {
          memcpy (mVertex,vb->vertex,sizeof (vertex_t) * vb->pos);
          memcpy (mNormal,vb->normal,sizeof (vector3d_t) * vb->pos);
          memcpy (mTextCoord,vb->tcoord,sizeof (textcoord_t) * vb->pos);
        }
        else
          return FALSE;  

        lockSize = vb->pos;
      } 
      else
      {
        vertexbuffer_t    vb;

        vb.vertex         = mVertex;
        vb.normal         = mNormal;
        vb.tcoord         = mTextCoord;
        vb.count          = mCount;
        vb.pos            = 0;
        vb.first          = base ();

        LoadData (index,&vb);

        lockSize          = vb.pos;
      }  

//      mVBuffer->lock (base (),lockSize);
      mVBuffer->CVARefresh ();

      mCurBuf = index;

      return TRUE;
    }  
    else
      return FALSE;
  return TRUE;
}

void    VertexLump::_LoadData       (DWORD par,int index,void* buf,size_t size)
{
  D_Try
  {
    VertexLump*     This  = (VertexLump*)par;
    vertexbuffer_t* vb    = (vertexbuffer_t*)buf;

    if (This->mCache)
    {
      vb->vertex           = NULL;
      vb->normal           = NULL;
      vb->tcoord           = NULL;
    }
  
    This->LoadData (index,vb);
  }
  D_Exc ("VertexLump: Error at _LoadData ");
}

void      VertexLump::InitCache       (int cacheNum,int cacheSize)
{
  D_Try
  {
    if (cacheNum > 1 && cacheSize > 1)
    {
      if (mCache) 
        delete mCache;

      mCache  = new (pool()) 
                Cache (pool(),cacheNum,
                       mCount * (sizeof (vertex_t) + sizeof (vector3d_t) + sizeof (textcoord_t)) + sizeof (vertexbuffer_t),
                       cacheSize<cacheNum?cacheSize:cacheNum,
                       _LoadData,
                       (DWORD)this
                      );

      mCurBuf = -1;
    }
  }
  D_Exc ("VertexLump: Error at InitCache");
}

void      VertexLump::InitCache       (int clSize,int cacheNum,int cacheSize)
{
  D_Try
  {
    mCount      = clSize;
    mBaseIndex  = mVBuffer->AllocChunk (mCount);
    mVBNum      = cacheNum;

    InitCache (cacheNum,cacheSize);

    mVBuffer->get (mBaseIndex,&mVertex,&mNormal,&mTextCoord);
  }
  D_Exc ("VertexLump: Error at InitCache");
}

VertexLump::VertexLump      (Pool* _Pool,VertexBuffer* vb,int count,int cacheNum,int cacheSize)
           : MemObject (_Pool),
             mCache    (NULL),                            
             mCurBuf   (-1),
             mCount    (count), mVBNum (cacheNum),
             mVertex   (NULL), mNormal (NULL), mTextCoord (NULL),
             mVBuffer  (vb),
             mBaseIndex (vb->AllocChunk (count))
             
{
  D_Try
  {
    if (mBaseIndex == -1)
    {
      D_Message ("VertexLump: Error at alloc base index");
      return;
    }

    InitCache (cacheNum,cacheSize);

    vb->get (mBaseIndex,&mVertex,&mNormal,&mTextCoord);
  }
  D_Exc ("VertexLump: Error at construct");
}

VertexLump::VertexLump  (Pool* _Pool,VertexBuffer* vb)
           : MemObject (_Pool),
             mCache    (NULL),                            
             mCurBuf   (-1),
             mCount    (0), mVBNum (0),
             mVertex   (NULL), mNormal (NULL), mTextCoord (NULL),
             mVBuffer  (vb),
             mBaseIndex (-1)
             
{   }

VertexLump::~VertexLump     ()
{
  D_Try
  {
    if (mCache) delete mCache;
  }
  D_Exc ("VertexLump: Error at destruct");
}
