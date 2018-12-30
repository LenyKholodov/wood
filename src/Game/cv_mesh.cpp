#include "pch.h"
#include "cv_utils.h"
#include <misc.h>
#include <graph\striper.h>
#include <gl\gl.h>

void       CopyByte (uint* src,uchar* dest,int count)
{
  for (int i=0;i<count;i++,*dest++ = * src++);
}

void       CopyWord (uint* src,uint16* dest,int count)
{
  for (int i=0;i<count;i++,*dest++ = * src++);
}

void       CopyDWord (uint* src,uint* dest,int count)
{
  for (int i=0;i<count;i++,*dest++ = * src++);
}

void       CopyData (mesh_t* mesh,uint* index,BOOL size_pack)
{
  if (!size_pack)
    memcpy (mesh->data,index,mesh->count * sizeof (uint));
  else
  {
    if (mesh->end < 256) 
    {
      mesh->elType    = GL_UNSIGNED_BYTE;
      mesh->size      = sizeof (mesh_t) + mesh->count * sizeof (uchar);

      CopyByte (index,(uchar*)mesh->data,mesh->count);
    }
    else if (mesh->end < 0x10000)
    {
      mesh->elType    = GL_UNSIGNED_SHORT;
      mesh->size      = sizeof (mesh_t) + mesh->count * sizeof (uint16);

      CopyWord (index,(uint16*)mesh->data,mesh->count);
    }
    else
    {
      mesh->elType    = GL_UNSIGNED_INT;
      mesh->size      = sizeof (mesh_t) + mesh->count * sizeof (uint);

      CopyDWord (index,(uint*)mesh->data,mesh->count);
    }
  }  
}

inline bool IsIn (int x,int start,int end)
{
  return x >= start && x <= end;
}

static void BuildTLList (uint* index,uint count,uint vcache_size,uint& start,uint& end)
{
  start      = *index;
  end        = *index;
  uint* ind  = index;

  for (int i=0;i<count;i++,ind++)
  {
    if (*ind < start) start = *ind;
    if (*ind > end)   end   = *ind;
  }  

  if (end - start <= vcache_size)  //all ok
    return;

  int   lump_num = (end-start) / vcache_size + 1;
  uint* pos      = index;

  for (int l=0;l<lump_num;l++)
  {
    uint  s   = start + l * vcache_size,
          e   = start + (l+1) * vcache_size;
    uint  cnt = count - ulong (pos-index);

    for (int i=0;i<cnt;)
      if (IsIn (pos [i],s,e) && IsIn (pos [i+1],s,e) && IsIn (pos [i+2],s,e))
      {
        if (i)
        {
          swap (pos [0],pos [i]);
          swap (pos [1],pos [i+1]);
          swap (pos [2],pos [i+2]);
        }  

        pos += 3;
        cnt -= 3;
      }  
      else  i+=3;
  }
}

mesh_t*         CreateMesh (
  const RingTmpl<int>&     ring,
  const facet_t*           farray,
  uint                     vcsize,
  BOOL                     strip_enable,
  BOOL                     size_pack,
  Pool*                    pool,
  Pool*                    temp
)
{
  D_Try
  {     
    if (!ring.count () || !farray)
      return NULL;

    uint* index = (uint*)temp->alloc (sizeof (uint) * ring.count () * 3);

    if (!index)
      D_NoMemoryRet ("::Create mesh",sizeof (uint) * ring.count () * 3,NULL);

    int                     count = ring.count ();
    RingTmpl<int>::iterator iter  = ring;
    uint*                   ind   = index;

    for (int i=0;i<count;i++,iter++)
    {
      *ind++ = farray [iter.data()].index [0];
      *ind++ = farray [iter.data()].index [1];
      *ind++ = farray [iter.data()].index [2];
    }

    uint start = *index, 
         end   = *index;

    BuildTLList (index,count * 3,vcsize,start,end);

    mesh_t* mesh = NULL;

    if (strip_enable)
    {
      stripsrc_t    src;

      src.count          = count * 3;
      src.faces          = index;
      src.flags          = 1|2|4;

      M_PushPool (temp);

      stripdest_t*  dest = ComputeStrip (src);

      M_PopPool  ();

      if (dest)
      {
        mesh = (mesh_t*)pool->alloc (sizeof (mesh_t) + dest->strip->len * sizeof (uint));

        if (!mesh)
        {
          D_NoMemory ("::CreateMesh",sizeof (mesh_t) + dest->strip->len * sizeof (uint));
          temp->free (index);
          temp->free (dest);
          return NULL;
        }

        mesh->material  = farray [iter.data()].texture;
        mesh->type      = GL_TRIANGLE_STRIP;
        mesh->elType    = GL_UNSIGNED_INT;
        mesh->index     = 0;
        mesh->vbuffer   = 0;
        mesh->count     = dest->strip->len;
        mesh->size      = sizeof (mesh_t) + dest->strip->len * sizeof (uint);
        mesh->start     = start;
        mesh->end       = end;

        CopyData (mesh,dest->strip->strip,size_pack);
          
        temp->free (dest);      
      }  
    }

    if (!mesh)
    {
      mesh = (mesh_t*)pool->alloc (sizeof (mesh_t) + sizeof (uint) * 3 * count);

      if (!mesh)
      {
        D_NoMemory ("::CreateMesh",sizeof (mesh_t) + sizeof (uint) * 3 * count);
        temp->free (index);
        return NULL;
      }

      mesh->material  = farray [iter.data()].texture;
      mesh->elType    = GL_UNSIGNED_INT;
      mesh->type      = GL_TRIANGLES;
      mesh->index     = 0;
      mesh->vbuffer   = 0;
      mesh->count     = count * 3;
      mesh->size      = sizeof (mesh_t) + mesh->count * sizeof (uint);
      mesh->start     = start;
      mesh->end       = end;

      CopyData (mesh,index,size_pack);
    }

    temp->free (index);

    return mesh;
  } 
  D_ExcRet ("::Error at CreateMesh",NULL);
}

BOOL            CreateMesh (
  mesh_t*                  mesh,                
  const RingTmpl<int>&     ring,
  const facet_t*           farray,
  uint                     vcsize,
  BOOL                     strip_enable,
  BOOL                     size_pack,
  Pool*                    temp
)
{
  D_Try
  {     
    if (!ring.count () || !farray || !mesh)
      return FALSE;

    uint* index = (uint*)temp->alloc (sizeof (uint) * ring.count () * 3);

    if (!index)
      D_NoMemoryRet ("::Create mesh",sizeof (uint) * ring.count () * 3,FALSE);

    int                     count = ring.count ();
    RingTmpl<int>::iterator iter  = ring;
    uint*                   ind   = index;

    for (int i=0;i<count;i++,iter++)
    {
      *ind++ = farray [iter.data()].index [0];
      *ind++ = farray [iter.data()].index [1];
      *ind++ = farray [iter.data()].index [2];
    }

    uint start = *index, 
         end   = *index;
    BOOL init  = FALSE;

    BuildTLList (index,count * 3,vcsize,start,end); 

    if (strip_enable)
    {
      stripsrc_t    src;

      src.count          = count;
      src.faces          = index;
      src.flags          = 1|2|4;

      M_PushPool (temp);

      stripdest_t*  dest = ComputeStrip (src,temp);

      M_PopPool  ();

      if (dest)
      {
        mesh->material  = farray [iter.data()].texture;
        mesh->type      = GL_TRIANGLE_STRIP;
        mesh->index     = 0;
        mesh->vbuffer   = 0;
        mesh->elType    = GL_UNSIGNED_INT;
        mesh->count     = dest->strip->len;
        mesh->size      = sizeof (mesh_t) + dest->strip->len * sizeof (uint);
        mesh->start     = start;
        mesh->end       = end;

        CopyData  (mesh,dest->strip->strip,size_pack);
          
        temp->free (dest);      

        init            = TRUE;
      }  
    }

    if (!init)
    {
      mesh->material  = farray [iter.data()].texture;
      mesh->type      = GL_TRIANGLES;
      mesh->elType    = GL_UNSIGNED_INT;
      mesh->index     = 0;
      mesh->vbuffer   = 0;
      mesh->count     = count * 3;
      mesh->size      = sizeof (mesh_t) + mesh->count * sizeof (uint);
      mesh->start     = start;
      mesh->end       = end;

      CopyData (mesh,index,size_pack);
    }

    temp->free (index);

    return TRUE;
  } 
  D_ExcRet ("::Error at CreateMesh",FALSE);
}

BOOL            CreateMesh (
  mesh_t*                   mesh,                
  const RingTmpl<facet_t>&  ring,
  uint                      vcsize,
  BOOL                      strip_enable,
  BOOL                      size_pack,
  Pool*                     temp
)
{
  D_Try
  {     
    if (!ring.count () || !mesh)
      return FALSE;

    uint* index = (uint*)temp->alloc (sizeof (uint) * ring.count () * 3);

    if (!index)
      D_NoMemoryRet ("::Create mesh",sizeof (uint) * ring.count () * 3,FALSE);

    int                         count = ring.count ();
    RingTmpl<facet_t>::iterator iter  = ring;
    uint*                       ind   = index;

    for (int i=0;i<count;i++,iter++)
    {
      *ind++ = iter.data ().index [0];
      *ind++ = iter.data ().index [1];
      *ind++ = iter.data ().index [2];
    }

    uint start = *index, 
         end   = *index;
    BOOL init  = FALSE;

    BuildTLList (index,count * 3,vcsize,start,end); 

    if (strip_enable)
    {
      stripsrc_t    src;

      src.count          = count;
      src.faces          = index;
      src.flags          = 1|2|4;

      M_PushPool (temp);

      stripdest_t*  dest = ComputeStrip (src,temp);

      M_PopPool  ();

      if (dest)
      {
        mesh->material  = iter.data ().texture;
        mesh->type      = GL_TRIANGLE_STRIP;
        mesh->elType    = GL_UNSIGNED_INT;
        mesh->index     = 0;
        mesh->vbuffer   = 0;
        mesh->count     = dest->strip->len;
        mesh->size      = sizeof (mesh_t) + dest->strip->len * sizeof (uint);
        mesh->start     = start;
        mesh->end       = end;

        CopyData (mesh,dest->strip->strip,size_pack);
          
        temp->free (dest);      

        init            = TRUE;
      }  
    }

    if (!init)
    {
      mesh->material  = iter.data ().texture;
      mesh->type      = GL_TRIANGLES;
      mesh->elType    = GL_UNSIGNED_INT;
      mesh->index     = 0;
      mesh->vbuffer   = 0;
      mesh->count     = count * 3;
      mesh->size      = sizeof (mesh_t) + mesh->count * sizeof (uint);
      mesh->start     = start;
      mesh->end       = end;

      CopyData (mesh,index,size_pack);
    }

    temp->free (index);

    return TRUE;
  } 
  D_ExcRet ("::Error at CreateMesh",FALSE);
}
