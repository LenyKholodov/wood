float    GetMaxHeight     (const vector3d_t& v,LevelLoader& loader)
{
  D_Try
  {
    LandscapeMap*   mMap   = GetLandscapeMap (&loader);
    LandscapeCache* mCache = GetLandscapeCache (&loader);

    int          grnode = int(v.z/mMap->step)*mMap->width+int(v.x/mMap->step);
    facetpack_t* fp     = (facetpack_t*)mCache->lock (grnode,GRC_CHUNK_FACET_PACK);
    float        height = 0;

    vector3d_t line [2] = {vector3d_t (v.x,0,v.z),vector3d_t (v.x,mMap->bbox.max.y,v.z)};
    vertex_t   temp (0);

    for (int i=0;i<mCache->GetNodeInfo (grnode).chunk [GRC_CHUNK_FACET_PACK].count;i++,fp++)
    {
      facet_t& facet    = fp->facet;
      plane_t  plane    = {facet.n.x,facet.n.y,facet.n.z,-(facet.n & mMap->vertex [facet.index [0]])};      
      vertex_t vrtx [3] = {mMap->vertex [facet.index [0]],mMap->vertex [facet.index [1]],mMap->vertex [facet.index [2]]};

      IntersectionPoint  (line,plane,temp);
      
      if (IsInsidePolygon (temp,vrtx,3))
        if (temp.y > height) height = temp.y;
    }

    return height;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    dout<<"Error at GetMaxHeight"<<endl;
    return 0;
  }
}

float    GetMinHeight     (const vector3d_t& v,LevelLoader& loader)
{
  D_Try
  {
    LandscapeMap*  mMap    = GetLandscapeMap (&loader);
    LandscapeCache* mCache = GetLandscapeCache (&loader);

    int          grnode = int(v.z/mMap->step)*mMap->width+int(v.x/mMap->step);
    facetpack_t* fp     = (facetpack_t*)mCache->lock (grnode,GRC_CHUNK_FACET_PACK);
    float        height = mMap->bbox.max.y;

    vector3d_t line [2] = {vector3d_t (v.x,0,v.z),vector3d_t (v.x,mMap->bbox.max.y,v.z)};
    vertex_t   temp (0);

    for (int i=0;i<mCache->GetNodeInfo (grnode).chunk [GRC_CHUNK_FACET_PACK].count;i++,fp++)
    {
      facetpack_t* pack = fp;

      facet_t& facet    = pack->facet;
      plane_t  plane    = {facet.n.x,facet.n.y,facet.n.z,-(facet.n & mMap->vertex [facet.index [0]])};      
      vertex_t vrtx [3] = {mMap->vertex [facet.index [0]],mMap->vertex [facet.index [1]],mMap->vertex [facet.index [2]]};

      IntersectionPoint  (line,plane,temp);
      
      if (IsInsidePolygon (temp,vrtx,3))
        if (temp.y < height) height = temp.y;
    }

    return height != mMap->bbox.max.y ? height : 0;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    dout<<"Error at GetMinHeight"<<endl;
    return 0;
  }
}

