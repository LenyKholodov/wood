#include "r_mdlsmp.h"
#include "r_mdlpool.h"
#include "r_mdlenv.h"

#include <gl\gl.h>
#include <graph\utils.h>
#include <graph\x3f.h>
#include <graph\hlpdraw.h>
#include <string.h>

static   textmat_t _DefaultMaterial;

BOOL       SimpleModelKeeper::InitModel        (model_t* mdl,MODELDATA md,va_list list)
{
  D_Try
  {
    smpmodeldata_t* data = (smpmodeldata_t*)LockModelData (md);

    if (!data)
      return FALSE;

    mdl->data.size  = sizeof (smpmodel_t);
    mdl->data.data  = GetModelPool ()->alloc (mdl,mdl->data.size);

    if (!mdl->data.data)
      return FALSE;    

    vector3d_t&     pos  = va_arg (list,vector3d_t);
    matrix_t&       m    = va_arg (list,matrix_t);   
    smpmodel_t*     prop = (smpmodel_t*)mdl->data.data;
    bound_t         b    = data->bound;
    char*           name = ((textmat_t*)(data->data + data->mat.offs))[0].text; 

    InitPosition (mdl,b.box,pos,m);

    prop->texture        = -1;
    prop->list           = -1;
    prop->mat            = data->mat.num ?
                           *((textmat_t*)(data->data + data->mat.offs)) :
                           _DefaultMaterial;
    prop->crc            = CRC32 (name,strlen(name));
    prop->pgCount        = data->facet.num;

    UnlockModelData     (md);
      
    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"SimpleModelKeeper: Error at InitModel "<<endl;
    #endif
    return FALSE;
  }
}

void       SimpleModelKeeper::render           (const modelnode_t* node,const Camera&)
{
  for (;node;node=node->next)
  {
    model_t*          model = node->model;
    smpmodel_t*       prop  = (smpmodel_t*)model->data.data;
    textmat_t&        mat   = prop->mat;

                      //Устанавливаем матрицу видовых преобразований

    glPushMatrix   ();   
    #ifdef DEBUG
      DrawBoundBox   (model->pos.box);
    #endif  
    glMultMatrixf  ((GLfloat*)&prop->trans);
                   
                     //Устанавливаем материал

    if (prop->texture > 0 && prop->crc == GetMatEnv()->GetCRC (prop->texture))
      SetMaterial (prop->texture,RENDER_TEXT_SET);
    else if (prop->texture != -2)
    {
      prop->texture  = GetMatEnv ()->AllocTexture (mat.text,prop->crc);
      
      if (!SetMaterial (prop->texture,RENDER_TEXT_SET) && prop->texture != -1)
      {
        GetMatEnv ()->FreeTexture (prop->texture);
        prop->texture = -2;
      }
    }  
    else
      GetMatEnv ()->unbind ();
  
    ::SetMaterial  (mat.m);

    if (mat.m.blend)
      glEnable (GL_BLEND);

                     //Рисуем

    if (!mMeshRender.DrawMesh (prop->list,model->modelData))
    {
      if (mMeshRender.BeginMesh (model->modelData,prop->list))
      {                        
        smpmodeldata_t*   data  = (smpmodeldata_t*)LockModelData (model->modelData);    

        if (!data)
          continue;

        vertex_t*    vertex   = (vertex_t*)(data->data + data->vertex.offs);
        vector3d_t*  normal   = (vector3d_t*)(data->data + data->normal.offs);

        if (data->strip.num)
        {
          uint*         index  = (uint*)(data->data + data->strip.offs);
          textcoord_t*  tcoord = (textcoord_t*)(data->data + data->tcoord.offs);
          int           count  = data->strip.num;

          glBegin (GL_TRIANGLE_STRIP);
            for (int i=0;i<count;i++,index++)
            {
              glNormal3fv     ((GLfloat*)(normal + *index));
              glTexCoord2fv   ((GLfloat*)(tcoord + *index));
              glVertex3fv     ((GLfloat*)(vertex + *index));
            }  
          glEnd   ();
        }
        else
        {
          facet_t*     facet    = (facet_t*)(data->data + data->facet.offs);
          int          fCount   = data->facet.num;     

          glBegin (GL_TRIANGLES);
            for (int i=0;i<fCount;i++,facet++)
            {
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
        }          

        mMeshRender.EndMesh ();                

        UnlockModelData (model->modelData);
      }        

      mMeshRender.DrawMesh  (prop->list,model->modelData);

      glDisable (GL_BLEND);
    }
                
    mPgCount += prop->pgCount;

    glPopMatrix ();
  }
}

void           SimpleModelKeeper::RenderShadow     (const model_t* model)
{
  smpmodel_t*       prop  = (smpmodel_t*)model->data.data;

  glPushMatrix   ();   
  glMultMatrixf  ((GLfloat*)&prop->trans);
                
                   //Рисуем

  if (!mMeshRender.DrawMesh (prop->list,model->modelData))
  {
    if (mMeshRender.BeginMesh (model->modelData,prop->list))
    {                        
      smpmodeldata_t*   data  = (smpmodeldata_t*)LockModelData (model->modelData);    

      if (!data)
        return;

      vertex_t*    vertex   = (vertex_t*)(data->data + data->vertex.offs);
      vector3d_t*  normal   = (vector3d_t*)(data->data + data->normal.offs);

      if (data->strip.num)
      {
        uint*         index  = (uint*)(data->data + data->strip.offs);
        textcoord_t*  tcoord = (textcoord_t*)(data->data + data->tcoord.offs);
        int           count  = data->strip.num;

        glBegin (GL_TRIANGLE_STRIP);
          for (int i=0;i<count;i++,index++)
          {
            glNormal3fv     ((GLfloat*)(normal + *index));
            glTexCoord2fv   ((GLfloat*)(tcoord + *index));
            glVertex3fv     ((GLfloat*)(vertex + *index));
          }  
        glEnd   ();
      }
      else
      {
        facet_t*     facet    = (facet_t*)(data->data + data->facet.offs);
        int          fCount   = data->facet.num;     

        glBegin (GL_TRIANGLES);
          for (int i=0;i<fCount;i++,facet++)
          {
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
      }          

      mMeshRender.EndMesh ();                

      UnlockModelData (model->modelData);
    }        

    mMeshRender.DrawMesh  (prop->list,model->modelData);
  }
              
  mPgCount += prop->pgCount;

  glPopMatrix   ();   
}

SimpleModelKeeper::SimpleModelKeeper   (Pool* _Pool,ModelEnv* _ModelEnv,int _MeshesNum)
                  : M3DModelKeeper (_Pool,_ModelEnv),
                    mMeshRender (_Pool,_MeshesNum)
{    }

SimpleModelKeeper::~SimpleModelKeeper  ()
{    }
