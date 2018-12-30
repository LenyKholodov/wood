uint    ConvertVertexBuffer (
  int                    fcount,
  int                    vcount,
  facet_t*               farray,
  vertex_t*              vertex,
  vector3d_t*            normal,
  textcoord_t*           tcoord,
  uint                   chunkSize,
  uint                   rSize,
  vertexbuffer_t**       dest
)
{
  D_Try        
  {
    uint                vbNum = vcount / chunkSize;
    uint                vbPos = 0;
    vertexbuffer_t*     vb    = new vertexbuffer_t [vbNum];
    *dest                     = vb;

    if (vcount % chunkSize) vbNum++;

    if (!vb)
    {
      #ifdef DEBUG
        dout<<"No enough memory for vertex buffer "<<endl;
      #endif
      return 0;
    }

    int vPos = 0;

    for (int i=0;vPos<vcount;i++,vPos+=chunkSize)
    {
      vb [i].vertex   = new vertex_t [chunkSize + rSize];
      vb [i].normal   = new vector3d_t [chunkSize + rSize];
      vb [i].tcoord   = new textcoord_t [chunkSize + rSize];

      if (!vb [i].vertex || !vb [i].normal || !vb [i].tcoord)
      {
        #ifdef DEBUG
          dout<<"No enough memory for vertex buffer "<<endl;
        #endif

        return i;
      }

      vb [i].count    = chunkSize + rSize;
      vb [i].pos      = vcount - vPos > chunkSize ? chunkSize : vcount - vPos;
      vb [i].first    = (chunkSize + rSize) * i;

      memcpy (vb [i].vertex,vertex + vPos,sizeof (vertex_t) * vb [i].pos);
      memcpy (vb [i].normal,normal + vPos,sizeof (vector3d_t) * vb [i].pos);
      memcpy (vb [i].tcoord,tcoord + vPos,sizeof (textcoord_t) * vb [i].pos);
    }

    facet_t*  facet = farray;

    for (i=0;i<fcount;i++,facet++) //attention!
    {
      facet->index [0] = (facet->index [0] % chunkSize) + vb [facet->index [0] / chunkSize].first;
      facet->index [1] = (facet->index [1] % chunkSize) + vb [facet->index [1] / chunkSize].first;
      facet->index [2] = (facet->index [2] % chunkSize) + vb [facet->index [2] / chunkSize].first;
    }

    return vbNum;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Error at ConvertVertexBuffer "<<endl;
    #endif
    return 0;
  }
}

uint    FindMostVB (uint* index,uint num,uint16* vb)
{
        //Calc most prioritize vb

  static uint mostVB  [256];         
  static uint countVB [0x10000];
  int         vbPos = 0,
              vbMost = 0,
              vbCount = 0;

  for (int j=0;j<num;j++)    
  {
//    int vbIndex = index [j] / chunkSize;
    int vbIndex = vb [index [j]];

    for (int k=0;k<vbPos;k++)
      if (mostVB [k] == vbIndex)
      {
        countVB [k]++;
        break;
      }  

    if (k == vbPos)
    {
      mostVB  [vbPos]   = vbIndex;
      countVB [vbPos++] = 1;
    } 
  }  

  for (j=0;j<vbPos;j++)
    if (countVB [j] > vbCount)
    {
      vbCount = countVB [j];
      vbMost  = mostVB [j];
    }

  return vbMost;
}

BOOL               ConvertMeshes (
  mesh_t*          mesh,
  uint             count,
  vertexbuffer_t*  vb,
  uint16*          vbindex
)
{
  uint chunkSize = vb [0].count;

  for (int i=0;i<count;i++,mesh=(mesh_t*)((char*)mesh+mesh->size))
  {
    uint* index    = (uint*)mesh->data;
    int   vbMost   = FindMostVB (index,mesh->count,vbindex);

    for (int j=0;j<mesh->count;j++,index++)
    {
//      int VB = *index / chunkSize;            
      int VB = vbindex [*index];

      if (VB != vbMost)
      {
        vertex_t&    v1   = vb [VB].vertex [*index - vb [VB].first];
        vertex_t*    v2   = vb [vbMost].vertex;
        int          cnt  = vb [vbMost].pos;

        for (int k=0;k<cnt;k++,v2++)
          if (CmpVertex (v1,*v2))
          {
            *index = k + vb [vbMost].first;
            
            break;
          }                                

        if (k == cnt)
          if (vb [vbMost].pos != vb [vbMost].count)
          {
            int pos   = vb [vbMost].pos++;

            vb [vbMost].vertex [pos] = vb [VB].vertex [*index - vb [VB].first];
            vb [vbMost].normal [pos] = vb [VB].normal [*index - vb [VB].first];
            vb [vbMost].tcoord [pos] = vb [VB].tcoord [*index - vb [VB].first];

            *index                   = pos + vb [vbMost].first;
          }
          else
          {
            D_Message ("No reserved vertex buffer limit for compile facet");
            return FALSE;
          }
      }  
    }  

    index    = (uint*)mesh->data;

    int      min = *index - vb [vbMost].first,
             max = *index - vb [vbMost].first;

    for (j=0;j<mesh->count;j++,index++)
    {
      *index -= vb [vbMost].first;

      if (*index < min) min = *index;
      if (*index > max) max = *index;
    }  

    mesh->vbuffer = vbMost;
    mesh->start   = min;
    mesh->end     = max;
  }

  return TRUE;
}

inline BOOL    CheckFacetVB (const facet_t& facet,uint chunkSize)
{
  return (facet.index [0] / chunkSize == facet.index [1] / chunkSize) &&
         (facet.index [1] / chunkSize == facet.index [2] / chunkSize) &&
         (facet.index [0] / chunkSize == facet.index [2] / chunkSize);
}

inline BOOL CmpVertex (const vertex_t& v1,const vertex_t& v2)
{
//  return fabs (v1.x - v2.x) < EPS && fabs (v1.y - v2.y) < EPS && fabs (v1.z - v2.z) < EPS;
  return v1 == v2;
}