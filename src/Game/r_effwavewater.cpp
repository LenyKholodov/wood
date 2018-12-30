#include "pch.h"
#include "r_effwavewater.h"
#include "r_mdlenv.h"

#include <misc.h>
#include <graph\utils.h>
#include <graph\hlpdraw.h>
#include <gl\gl.h>

const    int    U_NUM   = 8;
const    int    V_NUM   = 8;
const    float  STEP    = 10.0f;

/////////////////////////implementation of class WaveWaterEffect////////////////

void WaveWaterEffect::tick  ()
{
  if (clock () - mLast > RENDER_WAVE_WATER_REFRESH_TIME)
  {
    for (int v=0;v<V_NUM;v++)
      for (int u=0;u<U_NUM;u++)
      {
        int index       = v*U_NUM + u;
        mVertex [index] += mDir [index];
        
        if (mVertex [index].y > mZAmp + mBBox.min.y)  
        {
          mDir [index]      = -vector3d_t (0,randomfl () * mZAmp / STEP,0);
          mVertex [index].y = mZAmp + mBBox.min.y;
        }  
        if (mVertex [index].y < -mZAmp + mBBox.min.y) 
        {
          mDir [index]      = vector3d_t (0,randomfl () * mZAmp / STEP,0);
          mVertex [index].y = -mZAmp + mBBox.min.y;
        }  
      }

    refresh ();  

    mLast = clock ();
  }
}

void WaveWaterEffect::draw  ()
{
  SetTexture    (mTexture,mCRC,mTextName);

  #ifdef DEBUG
    DrawBoundBox(mBBox);
  #endif  

  glEnable      (GL_MAP2_VERTEX_3);
  glEnable      (GL_MAP2_TEXTURE_COORD_2);
  glEnable      (GL_AUTO_NORMAL);
  glEnable      (GL_BLEND);

//  glTexGeni     (GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); 
//  glTexGeni     (GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);            
//  glEnable      (GL_TEXTURE_GEN_S);
//  glEnable      (GL_TEXTURE_GEN_T);

  glDepthMask   (GL_FALSE);

//  glBlendFunc   (GL_SRC_COLOR,GL_ONE_MINUS_SRC_COLOR);
  glBlendFunc   (GL_SRC_COLOR,GL_ONE_MINUS_SRC_COLOR);

  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

  glShadeModel  (GL_SMOOTH);
  glDisable     (GL_CULL_FACE);

  glColor4f     (1, 1, 1, 0.75f);

  glMapGrid2d   (U_NUM*2, 0.0, 1.0, V_NUM*2, 0.0, 1.0);
  glEvalMesh2   (GL_FILL, 0, U_NUM*2, 0, V_NUM*2);

  glDisable     (GL_MAP2_VERTEX_3);
  glDisable     (GL_MAP2_TEXTURE_COORD_2);
  glEnable      (GL_CULL_FACE);
  glDisable     (GL_BLEND);
  glDisable     (GL_AUTO_NORMAL);
  glDisable     (GL_TEXTURE_GEN_S);
  glDisable     (GL_TEXTURE_GEN_T);

  glDepthMask   (GL_TRUE);

  AddPgCount (V_NUM * U_NUM * 2);
}

void WaveWaterEffect::refresh ()
{
  glMap2f  (GL_MAP2_VERTEX_3, 
            0,1,3,U_NUM, 
            0,1,U_NUM*3,V_NUM,
            (GLfloat*)mVertex
           );
  glMap2f  (GL_MAP2_TEXTURE_COORD_2, 
            0,1,2,U_NUM, 
            0,1,U_NUM*2,V_NUM,
            (GLfloat*)mTextCoord
           );  
}

WaveWaterEffect::WaveWaterEffect  (Pool* _Pool,
                                   const boundbox_t& box,
                                   const char* texName,
                                   WaterModelKeeper* kpr
                                  )
  : WaterEffect (_Pool,kpr),
    mVertex   (NULL),
    mTextCoord (NULL),
    mDir       (NULL),
    mBBox      (box),
    mZAmp      ((box.max.y - box.min.y) / 2.0f),
    mTexture   (-1),
    mCRC       (0),
    mLast      (clock ())
{
  D_Try
  {
    strcpy (mTextName.text,texName);
    mTextName.m = material_t ();

    mVertex     = (vertex_t*)GetPool()->alloc (sizeof (vertex_t) * V_NUM * U_NUM);
    mDir        = (vector3d_t*)GetPool()->alloc (sizeof (vector3d_t) * V_NUM * U_NUM);
    mTextCoord  = (textcoord_t*)GetPool()->alloc (sizeof (textcoord_t) * V_NUM * U_NUM);

    if (!mVertex || !mDir || !mTextCoord)
    {
      #ifdef DEBUG
        dout<<"WaveWaterEffect: No enough memory for sys data "<<endl;
      #endif
      return;
    }

    mCRC = CRC32 (texName,strlen (texName));

    float width  = box.max.x - box.min.x,
          height = box.max.z - box.min.z;

    for (int v=0;v<V_NUM;v++)
      for (int u=0;u<U_NUM;u++)
      {
        int          index = (V_NUM-v-1)*U_NUM + u;

        vertex_t&    p     = mVertex    [index];  
        textcoord_t& t     = mTextCoord [index];

        p.x                = width * float (u)/float (U_NUM);
        p.z                = height * float (v)/float (V_NUM);
        p.y                = (randomfl () - 0.5f )* mZAmp + 10.0f;

        p                 += box.min;

        t.u                = float (u) / float (U_NUM);
        t.v                = float (v) / float (V_NUM);

        mDir [index]       = vector3d_t (0,randomfl () * mZAmp / STEP,0);
      }

    refresh ();
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"WaveWaterEffect: Error at construct "<<endl;
    #endif
  }
}

WaveWaterEffect::~WaveWaterEffect ()
{
  D_Try
  {
    if (mVertex)        GetPool()->free (mVertex);
    if (mTextCoord)     GetPool()->free (mTextCoord);
    if (mDir)           GetPool()->free (mDir);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"WaveWaterEffect: Error at destruct "<<endl;
    #endif
  }
}

////////////////////implementation of class WaveWaterModelKeeper////////////////

void       WaterModelKeeper::SetTexture       (int& texture,ulong& crc,const textmat_t& mat)
{
/*  if (texture > 0 && crc == GetMatEnv ()->GetCRC (texture))
 //   GetModelEnv ()->SetMaterial (texture,RENDER_TEXT_SET);
   SetMaterial (texture,RENDER_TEXT_SET);
  else if (texture != -2)
  {
    texture  = GetMatEnv ()->AllocTexture (mat.text,crc);
    
//    if (!mModelEnv->SetMaterial (texture,RENDER_TEXT_SET) && texture != -1)
    if (!SetMaterial (texture,RENDER_TEXT_SET) && texture != -1)
    {
      GetMatEnv ()->FreeTexture (texture);
      texture = -2;
    }
  }  
  else
    GetMatEnv ()->unbind ();

  ::SetMaterial (mat.m);  
*/
}

void       WaterModelKeeper::tick       (model_t* mdl)
{
  D_Try
  {
    if (mdl->data.data)
      ((watermodel_t*)mdl->data.data)->effect->tick ();
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"WaterModelKeeper: Error at tick "<<endl;
    #endif
  }
}

BOOL       WaterModelKeeper::InitModel        (model_t* mdl,MODELDATA,va_list list)
{
  D_Try
  {
    uint        type    = va_arg (list,uint);
    boundbox_t& bbox    = va_arg (list,boundbox_t);
    const char* tName   = va_arg (list,const char*);

    mdl->data.size      = sizeof (watermodel_t);
    mdl->data.data      = GetModelPool ()->alloc (mdl,mdl->data.size);    

    if (!mdl->data.data)
      return FALSE;    

    InitPosition (mdl,bbox,vector3d_t (.0f));

    watermodel_t*  prop = (watermodel_t*)mdl->data.data;

    switch (type)
    {
      case EFFECT_WAVE_WATER:    prop->effect = new (mEffectPool) 
                                                WaveWaterEffect (mEffectPool,bbox,tName,this);
                                 break;               
      default: prop->effect = NULL;                                                 
    }

    return prop->effect != NULL;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"WaterModelKeeper: Error at InitModel "<<endl;
    #endif
    return FALSE;
  }
}

void       WaterModelKeeper::DoneModel        (model_t* mdl)
{
  D_Try
  {
    watermodel_t*  prop = (watermodel_t*)mdl->data.data;

    if (!prop)
      return;

    if (prop->effect) delete prop->effect;

    if (mdl) GetModelPool ()->free (mdl);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"WaterModelKeeper: Error at DoneModel "<<endl;
    #endif
  }
}

void       WaterModelKeeper::render           (const modelnode_t* node,const Camera&)
{
  for (;node;node=node->next)
    if (node->model->data.data)
      ((watermodel_t*)node->model->data.data)->effect->draw ();
}

WaterModelKeeper::WaterModelKeeper  (Pool* _Pool,ModelEnv* env,size_t _EffPoolSize)
                     : EffectModelKeeper (_Pool,env),
                       mCreatePool       (_EffPoolSize ? TRUE : FALSE),
                       mEffectPool       (_EffPoolSize ? new (_Pool) FloatPool (_Pool,_EffPoolSize) : _Pool)
{
  if (!IsValid (mEffectPool))
  {
    #ifdef DEBUG
      dout<<"WaterModelKeeper: Error at create pool"<<endl;
    #endif
    return;
  }
}

WaterModelKeeper::~WaterModelKeeper ()
{
  D_Try
  {
    if (mCreatePool) delete mEffectPool;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"WaterModelKeeper: Error at destruct "<<endl;
    #endif
  }
}

////////////////implementation of class WaterEffect/////////////////////////////

WaterEffect::WaterEffect  (Pool* _Pool,WaterModelKeeper* kpr)
            : MemObject (_Pool),
              mKeeper   (kpr)
{   }

WaterEffect::~WaterEffect ()
{   }

void WaterEffect::SetTexture       (int& texture,ulong& crc,const textmat_t& mat)
{
  mKeeper->SetTexture (texture,crc,mat);
}

void WaterEffect::AddPgCount       (uint cnt) { mKeeper->AddPgCount (cnt); }