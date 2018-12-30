float    MainRender::GetMaxHeight     (const vector3d_t& v)
{
  int     grnode = int(v.z/mMap->step)*mMap->width+int(v.x/mMap->step);
  mesh_t* mesh   = (mesh_t*)mLandRender->mCache->lock (grnode);
  float   height = 0;

  vector3d_t line [2] = {vector3d_t (v.x,0,v.z),vector3d_t (v.x,mMap->bbox.max.y,v.z)};
  vertex_t   temp (0);

  for (int i=0;i<mLandRender->mInfo [grnode].count;i++,mesh=(mesh_t*)((char*)mesh+mesh->size))
  {
    facetpack_t* pack = mesh->data;

    for (int j=0;j<mesh->count;j++,pack++)
    {
      facet_t& facet    = pack->facet;
      plane_t  plane    = {facet.n.x,facet.n.y,facet.n.z,-(facet.n & mMap->vertex [facet.index [0]])};      
      vertex_t vrtx [3] = {mMap->vertex [facet.index [0]],mMap->vertex [facet.index [1]],mMap->vertex [facet.index [2]]};

      IntersectionPoint  (line,plane,temp);
      
      if (IsInsidePolygon (temp,vrtx,3))
        if (temp.y > height) height = temp.y;
    }
  }

  return height;
}

float    MainRender::GetMinHeight     (const vector3d_t& v)
{
  int     grnode = int(v.z/mMap->step)*mMap->width+int(v.x/mMap->step);
  mesh_t* mesh   = (mesh_t*)mLandRender->mCache->lock (grnode);
  float   height = mMap->bbox.max.y;

  vector3d_t line [2] = {vector3d_t (v.x,0,v.z),vector3d_t (v.x,mMap->bbox.max.y,v.z)};
  vertex_t   temp (0);

  for (int i=0;i<mLandRender->mInfo [grnode].count;i++,mesh=(mesh_t*)((char*)mesh+mesh->size))
  {
    facetpack_t* pack = mesh->data;

    for (int j=0;j<mesh->count;j++,pack++)
    {
      facet_t& facet    = pack->facet;
      plane_t  plane    = {facet.n.x,facet.n.y,facet.n.z,-(facet.n & mMap->vertex [facet.index [0]])};      
      vertex_t vrtx [3] = {mMap->vertex [facet.index [0]],mMap->vertex [facet.index [1]],mMap->vertex [facet.index [2]]};

      IntersectionPoint  (line,plane,temp);
      
      if (IsInsidePolygon (temp,vrtx,3))
        if (temp.y < height) height = temp.y;
    }
  }

  return height != mMap->bbox.max.y ? height : 0;
}


float    GrMapConv::GetMinHeight (float x,float y,const FacetRing& fring,const boundbox_t& box)
{
  D_Try
  {
    FacetRing::iterator iter   = fring;
    int                 count  = fring.count ();
    float               height = box.max.y;

    vector3d_t line [2] = {vector3d_t (x,0,y),vector3d_t (x,box.max.y,y)};
    vertex_t   temp (0);

    for (int i=0;i<count;i++,iter++)
    {
      facet_t& facet    = mFacetSrc [iter.data ()];
      plane_t  plane    = {facet.n.x,facet.n.y,facet.n.z,-(facet.n & mVertexSrc [facet.index [0]])};
      vertex_t vrtx [3] = {mVertexSrc [facet.index [0]],
                           mVertexSrc [facet.index [1]],
                           mVertexSrc [facet.index [2]]
                          };

      IntersectionPoint  (line,plane,temp);
      
      if (IsInsidePolygon (temp,vrtx,3))
        if (temp.y < height) height = temp.y;
    }

    return height != box.max.y ? height : 0;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    dout<<"GrMapConv: Error at GetMinHeight"<<endl;
    return 0;
  }
}

void    GrMapConv::calcHMap   (const FacetRing& fring,const boundbox_t& box,float* map)
{
  float x       = box.min.x,
        y       = box.min.z,
        xstep   = (box.max.x - box.min.x) / float (mHMapWidth),
        ystep   = (box.max.z - box.min.z) / float (mHMapWidth);
  float* hmap   = map;

  for (int i=0;i<mHMapWidth;i++,y+=ystep)
  {
    x = box.min.x;

    for (int j=0;j<mHMapWidth;j++,x+=xstep,hmap++)
      *hmap = GetMinHeight (x,y,fring,box);
  }

  hmap = map+1;

  for (i=0;i<mHMapWidth-1;i++)
  {
    float  *ptr1 = map - 1,
           *ptr2 = map + 1,
           *ptr3 = map + 1 + mHMapWidth;

    for (int j=0;j<mHMapWidth-1;j++,ptr1++,ptr2++,ptr3++,hmap++)
      *hmap = (*hmap+*ptr1+*ptr2+*ptr3) / 4.0f;
  }        
}

size_t  GrMapConv::buildNode  
 ( char*        data,
   nodeinfo_t&  node,
   FacetRing&   fring,
   int&         meshIndex,
   int          tileIndex
 )
{
  node.chunk [GRC_CHUNK_FACET_PACK].offs   = 0;
  node.chunk [GRC_CHUNK_FACET_PACK].count  = fring.count (); 
  node.chunk [GRC_CHUNK_FACET_PACK].first  = 0;
  node.chunk [GRC_CHUNK_FACET_PACK].size   = fring.count () * sizeof (facetpack_t);

             //Compute height-map

  node.chunk [GRC_CHUNK_HEIGHT_MAP].offs   = fring.count () * sizeof (facetpack_t);
  node.chunk [GRC_CHUNK_HEIGHT_MAP].count  = mHMapWidth; 
  node.chunk [GRC_CHUNK_HEIGHT_MAP].first  = 0;
  node.chunk [GRC_CHUNK_HEIGHT_MAP].size   = sizeof (float) * sqr (mHMapWidth);  

  size_t              size      = node.chunk [GRC_CHUNK_HEIGHT_MAP].size +
                                  node.chunk [GRC_CHUNK_FACET_PACK].size;

  if (size > TEMP_DATA_SIZE)
  {
    dout<<"GrMapConv: size < TEMP_DATA_SIZE. Call to DadyCool"<<endl;
    return 0;
  }

  calcHMap (fring,mNode [tileIndex].box,(float*)(data + node.chunk [GRC_CHUNK_HEIGHT_MAP].offs));

             //Compute material-meshes

  FacetRing::iterator iter      = fring;
  int                 count     = fring.count ();
  facetpack_t*        fp        = (facetpack_t*)data;

  size_t              meshSize  = 0;
  int                 fstMesh   = meshIndex;
  mesh_t*             mesh      = (mesh_t*)(data + size);

  int                 mat       = 0;
  static int          material [0x10000];

  if (size > TEMP_DATA_SIZE)
  {
    dout<<"GrMapConv: size < TEMP_DATA_SIZE. Call to DadyCool"<<endl;
    return 0;
  }

  if (count)
  {
    for (int i=0;i<count;i++,iter++,fp++)
    {
      fp->facet = mFacetSrc [iter.data ()];
      fp->index = iter.data ();

      for (int j=mat;j && mFacetSrc [iter.data()].texture!=material [j-1];j--);

      if  (!j)
      {
        if (mat < 0x10000)     material [mat++] = mFacetSrc [iter.data ()].texture;
        else    
          dout<<"GrMapConv: Too many materials in 1 mesh. Call to DadyCool "<<endl; 
      } 
    }  

    for (i=0;i<mat;i++)
    {
      FacetRing     src (pool());
      stripdest_t*  dest = NULL;
 
      for (int j=0;j<count;j++,iter++)
        if (mFacet [iter.data ()].texture == material [i])
          src.insert (iter.data ());

      dest = ComputeStrip (src,mFacet,1|2|4,pool());

      if (size + sizeof (mesh_t) > TEMP_DATA_SIZE)
      {
        dout<<"GrMapConv: size < TEMP_DATA_SIZE. Call to DadyCool"<<endl;
        return 0;
      }

      if (!dest)
      {
        mesh->material  = material [i];
        mesh->type      = GL_TRIANGLES;
        mesh->index     = meshIndex++;
        mesh->count     = src.count () * 3;
        mesh->size      = sizeof (mesh_t) + mesh->count * sizeof (uint);

        uint* index               = (uint*)mesh->data;
        FacetRing::iterator iter1 = src;
        
        for (int j=0;j<mesh->count;j++,iter1++,index+=3)
        {
          index [0] = mFacet [iter1.data ()].index [0];
          index [1] = mFacet [iter1.data ()].index [1];
          index [2] = mFacet [iter1.data ()].index [2];
        }  

        meshSize       += mesh->size;    
        size           += mesh->size;
      }
      else
      {
        mesh->material  = material [i];
        mesh->type      = GL_TRIANGLE_STRIP;
        mesh->index     = meshIndex++;
        mesh->count     = dest->strip->len;
        mesh->size      = sizeof (mesh_t) + dest->strip->len * sizeof (uint);

        meshSize       += mesh->size;
        size           += mesh->size;

        memcpy (mesh->data,dest->strip->strip,dest->strip->len * sizeof (uint));
        
        pool()->free (dest);
      }

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
    mesh->index     = -1;
    mesh->count     = 0;
    mesh->size      = sizeof (mesh_t);

    meshSize       += mesh->size;    
    size           += mesh->size;
  }

  node.chunk [GRC_CHUNK_STRIP].offs    = node.chunk [GRC_CHUNK_HEIGHT_MAP].size +
                                         node.chunk [GRC_CHUNK_FACET_PACK].size;
  node.chunk [GRC_CHUNK_STRIP].first   = count ? fstMesh : -1;
  node.chunk [GRC_CHUNK_STRIP].count   = mat;
  node.chunk [GRC_CHUNK_STRIP].size    = meshSize;

  if (!ConvertMeshes ((mesh_t*)(data + node.chunk [GRC_CHUNK_STRIP].offs),mat,mVBuffer))
  {
    #ifdef DEBUG
      dout<<"GrMapConv: Error at convert meshes "<<endl;
    #endif
    return node.chunk [GRC_CHUNK_STRIP].offs;
  }

  return size;
}

float    GetHeight     (const vector3d_t& v,LevelLoader& loader)
{
  D_Try
  {
    LandscapeCache* mCache = GetLandscapeCache (&loader);
    LandscapeMap  * mMap   = GetLandscapeMap   (&loader);

    int          grnode = int(v.z/mMap->step)*mMap->width+int(v.x/mMap->step);
    float        width  = mMap->hmapWidth;
    float*       map    = (float*)mCache->lock (grnode,GRC_CHUNK_HEIGHT_MAP);
    int          index  = int ((v.z-mMap->map [grnode].box.min.z)/width) * int (width) +
                          int ((v.x-mMap->map [grnode].box.min.x)/width);

    return map [index];                              
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    dout<<"Error at GetMinHeight"<<endl;
    return 0;
  }
}

