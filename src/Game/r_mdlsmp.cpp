#include "pch.h"
#include "r_mdlsmp.h"
#include "r_mdlpool.h"
#include "r_mdlenv.h"
#include "r_main.h"

#include <gl\gl.h>
#include <graph\hlpdraw.h>

void       SimpleModelKeeper::render           (const modelnode_t* node,const Camera& eye)
{
  for (;node;node=node->next)
  {
    model_t*          model = node->model;
    multimodel_t*     prop  = (multimodel_t*)model->data.data;
    multimodeldata_t* data  = NULL;
    BOOL              alp   = FALSE;

                      //Устанавливаем матрицу видовых преобразований

    glPushMatrix   ();   
//    #ifdef DEBUG
//      DrawBoundBox   (model->pos.box);
//    #endif  
    glMultMatrixf  ((GLfloat*)&prop->trans);

    ChangeEnv      (model->pos.tile);                  

    if (mainrender()->GetMatEnvX ()->GetMat (prop->reindex [1]).color.blend)
    {
      if (!alp)
      {
        AddAlpha (length (model->pos.pos-eye.pos),(DWORD)model);
        alp = TRUE;                                            
      }  

      continue;
    }

                   //Устанавливаем материал

    mainrender()->GetMatEnvX ()->SetMat (prop->reindex [1]);
    mainrender()->GetMatEnvX ()->bind   (prop->reindex [1]);

                     //Рисуем

    if (!mMeshRender.DrawMesh (prop->listTable [1],model->modelData<<11))
    {
      if (mMeshRender.BeginMesh (model->modelData,prop->listTable [1]))
      {                                  
        if (!data)
        {
          data  = (multimodeldata_t*)LockModelData (model->modelData);          
          if (!data)
            break;
        }  

        vertex_t*    vertex   = (vertex_t*)(data->data + data->vertex.offs);
        vector3d_t*  normal   = (vector3d_t*)(data->data + data->normal.offs);
        ulong*       offs     = (ulong*)(data->data + data->mesh.offs);
        mesh_t*      mesh     = (mesh_t*)(data->data + offs [1]);
        int*         index    = (int*)mesh->data;
        int          fCount   = mesh->count;

        glBegin (GL_TRIANGLES);
          for (int i=0;i<fCount;i++,index++)
          {
            facet_t* facet  = (facet_t*)(data->data + data->facet.offs) + *index;

            glNormal3fv     ((GLfloat*)(normal + facet->index [0]));
            glTexCoord2fv   ((GLfloat*)&facet->tindex [0]);
            glVertex3fv     ((GLfloat*)(vertex + facet->index [0]));

            glNormal3fv     ((GLfloat*)(normal + facet->index [1]));
            glTexCoord2fv   ((GLfloat*)&facet->tindex [1]);
            glVertex3fv     ((GLfloat*)(vertex + facet->index [1]));

            glNormal3fv     ((GLfloat*)(normal + facet->index [2]));
            glTexCoord2fv   ((GLfloat*)&facet->tindex [2]);
            glVertex3fv     ((GLfloat*)(vertex + facet->index [2]));
          }  
        glEnd   ();

        mMeshRender.EndMesh ();
      }  

      mMeshRender.DrawMesh  (prop->listTable [1],model->modelData);
    }

    if (data) UnlockModelData (model->modelData);

    mPgCount += prop->pgCount;

    glPopMatrix ();
  }
}

void           SimpleModelKeeper::RenderShadow     (const model_t* model,const lightsrc_t&)
{
  multimodel_t*     prop  = (multimodel_t*)model->data.data;
  multimodeldata_t* data  = NULL;

  glPushMatrix   ();   
  glMultMatrixf  ((GLfloat*)&prop->trans);

  if (mainrender()->GetMatEnvX ()->GetMat (prop->reindex [1]).color.blend)
  {
    glPushAttrib   (GL_ENABLE_BIT);
    glEnable       (GL_ALPHA_TEST);
    glEnable       (GL_TEXTURE_2D);
    glAlphaFunc    (GL_LESS,RENDER_MODEL_SHADOW_ALPHA);
    mainrender()->GetMatEnvX ()->bind   (prop->reindex [1]);
  }  

  if (!mMeshRender.DrawMesh (prop->listTable [1],model->modelData))
  {
    if (mMeshRender.BeginMesh (model->modelData,prop->listTable [1]))
    {                                  
      if (!data)
      {
        data  = (multimodeldata_t*)LockModelData (model->modelData);          
        if (!data)
          return;
      }  

      vertex_t*    vertex   = (vertex_t*)(data->data + data->vertex.offs);
      vector3d_t*  normal   = (vector3d_t*)(data->data + data->normal.offs);
      ulong*       offs     = (ulong*)(data->data + data->mesh.offs);
      mesh_t*      mesh     = (mesh_t*)(data->data + offs [1]);
      int*         index    = (int*)mesh->data;
      int          fCount   = mesh->count;

      glBegin (GL_TRIANGLES);
        for (int i=0;i<fCount;i++,index++)
        {
          facet_t* facet  = (facet_t*)(data->data + data->facet.offs) + *index;

          glNormal3fv     ((GLfloat*)(normal + facet->index [0]));
          glTexCoord2fv   ((GLfloat*)&facet->tindex [0]);
          glVertex3fv     ((GLfloat*)(vertex + facet->index [0]));

          glNormal3fv     ((GLfloat*)(normal + facet->index [1]));
          glTexCoord2fv   ((GLfloat*)&facet->tindex [1]);
          glVertex3fv     ((GLfloat*)(vertex + facet->index [1]));

          glNormal3fv     ((GLfloat*)(normal + facet->index [2]));
          glTexCoord2fv   ((GLfloat*)&facet->tindex [2]);
          glVertex3fv     ((GLfloat*)(vertex + facet->index [2]));
        }  
      glEnd   ();

      mMeshRender.EndMesh ();
    }  

    mMeshRender.DrawMesh  (prop->listTable [1],model->modelData);
  }

  if (data) UnlockModelData (model->modelData);

  mPgCount += prop->pgCount;              

  glPopMatrix ();

  if (mainrender()->GetMatEnvX ()->GetMat (prop->reindex [1]).color.blend)
    glPopAttrib ();
}

SimpleModelKeeper::SimpleModelKeeper   (Pool* _Pool,ModelEnv* _ModelEnv,int _MeshesNum)
                  : MultiModelKeeper (_Pool,_ModelEnv,_MeshesNum)
{    }

SimpleModelKeeper::~SimpleModelKeeper  ()
{    }
