/*      FacetRing     src (pool());
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
//        mesh->index     = meshIndex++;
        mesh->index     = 0;
        mesh->count     = src.count () * 3;
        mesh->size      = sizeof (mesh_t) + mesh->count * sizeof (uint);

        meshIndex++;

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
//        mesh->index     = meshIndex++;
        mesh->index     = 0;
        mesh->count     = dest->strip->len;
        mesh->size      = sizeof (mesh_t) + dest->strip->len * sizeof (uint);

        meshSize       += mesh->size;
        size           += mesh->size;

        meshIndex++;

        memcpy (mesh->data,dest->strip->strip,dest->strip->len * sizeof (uint));
        
        pool()->free (dest);
      }*/

             //Compile facet arrays

/*  mFacetSrc           = (facet_t*)pool()->alloc (sizeof (facet_t) * mFCount);

  memcpy (mFacetSrc,mFacet,sizeof (facet_t) * mFCount);

  textcoord_t* temp   = (textcoord_t*)pool()->alloc (mVCount * 3 * sizeof (textcoord_t));
  int*         mirror = (int*)pool()->alloc (mVCount * 3 * sizeof (int));
  int size            = ConvertFacetArray (mFacet,mFCount,mirror,temp,mVCount * 3,mVCount);

  ConMessage ("]Add "<<size-mVCount<<" vertexes ");

  vertex_t*    vertex = (vertex_t*)pool()->alloc (size * sizeof (vertex_t));
  vector3d_t*  normal = (vector3d_t*)pool()->alloc (size * sizeof (vector3d_t));
  textcoord_t* tcoord = (textcoord_t*)pool()->alloc (size * sizeof (textcoord_t));

  memcpy (tcoord,temp,size * sizeof (textcoord_t));

  for (i=0;i<size;i++)
  {   
    vertex [i] = mVertex [mirror [i]];
    normal [i] = mNormal [mirror [i]];
  }

  pool()->free (mirror);
  pool()->free (mNormal);
  pool()->free (temp);

  mVertexSrc = mVertex;
  mVertex    = vertex;
  mNormal    = normal;
  mTextCoord = tcoord;
  mVBCount   = size;*/