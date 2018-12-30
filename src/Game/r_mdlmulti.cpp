#include "pch.h"
#include "r_mdlmulti.h"
#include "r_mdlpool.h"
#include "r_main.h"
#include "r_mdlenv.h"

#include <gl\gl.h>
#include <graph\utils.h>
#include <graph\x3f.h>
#include <graph\hlpdraw.h>

static   textmat_t _DefaultMaterial;

BOOL       MultiModelKeeper::InitModel        (model_t* mdl,MODELDATA md,va_list list)
{
  D_Try
  {
    vector3d_t&     pos  = va_arg (list,vector3d_t);
    matrix_t&       m    = va_arg (list,matrix_t);

    multimodeldata_t* data = (multimodeldata_t*)LockModelData (md);

    if (!data)
      return FALSE;

    mdl->data.size  = sizeof (multimodel_t) + 
                      sizeof (int) * data->mesh.num +
                      data->matname.num * sizeof (int);
    mdl->data.data  = GetModelPool ()->alloc (mdl,mdl->data.size);

    if (!mdl->data.data)
      return FALSE;    

    multimodel_t*     prop = (multimodel_t*)mdl->data.data;
    bound_t           b    = data->bound;

    InitPosition (mdl,b.box,pos,m);

    prop->num           = data->matname.num;
    prop->listTable     = prop->data;
    prop->reindex       = prop->data + data->mesh.num * sizeof (int); 

    ulong*      offs    = (ulong*)(data->data + data->mesh.offs);

    for (int i=0;i<data->mesh.num;i++)
    {
      prop->listTable    [i] = -1;

      mesh_t*      mesh      = (mesh_t*)(data->data + offs [i]);
      prop->pgCount         += mesh->count; 
    }

    char* str = data->data + data->matname.offs;

    for (i=0;i<data->matname.num;i++,str+=strlen (str)+1)
      prop->reindex [i] = mainrender()->GetMatEnvX()->SearchMat (str);
    mainrender()->GetMatEnvX()->AddMat (mMatGroup,prop->reindex,data->matname.num);

    if (data->mesh.num >= 2048)
    {
      #ifdef DEBUG
        dout<<"MultiModelKeeper: Warning! To many materials in model ("<<data->mesh.num
            <<"). This is make error at MeshRender ID"<<endl;
      #endif
    }

    UnlockModelData     (md);
      
    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"MultiModelKeeper: Error at InitModel (modeldata: "<<md<<")"<<endl;
    #endif
    return FALSE;
  }
}

void       MultiModelKeeper::render    (const modelnode_t* node,const Camera& eye)
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

    for (int i=0;i<prop->num;i++)
    {
      if (mainrender()->GetMatEnvX ()->GetMat (prop->reindex [i]).color.blend)
      {
        if (!alp)
        {
          AddAlpha (length (model->pos.pos-eye.pos),(DWORD)model);
          alp = TRUE;                                            
        }  

        continue;
      }

                     //Устанавливаем материал

      mainrender()->GetMatEnvX ()->SetMat (prop->reindex [i]);
      mainrender()->GetMatEnvX ()->bind   (prop->reindex [i]);

                       //Рисуем

      if (!mMeshRender.DrawMesh (prop->listTable [i],(model->modelData<<11)+i))
      {
        if (mMeshRender.BeginMesh ((model->modelData<<11)+i,prop->listTable [i]))
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
          mesh_t*      mesh     = (mesh_t*)(data->data + offs [i]);
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

        mMeshRender.DrawMesh  (prop->listTable [i],model->modelData);
      }
    }

    if (data) UnlockModelData (model->modelData);

    mPgCount += prop->pgCount;

    glPopMatrix ();
  }
}

void       MultiModelKeeper::RenderShadow    (const model_t* model,const lightsrc_t&)
{
  multimodel_t*     prop  = (multimodel_t*)model->data.data;
  multimodeldata_t* data  = NULL;

  glPushMatrix   ();   
  glMultMatrixf  ((GLfloat*)&prop->trans);

  glPushAttrib   (GL_ENABLE_BIT);

  for (int i=0;i<prop->num;i++)
  {
    if (mainrender()->GetMatEnvX ()->GetMat (prop->reindex [i]).color.blend)
    {
      glEnable       (GL_ALPHA_TEST);
      glEnable       (GL_TEXTURE_2D);
      glAlphaFunc    (GL_LESS,RENDER_MODEL_SHADOW_ALPHA);

      mainrender()->GetMatEnvX ()->bind   (prop->reindex [i]);
    }  

    if (!mMeshRender.DrawMesh (prop->listTable [i],(model->modelData<<11)+i))
    {
      if (mMeshRender.BeginMesh ((model->modelData<<11)+i,prop->listTable [i]))
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
        mesh_t*      mesh     = (mesh_t*)(data->data + offs [i]);
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

      mMeshRender.DrawMesh  (prop->listTable [i],model->modelData);
    }

    if (mainrender()->GetMatEnvX ()->GetMat (prop->reindex [i]).color.blend)
    {
      mainrender()->GetMatEnvX ()->bind   (prop->reindex [i]);

      glDisable      (GL_ALPHA_TEST);
      glDisable      (GL_TEXTURE_2D);
    }  

    if (data) UnlockModelData (model->modelData);
  }

  glPopAttrib    ();

  mPgCount += prop->pgCount;              

  glPopMatrix ();
}

void   MultiModelKeeper::DrawAlpha   (DWORD param,const Camera& eye)
{
  model_t*          model = (model_t*)param;
  multimodel_t*     prop  = (multimodel_t*)model->data.data;
  multimodeldata_t* data  = NULL;

                    //Устанавливаем матрицу видовых преобразований

  glPushMatrix   ();   
//  #ifdef DEBUG
//    DrawBoundBox   (model->pos.box);
//  #endif  
  glMultMatrixf  ((GLfloat*)&prop->trans);

  for (int i=0;i<prop->num;i++)
  {
    if (!mainrender()->GetMatEnvX ()->GetMat (prop->reindex [i]).color.blend)
      continue;
           
                   //Устанавливаем материал

    mainrender()->GetMatEnvX ()->SetMat (prop->reindex [i]);
    mainrender()->GetMatEnvX ()->bind   (prop->reindex [i]);
    ChangeEnv                           (model->pos.tile);

                     //Рисуем

    if (!mMeshRender.DrawMesh (prop->listTable [i],(model->modelData<<11)+i))
    {
      if (mMeshRender.BeginMesh ((model->modelData<<11)+i,prop->listTable [i]))
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
        mesh_t*      mesh     = (mesh_t*)(data->data + offs [i]);
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

      mMeshRender.DrawMesh  (prop->listTable [i],model->modelData);
    }
  }

  if (data) UnlockModelData (model->modelData);

  mPgCount += prop->pgCount;

  glPopMatrix ();
}

MultiModelKeeper::MultiModelKeeper   (Pool* _Pool,ModelEnv* _ModelEnv,int _MeshesNum)
                  : M3DModelKeeper (_Pool,_ModelEnv),
                    mMeshRender (_Pool,_MeshesNum),
                    mMatGroup   (-1)
{    
  mMatGroup = mainrender ()->GetMatEnvX()->CreateMatGroup (NULL,0,16); //!!!!!no!!!
}

MultiModelKeeper::~MultiModelKeeper  ()
{    
  if (mMatGroup != -1) mainrender ()->GetMatEnvX()->DeleteMatGroup (mMatGroup);
}
