#include "pch.h"
#include "r_mdl3d.h"
#include "r_mdlenv.h"

#include <graph\utils.h>
#include <binfile.h>

BOOL       M3DModelKeeper::InitPosition     
 ( model_t*             mdl,
   const boundbox_t&    box,
   const vector3d_t&    pos,
   const matrix_t&      trans
 )
{
  D_Try
  {
    if (!mdl->data.data)
      return FALSE;    

    bound_t         b; 
    boundbase_t     base;
    matrix_t        m    = trans;

    m3dmodel_t*     prop = (m3dmodel_t*)mdl->data.data;
    b.box                = box;

    vector3d_t      d    = b.box.max - b.box.min;

    base.axis [0]        = vector3d_t (d.x,0,0);
    base.axis [1]        = vector3d_t (0,d.y,0);
    base.axis [2]        = vector3d_t (0,0,d.z);
    base.pos             = b.box.min;

    base                 = m * base;   

    vector3d_t c         = (base.axis [0] + base.axis [1] + base.axis [2]) / 2.0f;
    base.pos            += pos;

    CalcBound           (base,b);

    mdl->pos.pos        = pos;
    mdl->pos.box        = b.box;

    m                   = matrix_t::translate (pos) * m;

    m.transpose         ();

    prop->box           = box;
    prop->trans         = m;
    prop->base          = base;
    prop->pgCount       = 0;

    mModelEnv->InsertModel (mdl);
      
    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"m3DModelKeeper: Error at InitPosition "<<endl;
    #endif
    return FALSE;
  }
}

void       M3DModelKeeper::DoneModel        (model_t* mdl)
{
  D_Try
  {
    if (mdl) GetModelPool ()->free (mdl);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"M3DModelKeeper: Error at DoneModel "<<endl;
    #endif
  }
}

void       M3DModelKeeper::ChangeProperties (model_t* mdl,va_list list)
{    
  switch (va_arg (list,uint))
  {
    case MODEL_CHANGE_MATRIX:
    {
        //prop->trans * x = m --> x = m / prop->trans

      m3dmodel_t*     prop  = (m3dmodel_t*)mdl->data.data;

      if (!prop) return;

      prop->trans.transpose ();

      bound_t         b;
      matrix_t        m     = va_arg (list,matrix_t);
      matrix_t        inv   = prop->trans; inv.invert ();
      matrix_t        trans = matrix_t::translate (mdl->pos.pos) * m * inv;

      prop->trans           = trans * prop->trans;
      prop->base            = trans * prop->base;

      CalcBound (prop->base,b);

      mdl->pos.pos          = trans * mdl->pos.pos;
      mdl->pos.box          = b.box;

      prop->trans.transpose ();

      GetModelEnv()->InsertModel (mdl);

      break;
    }  
    case MODEL_MULT_MATRIX:
    {
      m3dmodel_t*     prop  = (m3dmodel_t*)mdl->data.data;

      if (!prop) return;

      prop->trans.transpose ();

      bound_t         b;
      matrix_t&       trans = va_arg (list,matrix_t);
      matrix_t        x     = matrix_t::translate (mdl->pos.pos) * 
                              trans *                               
                              matrix_t::translate (-mdl->pos.pos);
      trans                 = x * prop->trans;

      prop->trans           = trans;
      prop->base            = x * prop->base;

      CalcBound (prop->base,b);

      mdl->pos.pos          = x * mdl->pos.pos;
      mdl->pos.box          = b.box;

      prop->trans.transpose ();

      GetModelEnv()->InsertModel (mdl);

      break;
    }  
    case MODEL_SCALE:
    {
      m3dmodel_t*     prop  = (m3dmodel_t*)mdl->data.data;

      if (!prop) return;

      prop->trans.transpose ();

      bound_t         b;
      matrix_t        trans = matrix_t::scale (va_arg (list,vector3d_t));
      matrix_t        x     = matrix_t::translate (mdl->pos.pos) * 
                              trans *                               
                              matrix_t::translate (-mdl->pos.pos);
      trans                 = x * prop->trans;

      prop->trans           = trans;
      prop->base            = x * prop->base;

      CalcBound (prop->base,b);

      mdl->pos.pos          = x * mdl->pos.pos;
      mdl->pos.box          = b.box;

      prop->trans.transpose ();

      GetModelEnv()->InsertModel (mdl);

      break;
    }
    case MODEL_TRANSFORM_ROUND_POINT:
    {
      m3dmodel_t*     prop  = (m3dmodel_t*)mdl->data.data;

      if (!prop) return;

      prop->trans.transpose ();

      bound_t         b;
      matrix_t&       trans = va_arg (list,matrix_t);
      vector3d_t&     pivot = va_arg (list,vector3d_t);
      matrix_t        x     = matrix_t::translate (mdl->pos.pos - pivot) * 
                              trans *
                              matrix_t::translate (-mdl->pos.pos + pivot);
      trans                 = x * prop->trans;

      prop->trans           = trans;
      prop->base            = x * prop->base;

      CalcBound (prop->base,b);

      mdl->pos.pos          = x * mdl->pos.pos;
      mdl->pos.box          = b.box;

      prop->trans.transpose ();

      GetModelEnv()->InsertModel (mdl);

      break;
    }
    case MODEL_GET_MATRIX:
    {
      m3dmodel_t*     prop  = (m3dmodel_t*)mdl->data.data;

      if (!prop) return;

      matrix_t*       trans = va_arg (list,matrix_t*);

      *trans                = prop->trans;      
      trans->transpose ();

      break;
    }
    case MODEL_GET_POS:
    {
      vector3d_t*     pos   = va_arg (list,vector3d_t*);
      *pos                  = mdl->pos.pos;  

      break;
    }
    default: return;
  }
}

void       M3DModelKeeper::SetPosition      (model_t* mdl,const vector3d_t& v)
{
  vector3d_t   oldPos = mdl->pos.pos;
  vector3d_t   d      = v - oldPos;
  m3dmodel_t*  prop   = (m3dmodel_t*)mdl->data.data;

  if (!prop)
    return;

  ModelKeeper::SetPosition (mdl,v);
  
  mdl->pos.box.min  += d;
  mdl->pos.box.max  += d;

  if (prop)
  {
    prop->base.pos += d;  
    prop->trans.transpose ();
    prop->trans     = matrix_t::translate (d) * prop->trans;
    prop->trans.transpose ();
  }
}

void       M3DModelKeeper::SetBoundBox      (model_t* mdl,const boundbox_t& box)
{
  ModelKeeper::SetBoundBox (mdl,box);

  mdl->pos.pos  = (box.max + box.min)/2.0f;
}

int        M3DModelKeeper::GetModelDataSize (va_list src,va_list dest)
{
  D_Try
  {
    if (strlen (src) > 128)
    {
      #ifdef DEBUG
        dout<<"M3DModelKeeper: Model name too long: '"<<src<<"'"<<endl;
      #endif
      return -1;
    }

    strcpy (dest,*((char**)src));

    BinFile* file = F_OpenFile (dest,BF_OPEN_RO);

    if (!IsValid (file))
    {
      #ifdef DEBUG
        dout<<"M3DModelKeeper: File not found: '"<<dest<<"'"<<endl;
      #endif
      return -1;
    }

    m3dheader_t hdr;

    file->read (&hdr,sizeof (hdr));

    if (hdr.id != filetype ())
    {
      #ifdef DEBUG
        dout<<"M3DModelKeeper: Wrong model format '"<<dest<<"'"<<endl;
      #endif
      delete file;
      return -1;
    }    

    delete file;

    return hdr.size;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"M3DModelKeeper: Error at GetModelDataSize "<<endl;
    #endif
    return -1;
  }
}
            
BOOL       M3DModelKeeper::LoadModelData    (void* data,size_t size,va_list name)
{
  D_Try
  {
    BinFile* file = F_OpenFile (name,BF_OPEN_RO);

    if (!IsValid (file))
    {
      #ifdef DEBUG
        dout<<"M3DModelKeeper: File not found: '"<<name<<"'"<<endl;
      #endif
      return FALSE;
    }

    m3dheader_t hdr;

    file->read (&hdr,sizeof (hdr));

    if (hdr.id != filetype ())
    {
      #ifdef DEBUG
        dout<<"M3DModelKeeper: Wrong model format '"<<name<<"'"<<endl;
      #endif
      delete file;
      return FALSE;
    }    

    D_Try
    {
      file->read (data,size >= hdr.size ? hdr.size : size);
    }
    D_Exception (EXCEPTION_EXECUTE_HANDLER)
    {           
      #ifdef DEBUG
        dout<<"M3DModelKeeper: Error at copy model data "<<endl;
      #endif
      delete file;
      return FALSE;
    }

    delete file;

    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"M3DModelKeeper: Error at LoadModelData "<<endl;
    #endif
    return FALSE;
  }
}

BOOL       M3DModelKeeper::UnloadModelData  (void*,size_t,va_list)
{
  return TRUE;
}

M3DModelKeeper::M3DModelKeeper   (Pool* _Pool,ModelEnv* _ModelEnv)
               : ModelKeeper (_Pool,_ModelEnv)
{
}

M3DModelKeeper::~M3DModelKeeper  ()
{
}
