          facetpack_t* fpack  = (facetpack_t*)mLandCache->get (vis->index,GRC_CHUNK_FACET_PACK);
          int          fcount = mLandCache->GetNodeInfo (vis->index).chunk [GRC_CHUNK_FACET_PACK].count;
          const matrix_t& m   = shadow->transform;
          mTriCount          += fcount;
          
          if (!fpack)
            break;

          glTexGenfv      (GL_S, GL_OBJECT_PLANE, m[0]);
          glTexGenfv      (GL_T, GL_OBJECT_PLANE, m[1]);
          glTexGenfv      (GL_Q, GL_OBJECT_PLANE, m[3]);

          glPolygonOffset (1,-SHADOW_GROUND_DIST*(si+1));
          glBindTexture   (GL_TEXTURE_2D,shadow->texture);
//          glTexEnvi     (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE); //!!!!
          glTexEnvi       (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE); 

          glBegin (GL_TRIANGLES);

          for (int j=0;j<fcount;j++,fpack++)
          {
            glVertex3fv   ((GLfloat*)(vertex + fpack->facet.index [0]));
            glVertex3fv   ((GLfloat*)(vertex + fpack->facet.index [1]));
            glVertex3fv   ((GLfloat*)(vertex + fpack->facet.index [2]));
          }    

          glEnd ();            