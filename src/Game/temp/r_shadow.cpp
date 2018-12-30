void        ShadowEnv::Draw            (int num,visnode_t* vis)
{
  D_Try
  {
    VertexBuffer* vbuf = mRender->GetLandRender ()->vbuf ();

    glPushAttrib (GL_ALL_ATTRIB_BITS);  

    glEnable     (GL_CLIP_PLANE0);
    glEnable     (GL_CLIP_PLANE1);
    glEnable     (GL_CLIP_PLANE2);
    glEnable     (GL_CLIP_PLANE3);
    glEnable     (GL_CLIP_PLANE4);
    glEnable     (GL_CLIP_PLANE5);

    glDisable    (GL_LIGHTING);
    glEnable     (GL_TEXTURE_2D);
    glEnable     (GL_BLEND);
    glEnable     (GL_POLYGON_OFFSET_FILL);

    glEnable     (GL_TEXTURE_GEN_S);
    glEnable     (GL_TEXTURE_GEN_T);                                                        
    glEnable     (GL_TEXTURE_GEN_Q);                                                        

    glTexGenf    (GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenf    (GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenf    (GL_Q, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);  

    glBlendFunc  (GL_SRC_COLOR,GL_ONE_MINUS_SRC_COLOR);

          //ВВести альфа-тест для корректного отображения нескольких теней!

//    glColor4f       (1,1,1,1);

    float           frsign = mRender->GetState (RENDER_CLEAR_DEPTH_BUFFER_BIT) ?
                             -1 :
                             (mRender->GetFrame () & 1) ? 1 : -1;

    for (int i=0;i<num;i++,vis++)
    {
      boundbox_t& box = mLandMap->map [vis->index].box;
      int         si  = 0;
      
      for (shadow_t* shadow=mFirst;shadow;shadow=shadow->next,si++)
        if (IsInBound (box,6,shadow->frustum.clip))
        {
          for (int k=0;k<6;k++)
          {
            plane_t& src      = shadow->frustum.clip [k];
            GLdouble plane [4] = {src.a,src.b,src.c,src.d};
            glClipPlane (GL_CLIP_PLANE0+k,plane);
          }
          
          int             num  = mLandCache->GetNodeInfo (vis->index).chunk [GRC_CHUNK_STRIP].count;
          mesh_t*         mesh = (mesh_t*)mLandCache->get (vis->index,GRC_CHUNK_STRIP);
          const matrix_t& m    = shadow->transform;

          glTexGenfv      (GL_S, GL_OBJECT_PLANE, m[0]);
          glTexGenfv      (GL_T, GL_OBJECT_PLANE, m[1]);
          glTexGenfv      (GL_Q, GL_OBJECT_PLANE, m[3]);

          glPolygonOffset (1,SHADOW_GROUND_DIST*(si+1)*frsign);
          glBindTexture   (GL_TEXTURE_2D,shadow->texture);
          glTexEnvi       (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE); 

                          //Multi draw elements or connect strips
  
          for (k=0;k<num;k++,mesh=(mesh_t*)((char*)mesh+mesh->size))
          {
            int           count = mesh->count;
            uint*         index = (uint*)mesh->data;
            mTriCount          += count / 3;

            vbuf->set      (mesh->vbuffer);
            glDrawElements (mesh->type,count,GL_UNSIGNED_INT,index);
          }
        }
    }

    glPopAttrib ();
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ShadowEnv: Error at Draw "<<endl;
    #endif
  }
}