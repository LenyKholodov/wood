void         LandMeshRender::_Flush ()
{
  D_Try
  {
    if (mChangeEnv)
    {
      for (int mindex=mMatFirst;mindex != -1;mindex = mMat [mindex].next)
      {
        if (mSetMaterial) (*mSetMaterial)(mParam,mindex-1);

        for (meshnode_t* mnode = mMat [mindex].firstMesh;mnode;mnode=mnode->next)
        {
          BOOL change = (*mChangeEnv) ((DWORD)mParam,mnode->tile);

          if (mMeshBufPos == 0)
          {
            if (mnode->mesh->count < mMeshBufSize)
            {
              mMeshBuf->count   = mnode->mesh->count;
              mMeshBuf->vbuffer = mnode->mesh->vbuffer;
              mMeshBuf->type    = mnode->mesh->type;
              mMeshBuf->start   = mnode->mesh->start;
              mMeshBuf->end     = mnode->mesh->end;

              mMeshBufPos       = mnode->mesh->count;

              memcpy (mMeshBuf->data,mnode->mesh->data,mnode->mesh->count * sizeof (uint));
            }
            else
              DrawMesh (mnode->mesh);
          }
          else if (!change && 
              mMeshBuf->vbuffer == mnode->mesh->vbuffer &&
              mMeshBuf->type    == mnode->mesh->type &&
              mMeshBufPos + mnode->mesh->count < mMeshBufSize - 2
             ) 
          {
            uint* dest = (uint*)(mMeshBuf->data + mMeshBufPos * sizeof (uint));
            uint* src  = (uint*)(mnode->mesh->data);

            if (mMeshBuf->type == GL_TRIANGLE_STRIP)
            {
              dest [0] = dest [-1];
              dest [1] = src  [0];

              dest            += 2;
              mMeshBuf->count += 2;
              mMeshBufPos     += 2;
            }

            memcpy (dest,src,mnode->mesh->count * sizeof (uint));

            if (mMeshBuf->end < mnode->mesh->end)
              mMeshBuf->end = mnode->mesh->end;
            if (mMeshBuf->start > mnode->mesh->start)
              mMeshBuf->start = mnode->mesh->start;

            mMeshBuf->count += mnode->mesh->count;
            mMeshBufPos     += mnode->mesh->count;
          }
          else
          {
            DrawMesh (mMeshBuf);

            if (mnode->mesh->count < mMeshBufSize)
            {
              mMeshBuf->count   = mnode->mesh->count;
              mMeshBuf->vbuffer = mnode->mesh->vbuffer;
              mMeshBuf->type    = mnode->mesh->type;
              mMeshBuf->start   = mnode->mesh->start;
              mMeshBuf->end     = mnode->mesh->end;

              mMeshBufPos       = mnode->mesh->count;

              memcpy (mMeshBuf->data,mnode->mesh->data,mnode->mesh->count * sizeof (uint));
            }
            else
            {
              DrawMesh (mnode->mesh);
              mMeshBufPos = 0;
            }  
          }
        }  

        mMat [mindex].firstMesh = NULL;
        mMat [mindex].num       = 0;

        if (mMeshBufPos)
        {
          DrawMesh (mMeshBuf);
          
          mMeshBufPos = 0;
        }  
      }
    }
    else
    {
      for (int mindex=mMatFirst;mindex != -1;mindex = mMat [mindex].next)
      {
        if (mSetMaterial) (*mSetMaterial)(mParam,mindex-1);

        for (meshnode_t* mnode = mMat [mindex].firstMesh;mnode;mnode=mnode->next)
          DrawMesh      (mnode->mesh);

        mMat [mindex].firstMesh = NULL;
        mMat [mindex].num       = 0;
      }
    }

    mMeshPos  = 0;
    mMatFirst = -1;

    mCache->reset ();
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"LandMeshRender: Error at Flush "<<endl;
    #endif

    mMeshPos  = 0;
    mMatFirst = -1;
  }
}
