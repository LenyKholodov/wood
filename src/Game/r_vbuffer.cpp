#include "pch.h"
#include "r_vbuffer.h"

#include <gl\gl.h>
#include <graph\glext.h>
#include <misc.h>

/////////////////////////implementation of class VertexBuffer///////////////////

void   VertexBuffer::CVALock          (int first,int count)
{
  if (RENDER_USE_CVA)
    glLockArraysEXT (first,count);
}

void   VertexBuffer::CVAUnlock        ()
{
  if (RENDER_USE_CVA)
    glUnlockArraysEXT ();
}

void   VertexBuffer::CVARefresh       ()
{
  if (mPos)
  {
    CVAUnlock ();
    CVALock   (0,mPos);
    dout<<"VertexBuffer: Relock first: 0, count: "<<mPos<<endl;
  }  
}

void   VertexBuffer::ChangeBase       (int new_base)
{
  glVertexPointer     (3,GL_FLOAT,0,mVertex + new_base);
  glNormalPointer     (GL_FLOAT,0,mNormal + new_base);
  glTexCoordPointer   (2,GL_FLOAT,0,mTextCoord + new_base);
}

void   VertexBuffer::RestoreBase      ()
{
  glVertexPointer     (3,GL_FLOAT,0,mVertex);
  glNormalPointer     (GL_FLOAT,0,mNormal);
  glTexCoordPointer   (2,GL_FLOAT,0,mTextCoord);
}
                  
int    VertexBuffer::AllocChunk (uint num)
{
  if (mPos + num <= mCount) 
  {
    int pos = mPos; mPos += num;

    return pos;
  }
  else
    return -1;
}

BOOL   VertexBuffer::get  (uint pos,vertex_t** v,vector3d_t** n,textcoord_t** t)
{
  if (pos > mPos) return FALSE;
  else
  {
    *v = mVertex + pos;
    *n = mNormal + pos;
    *t = mTextCoord + pos;

    return TRUE;
  }  
}

VertexBuffer::VertexBuffer  (Pool* _Pool,uint count)
             : MemObject (_Pool),
               mCount    (count),
               mPos      (0),
               mVertex   (NULL), mNormal (NULL), mTextCoord (NULL)
{
  D_Try
  {
    mVertex    = (vertex_t*)alloc (count * sizeof (vertex_t));
    mNormal    = (vector3d_t*)alloc (count * sizeof (vector3d_t));
    mTextCoord = (textcoord_t*)alloc (count * sizeof (textcoord_t));

    if (!mTextCoord || !mVertex || !mNormal)
    {
      D_Message ("VertexBuffer: No enough memory for sys data ");
      return;
    }

    glEnableClientState (GL_VERTEX_ARRAY);
    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
    glEnableClientState (GL_NORMAL_ARRAY);    

    glVertexPointer     (3,GL_FLOAT,0,mVertex);
    glNormalPointer     (GL_FLOAT,0,mNormal);
    glTexCoordPointer   (2,GL_FLOAT,0,mTextCoord);
  }
  D_Exc ("VertexBuffer: Error at construct ");
}

VertexBuffer::~VertexBuffer ()
{
  D_Try
  {
    glDisableClientState (GL_VERTEX_ARRAY);
    glDisableClientState (GL_TEXTURE_COORD_ARRAY);
    glDisableClientState (GL_NORMAL_ARRAY);

    free                 (mVertex);
    free                 (mNormal);
    free                 (mTextCoord);

    CVAUnlock            ();
  }
  D_Exc ("VertexBuffer: Error at destruct ");
}

/////////////////////implementation of class VertexPool/////////////////////////

vbuffernode_t*   VertexPool::AllocChunk  (uint count)
{
  D_Try
  {
    if (mFree < count)
      return NULL;

    for (vpnode_t* node = mFirstFree;node;node = node->nextFree)
       if (node->vb.count >= count)
       {
          if (node->vb.count-count)
          {        
            vpnode_t* block = (vpnode_t*)alloc (sizeof (vpnode_t));

            if (!block)
              return NULL;

            block->vb.count  = node->vb.count  - count;            
            block->vb.base   = node->vb.base   + count;
            block->vb.vertex = node->vb.vertex + count;
            block->vb.normal = node->vb.normal + count;
            block->vb.tcoord = node->vb.tcoord + count;
            block->prev      = node;
            block->next      = node->next;

            block->nextFree  = node->nextFree;
            block->prevFree  = node->prevFree;

            if (block->nextFree)  block->nextFree->prevFree = block;      
            if (block->prevFree)  block->prevFree->nextFree = block;
            else                  mFirstFree                = block;
            
            if (node->next) node->next->prev = block;
                      
            node->vb.count   = count;            
            node->next       = block;  
          }  
          else 
          {
            if (node->nextFree) node->nextFree->prevFree  = node->prevFree; 
            if (node->prevFree) node->prevFree->nextFree  = node->nextFree;
            else                mFirstFree                = node->nextFree;
          }

          node->nextFree = NULL;
          node->prevFree = NULL;

          mFree         -= count;
       
          return &node->vb;
       }
       
    return NULL;              
  }
  D_ExcRet ("VertexPool: Error at AllocChunk",NULL);
}

VertexPool::vpnode_t*        VertexPool::concat (vpnode_t* left,vpnode_t* right)
{
    //Присоединение правого к левому      
    
  if (left->next != right)
     return NULL;
       
  left->vb.count += right->vb.count;
  left->nextFree  = right->nextFree;
  left->next      = right->next;

  if (left->nextFree) left->nextFree->prevFree = left;
  if (left->next)     left->next->prev         = left;

  free (right);
  
  return left;  
}

void VertexPool::FreeChunk   (vbuffernode_t* vb)
{
  D_Try
  {
    vpnode_t* node = (vpnode_t*)vb;

    mFree       += node->vb.count;
                                  
    if (mFirstFree && mFirstFree->vb.base < node->vb.base)
    {              
      for (vpnode_t* _free = mFirstFree;_free && _free->nextFree;_free=_free->nextFree)
         if (_free->nextFree->vb.base >= node->vb.base)
            break;
      
      node->prevFree = _free;
      node->nextFree = _free->nextFree;

      if (_free->nextFree)
        _free->nextFree->prevFree = node;
        
      _free->nextFree             = node;
    }
    else if (mFirstFree && mFirstFree->vb.base > node->vb.base)
    {  
      node->prevFree               = NULL;
      node->nextFree               = mFirstFree;
      mFirstFree->prevFree         = node;
      mFirstFree                   = node;   
    }
    else
    {
      mFirstFree     = node;
      
      node->nextFree = NULL;
      node->prevFree = NULL;
      
      return ;
    }     
    
    if (node->prev == node->prevFree && node->prev)
       node = concat (node->prev,node);
       
    if (node->next == node->nextFree && node->next)
       node = concat (node,node->next);
  }
  D_Exc ("VertexPool: Error at free");
}

BOOL             VertexPool::move   (vpnode_t* left,vpnode_t* right)
{
  //left - free; right - data
         
  if (!right)
    return false;                  

  memmove (left->vb.vertex,right->vb.vertex,left->vb.count * sizeof (vertex_t));
  memmove (left->vb.normal,right->vb.normal,left->vb.count * sizeof (vector3d_t));
  memmove (left->vb.tcoord,right->vb.tcoord,left->vb.count * sizeof (textcoord_t));

  swap    (left->vb.count,right->vb.count);

  int offs         = right->vb.count;
  
  right->vb.base   = left->vb.base + left->vb.count;
  right->vb.vertex = mVertex + right->vb.base;
  right->vb.normal = mNormal + right->vb.base;
  right->vb.tcoord = mTextCoord + right->vb.base;
  right->nextFree  = left->nextFree;
  right->prevFree  = left->prevFree;

  left->nextFree   = NULL;
  left->prevFree   = NULL;

  //left - data; right - free

  if (right->prevFree)  right->prevFree->nextFree   = right;
  else                  mFirstFree                  = right; 
  if (right->nextFree)  right->nextFree->prevFree   = right;

  for (;right->next && right->nextFree == right->next;right = concat (right,right->next));

  if (mMoveFn)
  {
    D_Try
    {
      if (!(*mMoveFn)(left->vb,-offs,mParam)) 
        return false;
    }
    D_Exception (EXCEPTION_EXECUTE_HANDLER)
    {
      return false;
    }  
  }  
    
  return true;  
}

void             VertexPool::defrag ()
{
  D_Try
  { 
    if (mFirstFree)
    {
      for (vpnode_t* node = mFirstFree;node;)
          if (move (node,node->next))   node = mFirstFree;
          else                          node = node->nextFree;
    }  
  }
  D_Exc ("VertexPool: Error at defrag");
}

uint             VertexPool::VertexMaxAvail () const
{
  uint _max = 0;

  for (vpnode_t* node = mFirstFree;node;node = node->nextFree)
     if (node->vb.count >= _max)
       _max = node->vb.count;

  return _max;
}
               
VertexPool::VertexPool  (Pool* _Pool,VertexBuffer* vb,uint count)
           : MemObject (_Pool),
             mVBuffer  (vb),
             mCount    (0), mFree (0),
             mParam    (0), mMoveFn (NULL),
             mFirstFree (NULL),
             mVertex    (NULL), mNormal (NULL), mTextCoord (NULL)
{
  D_Try
  {
    int base = vb->AllocChunk (count);    

    if (base == -1)
    {
      D_Message ("VertexPool: No enough vertex indexes");
      return;
    }  

    vb->get (base,&mVertex,&mNormal,&mTextCoord);

    mCount     = mFree = count;
    mFirstFree = (vpnode_t*)alloc (sizeof (vpnode_t));

    if (!mFirstFree)
    {
      D_Message ("VertexPool: No enough memory for sys data");
      return;
    }

    mFirstFree->vb.count  = count;
    mFirstFree->vb.base   = base;
    mFirstFree->vb.vertex = mVertex;
    mFirstFree->vb.normal = mNormal;
    mFirstFree->vb.tcoord = mTextCoord;
    mFirstFree->prevFree  = mFirstFree->nextFree = NULL;
    mFirstFree->prev      = mFirstFree->next     = NULL;
  }
  D_Exc ("VertexPool: Error at construct");
}

VertexPool::~VertexPool ()
{  
  //Утечка памяти
//  vcnode_t* next = mFirst.next->next;
//
//  for (vcnode_t* node = mFirst.next;node!=&mLast;node=next,next=next->next)
//    free (node);
}

/////////////////////////implementation of class VertexCache////////////////////

inline VertexCache::vcnode_t*   VertexCache::find (int index)
{
  if (mIndex) return mIndex [index];
  else
  {
    for (vcnode_t* node = mFirst.next;node!=&mLast && node->index != index;node=node->next);
    return node;
  }
}

int    VertexCache::bind (int index,int* count)
{
  int base = set (index,count);

  if (base != -1)
    mVBuffer->ChangeBase (base);

  return base;
}

int    VertexCache::set  (int index,int* count)
{
  D_Try
  {
    if (index == mCurBuf && index > 0) return mCurBase;

    if (mVBCount && (index < 0 || index >= mVBCount))
      return -1;

    vcnode_t* node = find (index);

    if (node && node->index == -1)
        return -1;

    if (!node)
    {
      node = LoadData (index);

      if (!node)
        return -1;

      mVBuffer->CVARefresh ();

      node->index                = index;
      if (mIndex) mIndex [index] = node;
    }

    if (node != mFirst.next)  //Перемещение элемента в начало списка
    {
      node->prev->next = node->next;
      node->next->prev = node->prev;

      node->prev      = &mFirst;      
      node->next      = mFirst.next;
      mFirst.next     = mFirst.next->prev = node;
    }  

    if (count)
      *count = node->vb->count;

    mCurBuf  = index;
    mCurBase = node->vb->base;

    return node->vb->base;
  }
  D_ExcRet ("VertexCache: Error at set",-1);
}

VertexCache::vcnode_t*    VertexCache::AllocChunk      (uint count)
{
  D_Try
  {
    if (count > mVertexPool.VertexCount ())
    {
      D_Message ("LandscapeCache: Error! count > vertex pool size");
      return NULL;
    }

    uint free = mVertexPool.VertexAvail ();

    if (free < count)
    {
      vcnode_t* prev = mLast.prev->prev;

      for (vcnode_t* node = mLast.prev;node!=&mFirst && free < count;node=prev,prev=prev->prev)
      {
        free            += node->vb->count;
        
        node->prev->next = node->next;
        node->next->prev = node->prev;

        if (mIndex) mIndex [node->index] = NULL;

        mVertexPool.FreeChunk (node->vb);
        GetPool()->free          (node);
      }                             
    }

    vbuffernode_t* vbnode = mVertexPool.AllocChunk (count);

    if (!vbnode)
    {
      mVertexPool.defrag         ();
      vbnode = mVertexPool.AllocChunk (count);

      if (!vbnode)
        return NULL;
    }

    vcnode_t* node = (vcnode_t*)alloc (sizeof (vcnode_t));

    if (!node)
    {
      mVertexPool.FreeChunk (vbnode);
      return NULL;
    }

    node->vb          = vbnode;
    node->prev        = &mFirst;    
    node->next        = mFirst.next;    

    mFirst.next->prev = node;
    mFirst.next       = node;

    return node;
  }
  D_ExcRet ("VertexCache: Error at AllocChunk",NULL);
}

void     VertexCache::FreeChunk       (vcnode_t* node)
{
  D_Try
  {
    if (node)
    {
      node->prev->next = node->next;
      node->next->prev = node->prev;

      if (mIndex) mIndex [node->index] = NULL;

      mVertexPool.FreeChunk (node->vb);
      free                  (node);    
    }  
  }
  D_Exc ("VertexCache: Error at FreeChunk");  
}

VertexCache::VertexCache     (Pool* _Pool,VertexBuffer* vb,int count,int vbCount)
            : MemObject   (_Pool),
              mVertexPool (_Pool,vb,count),
              mVBCount    (vbCount),
              mIndex      (NULL),
              mVBuffer    (vb),
              mCurBuf     (-1), mCurBase (0)
{
  D_Try
  {
    if (!IsValid (&mVertexPool))
      return;

    if (mVBCount)
    {
      mIndex = (vcnode_t**)alloc (sizeof (vcnode_t*) * mVBCount);
 
      if (!mIndex)
      {
        D_Message ("VertexCache: No enough memory for sys data");
        return;
      }

      memset (mIndex,NULL,sizeof (vcnode_t*) * mVBCount);
    }

    mFirst.prev = mLast.next = NULL;
    mFirst.next = &mLast;
    mLast.prev  = &mFirst;
    mLast.vb    = mFirst.vb = NULL;
    mLast.index = mFirst.index = -1;    
  }
  D_Exc ("VertexCache: Error at construct");
}

VertexCache::~VertexCache    ()
{
  if  (mIndex) free (mIndex);

  vcnode_t* next = mFirst.next->next;

  for (vcnode_t* node = mFirst.next;node!=&mLast;node=next,next=next->next)
    free (node);
}
