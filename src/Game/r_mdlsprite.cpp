#include "pch.h"
#include "r_mdlsprite.h"
#include "r_mdlenv.h"
#include "r_main.h"

#include <gl\gl.h>
#include <misc.h>
#include <graph\hlpdraw.h>
#include <graph\utils.h>

static material_t __DefMaterial;

BOOL       Sprite3DModelKeeper::InitModel        (model_t* mdl,MODELDATA md,va_list list)
{
  D_Try
  {
    matrix_t           m (1);
    bound_t            b;
    spritemodeldata_t* data    = (spritemodeldata_t*)LockModelData (md);

    if (!data)
      return FALSE;

    mdl->data.size             = sizeof (spritemodel_t) + 
                                 sizeof (int) * data->text.num * 2;
    mdl->data.data             = GetModelPool ()->alloc (mdl,mdl->data.size); 
   
    if (!mdl->data.data)
      return FALSE;    

    spritemodel_t*     prop    = (spritemodel_t*)mdl->data.data;
    vector3d_t&        pos     = va_arg (list,vector3d_t);
    float              angle   = float (va_arg (list,int)) / FL_RAD,
                       width   = va_arg (list,int),
                       height  = va_arg (list,int);
    float              d       = sqrt (sqr (width) + sqr (height)) / 2.0f;

    b.box.min                  = - vector3d_t (d);
    b.box.max                  =   vector3d_t (d);

    InitPosition        (mdl,b.box,pos,m);

    prop->angle                = float2angle (angle);
    prop->width                = width;
    prop->height               = height;

    prop->reindex              = prop->data;

    textmat_t*  mat            = (textmat_t*)(data->data + data->text.offs);

    for (int i=0;i<data->text.num;i++)
    {
//      prop->reindex [i] = mainrender()->GetMatEnvX()->SearchMat (mat [i].name);
      prop->reindex [i] = mainrender()->GetMatEnvX()->SearchMat (mat [i].text);
    }
    mainrender()->GetMatEnvX()->AddMat (mMatGroup,prop->reindex,data->text.num);

    UnlockModelData     (md);
      
    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Sprite3DModelKeeper: Error at InitModel "<<endl;
    #endif
    return FALSE;
  }
}

void       Sprite3DModelKeeper::render  (const modelnode_t* node,const Camera& eye)
{
  for (;node;node=node->next)
  {
    model_t*           model = node->model;
    vector3d_t         dir   = model->pos.pos - eye.pos;
    float              dist  = dir.length ();
    
    AddAlpha (dist,(DWORD)model);
  }  
}

void Sprite3DModelKeeper::DrawAlpha  (DWORD param,const Camera& eye)
{
  model_t*           model = (model_t*)param;
  spritemodel_t*     prop  = (spritemodel_t*)model->data.data;
  spritemodeldata_t* data  = (spritemodeldata_t*)LockModelData (model->modelData);

  if (!data || !prop)
    return;

                    //Определяем индекс спрайта

  vector3d_t        dir     = model->pos.pos - eye.pos;
  vector3d_t        up      = eye.up * prop->height;
  float             dist    = dir.length ();        
  dir                      /= dist;
  Angle             angle   = float2angle (GetAngle (dir.x,dir.z));
  int               index   = 0;
  sprite3d_t*       sprite  = (sprite3d_t*)(data->data + data->sprite.offs);

  ChangeEnv      (model->pos.tile);

  if (dist >= RENDER_SPRITE_MIN_DIST)
  {
    if (data->koef >= 0) 
    {
      index   = int (float(angle) * data->koef) % data->sprite.num;     
      sprite += index;
    }  
    else              
    {
      int         num  = data->sprite.num;

      for (int i=0;i<num && !(sprite->start < angle && sprite->end > angle);i++,sprite++,index++);
      if  (i == num) 
      {
        index--;
        sprite--;
      }  
    }

    index                   = data->sprite.num - index - 1;
    sprite                  = (sprite3d_t*)(data->data + data->sprite.offs) + index;

    textmat_t&        mat   = ((textmat_t*)(data->data + data->text.offs)) [sprite->texture];

//    #ifdef DEBUG
//      DrawBoundBox   (model->pos.box);
//    #endif

                
                     //Устанавливаем материал

    mainrender()->GetMatEnvX()->bind (prop->reindex [index]);

    glEnable (GL_COLOR_MATERIAL);

    glColor4f     (1,1,1,1); ///alpha

                   //Рисуем

    vector3d_t        flang = normalize (up ^ eye.dir) * prop->width;
    vector3d_t&       c     = model->pos.pos;
    vector3d_t        v [4] = {
      c + up - flang,
      c - up - flang,
      c - up + flang,
      c + up + flang
    };                     

    dir                     = -dir;

    glBegin (GL_QUADS);
      glNormal3fv   ((GLfloat*)&dir);

      glTexCoord2f  (1,0);  
      glVertex3fv   ((GLfloat*)&v [3]);
      glTexCoord2f  (1,1);  
      glVertex3fv   ((GLfloat*)&v [2]);
      glTexCoord2f  (0,1);  
      glVertex3fv   ((GLfloat*)&v [1]);
      glTexCoord2f  (0,0);  
      glVertex3fv   ((GLfloat*)&v [0]);
    glEnd   ();
  }                           

  glDisable (GL_COLOR_MATERIAL);

  mPgCount += 2;

  UnlockModelData (model->modelData);
}

void Sprite3DModelKeeper::RenderShadow  (const model_t* model,const lightsrc_t& light)
{
  spritemodel_t*     prop  = (spritemodel_t*)model->data.data;
  spritemodeldata_t* data  = (spritemodeldata_t*)LockModelData (model->modelData);

  if (!data || !prop)
    return;

                    //Определяем индекс спрайта

  vector3d_t        dir     = model->pos.pos - light.light.pos;
  vector3d_t        up      = vector3d_t (0,1,0) * prop->height; //not global up vector!!!
  float             dist    = dir.length ();        
  dir                      /= dist;
  Angle             angle   = float2angle (GetAngle (dir.x,dir.z));
  int               index   = 0;
  sprite3d_t*       sprite  = (sprite3d_t*)(data->data + data->sprite.offs);

  if (dist >= RENDER_SPRITE_MIN_DIST)
  {
    if (data->koef >= 0) 
    {
      index   = int (float(angle) * data->koef) % data->sprite.num;     
      sprite += index;
    }  
    else              
    {
      int         num  = data->sprite.num;

      for (int i=0;i<num && !(sprite->start < angle && sprite->end > angle);i++,sprite++,index++);
      if  (i == num) 
      {
        index--;
        sprite--;
      }  
    }

    index                   = data->sprite.num - index - 1;
    sprite                  = (sprite3d_t*)(data->data + data->sprite.offs) + index;

                
                     //Устанавливаем материал

    glPushAttrib   (GL_ENABLE_BIT);
    glEnable       (GL_TEXTURE_2D);
    glEnable       (GL_BLEND);

    mainrender()->GetMatEnvX ()->bind   (prop->reindex [index]);

    glEnable      (GL_COLOR_MATERIAL);

    glColor4f     (1,1,1,1); ///alpha

                   //Рисуем

    vector3d_t        flang = normalize (up ^ light.light.dir) * prop->width;
    const
    vector3d_t&       c     = model->pos.pos;
    vector3d_t        v [4] = {
      c + up - flang,
      c - up - flang,
      c - up + flang,
      c + up + flang
    };                     

    dir                     = -dir;

    glBegin (GL_QUADS);
      glNormal3fv   ((GLfloat*)&dir);

      glTexCoord2f  (1,0);  
      glVertex3fv   ((GLfloat*)&v [3]);
      glTexCoord2f  (1,1);  
      glVertex3fv   ((GLfloat*)&v [2]);
      glTexCoord2f  (0,1);  
      glVertex3fv   ((GLfloat*)&v [1]);
      glTexCoord2f  (0,0);  
      glVertex3fv   ((GLfloat*)&v [0]);
    glEnd   ();
  }                           

  glDisable     (GL_COLOR_MATERIAL);

  mPgCount += 2;

  UnlockModelData (model->modelData);

  glPopAttrib     ();
} 

Sprite3DModelKeeper::Sprite3DModelKeeper  (Pool* _Pool,ModelEnv* _ModelEnv)
                    : M3DModelKeeper (_Pool,_ModelEnv),
                      mMatGroup      (-1)
{   
  __DefMaterial.ambient  = vector3d_t (0.8);
  __DefMaterial.specular = vector3d_t (.0f);
  __DefMaterial.diffuse  = vector3d_t (.0f);

  mMatGroup = mainrender ()->GetMatEnvX()->CreateMatGroup (NULL,0,16); //!!!!!no!!!
}

Sprite3DModelKeeper::~Sprite3DModelKeeper ()
{   
  if (mMatGroup != -1) mainrender ()->GetMatEnvX()->DeleteMatGroup (mMatGroup);
}
