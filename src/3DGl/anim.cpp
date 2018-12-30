#include <pch.h>
#include <graph\anim.h>

float    Ease    (float t,float from,float to);
void     Recalc  (int frame,vecseg_t& seg,vector3d_t& res);
void     Recalc  (int frame,quatseg_t& seg,vector3d_t&,float&);
void     Recalc  (int frame,quatseg_t& seg,matrix_t&);

BOOL     AM_GetTranslate        (int frame,const Object3D::Node& node,vector3d_t& trans)
{
  D_Try
  {
    vecseg_t*   seg  = (vecseg_t*)node.trans ();
    animseg_t*  anim = (animseg_t*)node.anim ();

    for (int i=0;i<anim->trans.count && seg->end < frame;i++,seg=i<anim->trans.count?seg+1:seg);    

    Recalc (frame,*seg,trans);

    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Error at AM_GetTranslate "<<endl;
    #endif
    return FALSE;
  }
}

BOOL     AM_GetScale            (int frame,const Object3D::Node& node,vector3d_t& scale)
{
  D_Try
  {
    vecseg_t*   seg  = (vecseg_t*)node.scale ();
    animseg_t*  anim = (animseg_t*)node.anim ();

    for (int i=0;i<anim->scale.count && seg->end < frame;i++,seg=i<anim->scale.count?seg+1:seg);    

    Recalc (frame,*seg,scale);

    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Error at AM_GetScale "<<endl;
    #endif
    return FALSE;
  }
}

BOOL     AM_GetRotate           (int frame,const Object3D::Node& node,matrix_t& m)
{
  D_Try
  {
    quatseg_t*  seg  = (quatseg_t*)node.scale ();
    animseg_t*  anim = (animseg_t*)node.anim ();

    for (int i=0;i<anim->rot.count && seg->end < frame;i++,seg=i<anim->rot.count?seg+1:seg);

    Recalc (frame,*seg,m);

    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Error at AM_GetRotate "<<endl;
    #endif
    return FALSE;
  }
}

BOOL     AM_GetRotate           (int frame,const Object3D::Node& node,vector3d_t& ort,float& angle)
{
  D_Try
  {
    quatseg_t*  seg  = (quatseg_t*)node.scale ();
    animseg_t*  anim = (animseg_t*)node.anim ();

    for (int i=0;i<anim->rot.count && seg->end < frame;i++,seg=i<anim->rot.count?seg+1:seg);

    Recalc (frame,*seg,ort,angle);

    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Error at AM_GetRotate "<<endl;
    #endif
    return FALSE;
  }
}

BOOL     AM_GetNullPosMatrix    (const Object3D::Node& node,matrix_t& m)
{
  D_Try
  {
    if (node.anim ()) 
      m = node.anim ()->start;
    else          
      return FALSE;

    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Error at AM_GetNullPosMatrix "<<endl;
    #endif
    return FALSE;
  }
}

BOOL     AM_GetPosMatrix        (int frame,Object3D::Node node,matrix_t* _Transform,matrix_t* _Parent,matrix_t* _ForChild)
{
  D_Try
  {
    if (!_Transform)
      return FALSE;

    animseg_t* anim = (animseg_t*)node.anim ();    

    if (!anim)
      return FALSE;

    vecseg_t   *trTrans = (vecseg_t*)node.trans (),  
               *trScale = (vecseg_t*)node.scale ();
    quatseg_t* trRot    = (quatseg_t*)node.rotate ();          
    vector3d_t scale, trans;
    matrix_t   rm (1);

    for (int i=0;i<anim->trans.count && trTrans->end < frame;i++,trTrans=i<anim->trans.count?trTrans+1:trTrans);
    for (i=0    ;i<anim->scale.count && trScale->end < frame;i++,trScale=i<anim->scale.count?trScale+1:trScale);
    for (i=0    ;i<anim->rot.count && trRot->end < frame;i++,trRot=i<anim->rot.count?trRot+1:trRot);

    Recalc (frame,*trRot,rm);
    Recalc (frame,*trTrans,trans);
    Recalc (frame,*trScale,scale);

    if (_Parent) *_Transform = *_Parent;
    else         *_Transform = matrix_t (1);

    *_Transform   *= matrix_t::translate (trans);
      
    *_Transform   *= rm;
    *_Transform   *= matrix_t::translate (-anim->pivot*scale);

    *_Transform   *= matrix_t::scale     (scale);

    *_Transform   *= anim->start;

    if (_ForChild)
    {
        *_ForChild  = matrix_t::translate  (trans) *
                      rm * matrix_t::scale (scale);
    }

    return TRUE;
  }     
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Error at AM_GetPosMatrix "<<endl;
    #endif
    return FALSE;
  }
}

BOOL    AM_GetSceletonMatrix    (int frame,Object3D::Node node,matrix_t& _SceletonMatrix)
{
  D_Try
  {
    animseg_t* anim = (animseg_t*)node.anim ();    

    if (!anim)
      return FALSE;

    vecseg_t   *trTrans = (vecseg_t*)node.trans (),  
               *trScale = (vecseg_t*)node.scale ();
    quatseg_t* trRot    = (quatseg_t*)node.rotate ();          
    vector3d_t scale, trans;
    matrix_t   rm (1);

    for (int i=0;i<anim->trans.count && trTrans->end < frame;i++,trTrans=i<anim->trans.count?trTrans+1:trTrans);
    for (i=0    ;i<anim->scale.count && trScale->end < frame;i++,trScale=i<anim->scale.count?trScale+1:trScale);
    for (i=0    ;i<anim->rot.count && trRot->end < frame;i++,trRot=i<anim->rot.count?trRot+1:trRot);

    Recalc (frame,*trRot,rm);
    Recalc (frame,*trTrans,trans);
    Recalc (frame,*trScale,scale);   

    _SceletonMatrix  = matrix_t::translate  (trans) *
                       rm * matrix_t::scale (scale);

    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Error at AM_GetSceletonMatrix "<<endl;
    #endif
    return FALSE;
  }
}

void            Recalc  (int frame,vecseg_t& seg,vector3d_t& res)
{
  float t = 0;

  if (frame > seg.start && frame <= seg.end)
    t = Ease ((frame - seg.start) * seg.locTime,seg.easeFrom,seg.easeTo);
  else if (frame > seg.end)
    t = 1.0f;
  else  if (frame <= seg.start)
    t = 0.0f;

  res     = seg.a * (t * t * t) + seg.b * (t * t) + seg.c * t + seg.d;
}

void    Recalc  (int frame,quatseg_t& seg,matrix_t& m)
{
  quat_t res;

  if (frame > seg.start && frame <= seg.end)
  {  
    float t = Ease ((frame - seg.start) * seg.locTime,seg.easeFrom,seg.easeTo);
    quat_t tmp1, tmp2, tmp3, tmp4, tmp5;

    lerp (tmp1,seg.v1, seg.c1, t),
    lerp (tmp2,seg.c1, seg.c2, t),
    lerp (tmp3,seg.c2, seg.v2, t),
    lerp (tmp4,tmp1, tmp2, t),
    lerp (tmp5,tmp2, tmp3, t),

    lerp (res,tmp4, tmp5, t);   

    res.normalize ();
  }
  else if (frame > seg.end)
    res = seg.v2;
  else if (frame <= seg.start)
    res = seg.v1;    

  m = res.toMatrix ();
}

void    Recalc  (int frame,quatseg_t& seg,vector3d_t& ort,float& angle)
{
  quat_t res;

  if (frame > seg.start && frame <= seg.end)
  {  
    float t = Ease ((frame - seg.start) * seg.locTime,seg.easeFrom,seg.easeTo);
    quat_t tmp1, tmp2, tmp3, tmp4, tmp5;

    lerp (tmp1,seg.v1, seg.c1, t),
    lerp (tmp2,seg.c1, seg.c2, t),
    lerp (tmp3,seg.c2, seg.v2, t),
    lerp (tmp4,tmp1, tmp2, t),
    lerp (tmp5,tmp2, tmp3, t),

    lerp (res,tmp4, tmp5, t);   

    res.normalize ();
  }
  else if (frame > seg.end)
    res = seg.v2;
  else if (frame <= seg.start)
    res = seg.v1;    

  angle = 2.0f * acos (res.w);

  if (sin (angle / 2.0f) < 0.0f)  
    ort   = normalize (vector3d_t (-res.x,-res.y,-res.z));
  else
    ort   = normalize (vector3d_t (res.x,res.y,res.z));
}

float Ease(float t, float from, float to) 
{
  float k;
  float s = from + to;

  if (s == 0.0) return t;

  if (s > 1.0) 
  {
    from = from / s;
    to = to / s;
  }

  k = 1.0 / (2.0 - from - to);

  if (t < from)    
    return ((k / from) * t * t);
  else 
  {
    if (t < 1.0 - to)
      return (k * (2 * t - from));
    else 
    {
      t = 1.0 - t;
      return (1.0 - (k / to) * t * t);
    }
  }
}
