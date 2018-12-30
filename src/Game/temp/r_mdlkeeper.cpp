#include "r_mdlkeeper.h"
#include "r_mdlenv.h"

BOOL       ModelKeeper::InitModel      (MODEL mdl,va_list list)
{
  D_Try
  {
    if (!_InitModel (mdl,list))
      return FALSE;

    mModelRing.insert (mdl);

    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {           
    #ifdef DEBUG
      dout<<"ModelKeeper: Error at InitModel "<<endl;
    #endif
    return FALSE;
  }
}

BOOL      ModelKeeper::_InitModel       (const MODEL src,MODEL dest)
{
  *dest          = *src;

  dest->prop     = mModelEnv->AllocModelData (&dest->prop,dest->propSize);
  dest->dataSize = 0;

  if (!dest->prop) return FALSE;

  memcpy (src->prop,dest->prop,dest->propSize);

  return TRUE;
}

BOOL      ModelKeeper::InitModel   (const MODEL src,MODEL dest)
{
  D_Try
  {
    if (!_InitModel (src,dest))
      return FALSE;

    mModelRing.insert (dest);

    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelKeeper: Error at DuplicateModel "<<endl;
    #endif
    return NULL;
  }
}

void       ModelKeeper::DoneModel      (MODEL mdl)
{  
  D_Try
  {
    mModelRing.erase (mdl);

    _DoneModel       (mdl);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
    #endif
  }
}

void       ModelKeeper::SetPosition      (MODEL mdl,const vector3d_t& v)
{
  mdl->pos = v;
}

void       ModelKeeper::SetTransMatrix   (MODEL mdl,const matrix_t& m)
{
  mdl->m = m;
}

void       ModelKeeper::SetBoundBox      (MODEL mdl,const boundbox_t& box)
{
  mdl->box = box;
}

void       ModelKeeper::RenderScene      (modelnode_t* node)
{
  D_Try
  {
    render (node);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelKeeper: Error at RenderScene "<<endl;
    #endif
  }
}

BOOL       ModelKeeper::AllocModelData   (MODEL mdl,size_t propSize,size_t dataSize)
{
  D_Try
  {
    mdl->prop     = propSize ? mModelEnv->AllocModelData (&mdl->prop,propSize) : NULL;
    mdl->data     = dataSize ? mModelEnv->AllocModelData (&mdl->data,dataSize) : NULL;

    BOOL status   = (mdl->data != NULL && dataSize) && 
                    (mdl->prop != NULL && propSize);

    mdl->propSize = mdl->prop ? propSize : 0;
    mdl->dataSize = mdl->data ? dataSize : 0;

    return status;
  } 
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelKeeper: Error at AllocModelData "<<endl;
    #endif
    return FALSE;
  }     
}

void       ModelKeeper::FreeModelData    (MODEL mdl)
{
  D_Try
  {
    if (mdl->dataSize) mModelEnv->FreeModelData (&mdl->data);
    if (mdl->propSize) mModelEnv->FreeModelData (&mdl->prop);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelKeeper: Error at FreeModelData "<<endl;
    #endif
  }
}

void       ModelKeeper::Done  ()
{
  D_Try
  {
    MdlRing::iterator iter  = mModelRing;
    int               count = mModelRing.count ();

    for (int i=0;i<count;i++,iter++)
      mModelEnv->DeleteModel (iter);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"ModelKeeper: Error at Done "<<endl;
    #endif
  }
}

ModelKeeper::ModelKeeper  (Pool* _Pool,ModelEnv* _MdlEnv)
            : MemObject   (_Pool),
              mModelEnv   (_MdlEnv),
              mModelRing  (_Pool)
{   }

ModelKeeper::~ModelKeeper ()
{   
  Done ();
}

/////////////////////implementation of class TestKeeper/////////////////////////

#include <gl\gl.h>

struct testkprprop_t
{
  boundbox_t  box;
};

void       TestKeeper::SetPosition      (MODEL mdl,const vector3d_t& v)
{
  ModelKeeper::SetPosition (mdl,v);

  mdl->box       = ((testkprprop_t*)mdl->prop)->box;
  mdl->box.min  += v;
  mdl->box.max  += v;
}

BOOL           TestKeeper::_InitModel       (MODEL mdl,va_list list) 
{
  boundbox_t box = va_arg (list,boundbox_t);

  AllocModelData (mdl,sizeof (testkprprop_t),0);

  ((testkprprop_t*)mdl->prop)->box = box;
  mdl->pos                         = (box.max+box.min)/2.0f;
  mdl->box                         = box;

  return TRUE; 
}

void drawBox (const boundbox_t& box)
{
  vertex_t   v0 (box.min.x,     box.min.y,      box.max.z);
  vertex_t   v1 (box.max.x,     box.min.y,      box.max.z);
  vertex_t   v2 (box.max.x,     box.min.y,      box.min.z);
  vertex_t   v3 (box.min.x,     box.min.y,      box.min.z);
  vertex_t   v4 (box.min.x,     box.max.y,      box.max.z);
  vertex_t   v5 (box.max.x,     box.max.y,      box.max.z);
  vertex_t   v6 (box.max.x,     box.max.y,      box.min.z);
  vertex_t   v7 (box.min.x,     box.max.y,      box.min.z);  

  glColor3f (0.8,0.8,0.8);

  glBegin (GL_QUADS);
    glVertex3fv ((GLfloat*)&v0);
    glVertex3fv ((GLfloat*)&v1);
    glVertex3fv ((GLfloat*)&v2);
    glVertex3fv ((GLfloat*)&v3);

    glVertex3fv ((GLfloat*)&v4);
    glVertex3fv ((GLfloat*)&v5);
    glVertex3fv ((GLfloat*)&v6);
    glVertex3fv ((GLfloat*)&v7);

    glVertex3fv ((GLfloat*)&v0);
    glVertex3fv ((GLfloat*)&v4);
    glVertex3fv ((GLfloat*)&v5);
    glVertex3fv ((GLfloat*)&v3);

    glVertex3fv ((GLfloat*)&v1);
    glVertex3fv ((GLfloat*)&v5);
    glVertex3fv ((GLfloat*)&v6);
    glVertex3fv ((GLfloat*)&v2);

    glVertex3fv ((GLfloat*)&v2);
    glVertex3fv ((GLfloat*)&v6);
    glVertex3fv ((GLfloat*)&v7);
    glVertex3fv ((GLfloat*)&v3);

    glVertex3fv ((GLfloat*)&v3);
    glVertex3fv ((GLfloat*)&v7);
    glVertex3fv ((GLfloat*)&v4);
    glVertex3fv ((GLfloat*)&v0);
  glEnd   ();  
}


void           TestKeeper::render           (modelnode_t* node)
{
  for (;node;node=node->next)
    drawBox (node->model->box);
}

TestKeeper::TestKeeper  (Pool* _Pool,ModelEnv* _Env)
           : ModelKeeper (_Pool,_Env)
{  }

TestKeeper::~TestKeeper ()
{  }
