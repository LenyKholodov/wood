/*    RingTmpl<int> iring (M_GetPool());
    mesh_t*       mesh  = (mesh_t*)(model->data + offs [i]);
    int*          index = (int*)mesh->data;
    int           mini  = 0,
                  maxi  = 0;

    if (mesh->count)
    {
      mini = mFacet [*index].index [0];
      maxi = mFacet [*index].index [0];

      for (int j=0;j<mesh->count;j++,index++)
      {
        iring.insert (*index);

        for (int k=0;k<3;k++)
        {
          int vindex = mFacet [*index].index [k];

          if (mini > vindex) mini = vindex;
          if (maxi < vindex) maxi = vindex;
        }  
      }  

      meshNum++;
    }
    else
      continue;

    stripdest_t* dest = ComputeStrip (iring,mFacet,1|2|4);
    int          mtr  = mesh->material;

    if (!dest)
    {
      index             = (int*)mesh->data;
      int count         = mesh->count;

      mesh              = (mesh_t*)(new char [sizeof (mesh_t) + count * 3 * sizeof (int)]);

      if (!mesh)
        continue;

      mesh->vbuffer     = treeIndex;
      mesh->count       = count * 3;
      mesh->material    = matReindex [mtr];
      mesh->index       = 0;
      mesh->size        = sizeof (mesh_t) + count * 3 * sizeof (int);
      mesh->start       = mini;
      mesh->end         = maxi;
      mesh->type        = GL_TRIANGLES;      

      int* destIndex    = (int*)mesh->data; 

      for (int j=0;j<count;j++,index++,destIndex += 3)
      {
        destIndex [0] = mFacet [*index].index [0];
        destIndex [1] = mFacet [*index].index [1];
        destIndex [2] = mFacet [*index].index [2];
      }  
    }
    else
    {
      mesh              = (mesh_t*)(new char [sizeof (mesh_t) + dest->strip->len * sizeof (int)]);

      if (!mesh)
        continue;

      mesh->vbuffer     = treeIndex;
      mesh->count       = dest->strip->len;
      mesh->material    = matReindex [mtr];
      mesh->index       = 0;
      mesh->size        = sizeof (mesh_t) + dest->strip->len * sizeof (int);
      mesh->start       = mini;
      mesh->end         = maxi;
      mesh->type        = GL_TRIANGLE_STRIP;

      memcpy (mesh->data,dest->strip->strip,sizeof (int) * dest->strip->len);

      M_GetPool()->free (dest);
    }*/
