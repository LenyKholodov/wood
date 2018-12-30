#include <pch.h>
#include <graph\utils.h>
#include <fixed\fixed.h>
#include <debug.h>

#pragma comment (lib,"opengl32.lib")

static int mod3 [6] = {0,1,2,0,1,2};

BOOL    IsInBound       (const vertex_t& v,int num,const plane_t* p)
{
  for (int i=0;i<num;i++)
    if (v.x*p [i].a + v.y*p [i].b + v.z*p [i].c + p [i].d <= 0)
      return FALSE;    

  return TRUE;
}

BOOL    IsInBound       (int count,const vertex_t* v,int num,const plane_t* p)
{
  for (int i=0;i<num;i++)
  {
    BOOL flag = FALSE;

    for (int j=0;j<count && !flag;j++)
      if (v [j].x*p [i].a + v [j].y*p [i].b + v [j].z*p [i].c + p [i].d > 0)
        flag = TRUE;

    if (!flag) return FALSE;    
  }

  return TRUE;
}

BOOL    IsInBound       (int count,int* index,const vertex_t* vertex,int num,const plane_t* p)
{
  for (int i=0;i<num;i++)
  {
    BOOL      flag = FALSE;

    for (int j=0;j<count && !flag;j++)
    {
      const vertex_t& v = vertex [index [j]];

      if (v.x*p [i].a + v.y*p [i].b + v.z*p [i].c + p [i].d > 0)
      {
        flag = TRUE;
        break;
      }  
    }

    if (!flag) return FALSE;    
  }

  return TRUE;
}

BOOL    IsInBound       (const vertex_t& c,float width,int num,const plane_t* p)
{
  float      w = width / 2.0f;           
  vertex_t   v0 (c.x-w,     c.y-w,      c.z+w);
  vertex_t   v1 (c.x+w,     c.y-w,      c.z+w);
  vertex_t   v2 (c.x+w,     c.y-w,      c.z-w);
  vertex_t   v3 (c.x-w,     c.y-w,      c.z-w);
  vertex_t   v4 (c.x-w,     c.y+w,      c.z+w);
  vertex_t   v5 (c.x+w,     c.y+w,      c.z+w);
  vertex_t   v6 (c.x+w,     c.y+w,      c.z-w);
  vertex_t   v7 (c.x-w,     c.y+w,      c.z-w);

  for (int i=0;i<num;i++)
  {
    if (v0.x*p [i].a + v0.y*p [i].b + v0.z*p [i].c + p [i].d > 0) continue;          
    if (v1.x*p [i].a + v1.y*p [i].b + v1.z*p [i].c + p [i].d > 0) continue;          
    if (v2.x*p [i].a + v2.y*p [i].b + v2.z*p [i].c + p [i].d > 0) continue;          
    if (v3.x*p [i].a + v3.y*p [i].b + v3.z*p [i].c + p [i].d > 0) continue;          
    if (v4.x*p [i].a + v4.y*p [i].b + v4.z*p [i].c + p [i].d > 0) continue;          
    if (v5.x*p [i].a + v5.y*p [i].b + v5.z*p [i].c + p [i].d > 0) continue;          
    if (v6.x*p [i].a + v6.y*p [i].b + v6.z*p [i].c + p [i].d > 0) continue;          
    if (v7.x*p [i].a + v7.y*p [i].b + v7.z*p [i].c + p [i].d > 0) continue;

    return FALSE;
  }

  return TRUE;
}

BOOL    IsInBound       (const facet_t& facet,const vertex_t* vertex,int num,const plane_t* p)
{
  const vertex_t& v0 = vertex [facet.index [0]];
  const vertex_t& v1 = vertex [facet.index [1]];
  const vertex_t& v2 = vertex [facet.index [2]];

  for (int i=0;i<num;i++)
  {
    if (v0.x*p [i].a + v0.y*p [i].b + v0.z*p [i].c + p [i].d > 0) continue;      
    if (v1.x*p [i].a + v1.y*p [i].b + v1.z*p [i].c + p [i].d > 0) continue;
    if (v2.x*p [i].a + v2.y*p [i].b + v2.z*p [i].c + p [i].d > 0) continue;
    
    return FALSE;
  }

  return TRUE;  
}

BOOL    IsInBound       (const portal_t& portal,int num,const plane_t* p)
{
  const vertex_t& v0 = portal.v [0];
  const vertex_t& v1 = portal.v [1];
  const vertex_t& v2 = portal.v [2];

  for (int i=0;i<num;i++)
  {
    if (v0.x*p [i].a + v0.y*p [i].b + v0.z*p [i].c + p [i].d > 0) continue;      
    if (v1.x*p [i].a + v1.y*p [i].b + v1.z*p [i].c + p [i].d > 0) continue;
    if (v2.x*p [i].a + v2.y*p [i].b + v2.z*p [i].c + p [i].d > 0) continue;

    return FALSE;
  }

  return TRUE;  
}

BOOL    IsInBound       (const boundbase_t& box,int num,const plane_t* p)
{
  const vertex_t& v0  = box.pos;
  const vertex_t  v1  = box.pos + box.axis [0];
  const vertex_t  v2  = box.pos + box.axis [1];
  const vertex_t  v3  = box.pos + box.axis [2];
  const vertex_t  v4  = box.pos + box.axis [0] + box.axis [1];
  const vertex_t  v5  = box.pos + box.axis [0] + box.axis [2];
  const vertex_t  v6  = box.pos + box.axis [1] + box.axis [2];
  const vertex_t  v7  = box.pos + box.axis [0] + box.axis [1] + box.axis [2];

  for (int i=0;i<num;i++)
  {
    if (v0.x*p [i].a + v0.y*p [i].b + v0.z*p [i].c + p [i].d > 0) continue;          
    if (v1.x*p [i].a + v1.y*p [i].b + v1.z*p [i].c + p [i].d > 0) continue;          
    if (v2.x*p [i].a + v2.y*p [i].b + v2.z*p [i].c + p [i].d > 0) continue;          
    if (v3.x*p [i].a + v3.y*p [i].b + v3.z*p [i].c + p [i].d > 0) continue;          
    if (v4.x*p [i].a + v4.y*p [i].b + v4.z*p [i].c + p [i].d > 0) continue;          
    if (v5.x*p [i].a + v5.y*p [i].b + v5.z*p [i].c + p [i].d > 0) continue;          
    if (v6.x*p [i].a + v6.y*p [i].b + v6.z*p [i].c + p [i].d > 0) continue;          
    if (v7.x*p [i].a + v7.y*p [i].b + v7.z*p [i].c + p [i].d > 0) continue;          

    return FALSE;
  }

  return TRUE;
}

BOOL    IsInBound       (const boundbox_t& box,int num,const plane_t* p)
{
  const vertex_t  d   = box.max - box.min;
  
  const vertex_t& Min = box.min;
  const vertex_t& Max = box.max;

  const vertex_t& v0  = Min;
  const vertex_t& v1  = Max;
  const vertex_t  v2 (Min.x+d.x,        Min.y,          Min.z);
  const vertex_t  v3 (Min.x,            Min.y+d.y,      Min.z);
  const vertex_t  v4 (Min.x,            Min.y,          Min.z+d.z);
  const vertex_t  v5 (Min.x+d.x,        Min.y,          Min.z+d.z);
  const vertex_t  v6 (Min.x,            Min.y+d.y,      Min.z+d.z);
  const vertex_t  v7 (Min.x+d.x,        Min.y+d.y,      Min.z);

  for (int i=0;i<num;i++)
  {
    if (v0.x*p [i].a + v0.y*p [i].b + v0.z*p [i].c + p [i].d > 0) continue;          
    if (v1.x*p [i].a + v1.y*p [i].b + v1.z*p [i].c + p [i].d > 0) continue;          
    if (v2.x*p [i].a + v2.y*p [i].b + v2.z*p [i].c + p [i].d > 0) continue;          
    if (v3.x*p [i].a + v3.y*p [i].b + v3.z*p [i].c + p [i].d > 0) continue;          
    if (v4.x*p [i].a + v4.y*p [i].b + v4.z*p [i].c + p [i].d > 0) continue;          
    if (v5.x*p [i].a + v5.y*p [i].b + v5.z*p [i].c + p [i].d > 0) continue;          
    if (v6.x*p [i].a + v6.y*p [i].b + v6.z*p [i].c + p [i].d > 0) continue;          
    if (v7.x*p [i].a + v7.y*p [i].b + v7.z*p [i].c + p [i].d > 0) continue;          

    return FALSE;
  }

  return TRUE;
}

BOOL    IsInBound       (const boundsphere_t& sphere,int num,const plane_t* p)
{
  const vertex_t&   c  = sphere.center;
  float             r  = sphere.r;

  for (int i=0;i<num;i++)
    if (c.x*p [i].a + c.y*p [i].b + c.z*p [i].c + p[i].d <= -r)
      return FALSE;

  return TRUE;
}

BOOL    IsInBound       (const bound_t& b,int num,const plane_t* p)
{
  const vertex_t  d   = b.box.max - b.box.min;
  
  const vertex_t& Min = b.box.min;
  const vertex_t& Max = b.box.max;

  const vertex_t& v0  = Min;
  const vertex_t& v1  = Max;
  const vertex_t  v2 (Min.x+d.x,        Min.y,          Min.z);
  const vertex_t  v3 (Min.x,            Min.y+d.y,      Min.z);
  const vertex_t  v4 (Min.x,            Min.y,          Min.z+d.z);
  const vertex_t  v5 (Min.x+d.x,        Min.y,          Min.z+d.z);
  const vertex_t  v6 (Min.x,            Min.y+d.y,      Min.z+d.z);
  const vertex_t  v7 (Min.x+d.x,        Min.y+d.y,      Min.z);

  const vertex_t&   c  = b.sphere.center;
  float             r  = b.sphere.r;

  for (int i=0;i<num;i++)
  {
    if (c.x*p [i].a + c.y*p [i].b + c.z*p [i].c + p[i].d <= -r)
      return FALSE;      

    if (v0.x*p [i].a + v0.y*p [i].b + v0.z*p [i].c + p [i].d > 0) continue;          
    if (v1.x*p [i].a + v1.y*p [i].b + v1.z*p [i].c + p [i].d > 0) continue;          
    if (v2.x*p [i].a + v2.y*p [i].b + v2.z*p [i].c + p [i].d > 0) continue;          
    if (v3.x*p [i].a + v3.y*p [i].b + v3.z*p [i].c + p [i].d > 0) continue;          
    if (v4.x*p [i].a + v4.y*p [i].b + v4.z*p [i].c + p [i].d > 0) continue;          
    if (v5.x*p [i].a + v5.y*p [i].b + v5.z*p [i].c + p [i].d > 0) continue;          
    if (v6.x*p [i].a + v6.y*p [i].b + v6.z*p [i].c + p [i].d > 0) continue;          
    if (v7.x*p [i].a + v7.y*p [i].b + v7.z*p [i].c + p [i].d > 0) continue;          

    return FALSE;
  }

  return TRUE;
}

/*BOOL    IsVisible       (const Camera&,const vertex_t&)
{
  return TRUE;
}

BOOL    IsVisible       (const Camera& camera,const vertex_t& c,float width)
{
  boundbox_t box;
  float      w   = width / 2;  
  box.min        = vector3d_t (c.x-w,c.y-w,c.z-w);
  box.max        = vector3d_t (c.x+w,c.y+w,c.z+w);

  return IsVisible (camera,box);
}

BOOL    IsVisible       (const Camera&,const facet_t&,const vertex_t*)
{
  return TRUE;
}

BOOL    IsVisible       (const Camera&,const portal_t&)
{
  return TRUE;
}

BOOL    IsVisible       (const Camera&,const boundbox_t&)
{
  return TRUE;
}

BOOL    IsVisible       (const Camera&,const boundsphere_t&)
{
  return TRUE;
}

BOOL    IsVisible       (const Camera&,const bound_t&)
{
  return TRUE;
}

BOOL    IsVisible       (const Camera&,const vertex_t&,const portalbound_t&)
{
  return TRUE;
}

BOOL    IsVisible       (const Camera& camera,const vertex_t& c,float width,const portalbound_t& p)
{
  boundbox_t box;
  float      w   = width / 2;  
  box.min        = vector3d_t (c.x-w,c.y-w,c.z-w);
  box.max        = vector3d_t (c.x+w,c.y+w,c.z+w);


  return IsVisible (camera,box,p);
}

BOOL    IsVisible       (const Camera&,const facet_t&,const vertex_t*,const portalbound_t&)
{
  return TRUE;
}

BOOL    IsVisible       (const Camera&,const portal_t&,const portalbound_t&)
{
  return TRUE;
}

BOOL    IsVisible       (const Camera&,const boundbox_t&,const portalbound_t&)
{
  return TRUE;
}

BOOL    IsVisible       (const Camera&,const boundsphere_t&,const portalbound_t&)
{
  return TRUE;
}

BOOL    IsVisible       (const Camera&,const bound_t&,const portalbound_t&)
{
  return TRUE;
}


portalbound_t&  CalcPortalBound     (const Camera& eye,const portal_t& p,portalbound_t& pb)
{
  for (int i=0;i<3;i++)
  {
    vector3d_t n     = normalize ((p.v [i] - eye.pos) ^ (p.v [(i + 1) & 3] - eye.pos));
    pb.clip [i].a    = n.x;
    pb.clip [i].b    = n.y;
    pb.clip [i].c    = n.z;
    pb.clip [i].d    = -(n & eye.pos);
//    pb.clip [i].d    = -(n & p.v [0]);
  }

  pb.portal = p;

  return pb;
}

portal_t&       CalcPortalIntercept (const Camera&,const portal_t& pnear,const portal_t& pfar,portal_t& p)
{
  return p = pnear;  
}

BOOL    IsFront         (const Camera& camera,const facet_t& facet,const vertex_t* v)
{
  if ((camera.dir & facet.n) > 0)                          return FALSE;
  if (((v [facet.index [0]] - camera.pos) & facet.n) > 0)  return FALSE;  
  
  return TRUE;
}

BOOL    IsFront         (const Camera& camera,const portal_t& portal)
{
  if ((portal.n & camera.dir) > 0)                   return FALSE;
  if (((portal.v [0] - camera.pos) & portal.n) > 0)  return FALSE;

  return TRUE;
}

BOOL    IsFront         (const Camera& camera,const vertex_t& c,float width)
{
  boundbox_t box;
  float      w   = width / 2;  
  box.min        = vector3d_t (c.x-w,c.y-w,c.z-w);
  box.max        = vector3d_t (c.x+w,c.y+w,c.z+w);

  return IsFront (camera,box);
}

BOOL    IsFront         (const Camera& camera,const boundbox_t& box)
{
   return (((box.min - camera.pos) & camera.dir) > 0) ||
          (((box.max - camera.pos) & camera.dir) > 0);
}

BOOL    IsFront         (const Camera& c,const bound_t& b)
{
  return IsFront (c,b.box);
}
*/

void            SetMaterial         (const material_t& mat)
{
  glMaterialfv  (GL_FRONT_AND_BACK,GL_AMBIENT,(GLfloat*)&mat.ambient);
  glMaterialfv  (GL_FRONT_AND_BACK,GL_DIFFUSE,(GLfloat*)&mat.diffuse);
  glMaterialfv  (GL_FRONT_AND_BACK,GL_SPECULAR,(GLfloat*)&mat.specular);
  glMaterialfv  (GL_FRONT_AND_BACK,GL_EMISSION,(GLfloat*)&mat.emission);
  glMaterialf   (GL_FRONT_AND_BACK,GL_SHININESS,mat.shiness);
}

void            SetLight            (int i,const light_t& light)
{
  GLfloat       diffuse  [4] = {light.diffuse.x,light.diffuse.y,light.diffuse.z,1.0};
  GLfloat       ambient  [4] = {light.ambient.x,light.ambient.y,light.ambient.z,1.0};
  GLfloat       specular [4] = {light.specular.x,light.specular.y,light.specular.z,1.0};
  GLfloat       pos      [4] = {light.pos.x,light.pos.y,light.pos.z,float(!light.farlight)};
  GLfloat       dir      [4] = {light.dir.x,light.dir.y,light.dir.z,0};

  glLightfv     (GL_LIGHT0+i,GL_AMBIENT,ambient);
  glLightfv     (GL_LIGHT0+i,GL_DIFFUSE,diffuse);
  glLightfv     (GL_LIGHT0+i,GL_SPECULAR,specular);
  glLightfv     (GL_LIGHT0+i,GL_POSITION,pos);
  glLightfv     (GL_LIGHT0+i,GL_SPOT_DIRECTION,dir);  
  glLightf      (GL_LIGHT0+i,GL_SPOT_CUTOFF,light.angle*57.3f);
  glLightf      (GL_LIGHT0+i,GL_SPOT_EXPONENT,light.shiness);
}

void            SetLight            (int i,BOOL state)
{
  if (state) glEnable  (GL_LIGHT0+i);
  else       glDisable (GL_LIGHT0+i);
}

light_t         GetLight            (int i)
{
  GLfloat       diffuse  [4];
  GLfloat       ambient  [4]; 
  GLfloat       specular [4];
  GLfloat       pos      [4];
  GLfloat       dir      [4];

  light_t       light;

  glGetLightfv     (GL_LIGHT0+i,GL_AMBIENT,ambient);
  glGetLightfv     (GL_LIGHT0+i,GL_DIFFUSE,diffuse);
  glGetLightfv     (GL_LIGHT0+i,GL_SPECULAR,specular);
  glGetLightfv     (GL_LIGHT0+i,GL_POSITION,pos);
  glGetLightfv     (GL_LIGHT0+i,GL_SPOT_DIRECTION,dir);  
  glGetLightfv     (GL_LIGHT0+i,GL_SPOT_CUTOFF,&light.angle);
  glGetLightfv     (GL_LIGHT0+i,GL_SPOT_EXPONENT,&light.shiness);

  memcpy (&light.diffuse,diffuse,sizeof (vector3d_t));
  memcpy (&light.ambient,ambient,sizeof (vector3d_t));
  memcpy (&light.specular,specular,sizeof (vector3d_t));
  memcpy (&light.pos,pos,sizeof (vector3d_t));
  memcpy (&light.dir,dir,sizeof (vector3d_t));

  light.angle /= 57.3;

  return light;
}

void         normalize      (plane_t& p)
{
  float    magn = sqrt (p.a*p.a + p.b*p.b + p.c*p.c);

  p.a          /= magn;
  p.b          /= magn;
  p.c          /= magn;
  p.d          /= magn;
}

void         normalize      (line_t& l)
{
  float    magn = sqrt (l.a*l.a + l.b*l.b);
  float    div  = magn < FL_EPS ? 1.0f : 1.0f/magn;

  l.a          *= div;
  l.b          *= div;
  l.c          *= div;
}

frustum_t&      CalcGLFrustum       (frustum_t& f)
{
  float   proj [16]; 
  float   modl[16];
  float   clip[16];

  glGetFloatv (GL_PROJECTION_MATRIX,  proj);
  glGetFloatv (GL_MODELVIEW_MATRIX,   modl);

  clip [0]  = modl [0]*proj [0] + modl [1]*proj [4] + modl [2]*proj [8]  + modl [3]*proj [12];
  clip [1]  = modl [0]*proj [1] + modl [1]*proj [5] + modl [2]*proj [9]  + modl [3]*proj [13];
  clip [2]  = modl [0]*proj [2] + modl [1]*proj [6] + modl [2]*proj [10] + modl [3]*proj [14];
  clip [3]  = modl [0]*proj [3] + modl [1]*proj [7] + modl [2]*proj [11] + modl [3]*proj [15];
                                                                                       
  clip [4]  = modl [ 4] * proj [0] + modl [5] * proj [4] + modl [6] * proj [ 8] + modl [7] * proj [12];
  clip [5]  = modl [ 4] * proj [1] + modl [5] * proj [5] + modl [6] * proj [ 9] + modl [7] * proj [13];
  clip [6]  = modl [ 4] * proj [2] + modl [5] * proj [6] + modl [6] * proj [10] + modl [7] * proj [14];
  clip [7]  = modl [ 4] * proj [3] + modl [5] * proj [7] + modl [6] * proj [11] + modl [7] * proj [15];

  clip [8]  = modl [ 8] * proj [0] + modl [9] * proj [4] + modl [10] * proj [ 8] + modl [11] * proj [12];
  clip [9]  = modl [ 8] * proj [1] + modl [9] * proj [5] + modl [10] * proj [ 9] + modl [11] * proj [13];
  clip [10] = modl [ 8] * proj [2] + modl [9] * proj [6] + modl [10] * proj [10] + modl [11] * proj [14];
  clip [11] = modl [ 8] * proj [3] + modl [9] * proj [7] + modl [10] * proj [11] + modl [11] * proj [15];

  clip [12] = modl [12] * proj [0] + modl [13] * proj [4] + modl [14] * proj [ 8] + modl [15] * proj [12];
  clip [13] = modl [12] * proj [1] + modl [13] * proj [5] + modl [14] * proj [ 9] + modl [15] * proj [13];
  clip [14] = modl [12] * proj [2] + modl [13] * proj [6] + modl [14] * proj [10] + modl [15] * proj [14];
  clip [15] = modl [12] * proj [3] + modl [13] * proj [7] + modl [14] * proj [11] + modl [15] * proj [15];
        
  f.clip [0].a = clip [3]  - clip [0];
  f.clip [0].b = clip [7]  - clip [4];
  f.clip [0].c = clip [11] - clip [8];
  f.clip [0].d = clip [15] - clip [12];  

  f.clip [1].a = clip [3]  + clip [0];
  f.clip [1].b = clip [7]  + clip [4];
  f.clip [1].c = clip [11] + clip [8];
  f.clip [1].d = clip [15] + clip [12];

  f.clip [2].a = clip [3]  + clip [1];
  f.clip [2].b = clip [7]  + clip [5];
  f.clip [2].c = clip [11] + clip [9];
  f.clip [2].d = clip [15] + clip [13];

  f.clip [3].a = clip [3]  - clip [1];
  f.clip [3].b = clip [7]  - clip [5];
  f.clip [3].c = clip [11] - clip [9];
  f.clip [3].d = clip [15] - clip [13];

  f.clip [4].a = clip [3]  - clip [2];
  f.clip [4].b = clip [7]  - clip [6];
  f.clip [4].c = clip [11] - clip [10];
  f.clip [4].d = clip [15] - clip [14];

  f.clip [5].a = clip [3]  + clip [2];
  f.clip [5].b = clip [7]  + clip [6];
  f.clip [5].c = clip [11] + clip [10];
  f.clip [5].d = clip [15] + clip [14];

  normalize (f.clip [0]);
  normalize (f.clip [1]);
  normalize (f.clip [2]);
  normalize (f.clip [3]);
  normalize (f.clip [4]);
  normalize (f.clip [5]);

  return f;
}

frustum_t&      CalcFrustum         (const matrix_t& m,frustum_t& f)
{        
  static int transm [16] = {0, 4, 8,12,
                            1, 5, 9,13,
                            2, 6,10,14,
                            3, 7,11,15};
  float* clip            = (float*)&m;

  f.clip [0].a = clip [transm [3]]  - clip [transm [0]];
  f.clip [0].b = clip [transm [7]]  - clip [transm [4]];
  f.clip [0].c = clip [transm [11]] - clip [transm [8]];
  f.clip [0].d = clip [transm [15]] - clip [transm [12]];  

  f.clip [1].a = clip [transm [3]]  + clip [transm [0]];
  f.clip [1].b = clip [transm [7]]  + clip [transm [4]];
  f.clip [1].c = clip [transm [11]] + clip [transm [8]];
  f.clip [1].d = clip [transm [15]] + clip [transm [12]];

  f.clip [2].a = clip [transm [3]]  + clip [transm [1]];
  f.clip [2].b = clip [transm [7]]  + clip [transm [5]];
  f.clip [2].c = clip [transm [11]] + clip [transm [9]];
  f.clip [2].d = clip [transm [15]] + clip [transm [13]];

  f.clip [3].a = clip [transm [3]]  - clip [transm [1]];
  f.clip [3].b = clip [transm [7]]  - clip [transm [5]];
  f.clip [3].c = clip [transm [11]] - clip [transm [9]];
  f.clip [3].d = clip [transm [15]] - clip [transm [13]];

  f.clip [4].a = clip [transm [3]]  - clip [transm [2]];
  f.clip [4].b = clip [transm [7]]  - clip [transm [6]];
  f.clip [4].c = clip [transm [11]] - clip [transm [10]];
  f.clip [4].d = clip [transm [15]] - clip [transm [14]];

  f.clip [5].a = clip [transm [3]]  + clip [transm [2]];
  f.clip [5].b = clip [transm [7]]  + clip [transm [6]];
  f.clip [5].c = clip [transm [11]] + clip [transm [10]];
  f.clip [5].d = clip [transm [15]] + clip [transm [14]];

  normalize (f.clip [0]);
  normalize (f.clip [1]);
  normalize (f.clip [2]);
  normalize (f.clip [3]);
  normalize (f.clip [4]);
  normalize (f.clip [5]);

  return f;
}

BOOL            IsPlaneIntersect   (vertex_t* line,const plane_t& plane)
{
  vector3d_t n (plane.a,plane.b,plane.c);
  float d1 = (n & line [0]) + plane.d,
        d2 = (n & line [1]) + plane.d;

  return d1 * d2 < 0.0f;
}

BOOL            IsPlaneIntersect   (vertex_t* line,vertex_t* poly)
{
  vector3d_t n  = (poly [0] - poly [2]) ^ (poly [2] - poly [1]);
  float      d  = -(n & poly [0]);
  float      d1 = (n & line [0]) + d,
             d2 = (n & line [1]) + d;

  return d1 * d2 < FL_EPS;
}

BOOL            IsPolygonIntersect (vertex_t* line,vertex_t* v,int count)
{
  plane_t    p;
  vector3d_t x;
  vector3d_t n  = (v [0] - v [2]) ^ (v [2] - v [1]);
  p.a           = n.x;
  p.b           = n.y;
  p.c           = n.z;
  p.d           = -(n & v [0]);    

  if (!IntersectionPoint (line,p,x)) 
    return FALSE;

  return IsInsidePolygon (x,v,count);
}

BOOL            IntersectionPoint  (vertex_t* line,const plane_t& plane,vertex_t& x)
{
  vector3d_t n (plane.a,plane.b,plane.c);

  float      d1 = (n & line [0]) + plane.d,
             d2 = (n & line [1]) + plane.d;

  vector3d_t dir         = normalize (line [1] - line [0]);
  float      numerator   = -d1,
             denominator = n & dir, 
             dist        = 0.0f;
                                  
  if (fabs (denominator) < FL_EPS) 
  {
    x = line [0];
    return fabs (d1) < FL_EPS;
  }  

  dist = numerator / denominator;
  x    = line [0] + dir * dist;
      
  return  d1 * d2 < 0.0f;
}

BOOL            IsInsidePolygon    (const vertex_t& x,vertex_t* v,int count)
{
  const float MATCH_FACTOR = 0.9999f;
  float angle              = 0.0f;

  vector3d_t va (.0f), vb (.0f);

  for (int i = 0;i < count - 1;i++)
  {       
    va     = v [i] - x;
    vb     = v [i + 1] - x;
                                                                                                
    angle += ::angle (va,vb);
  }

  va     = v [i] - x;
  vb     = v [0] - x;
                                                                                                
  angle += ::angle (va,vb);

  if (angle >= (MATCH_FACTOR * (2.0f * FL_PI))) return TRUE; 
                
  return FALSE;
}

/*int      SplitFacet    
 ( const vertex_t* v,
   const plane_t&  plane,
   vertex_t*       tri1,
   vertex_t*       tri2,
   vertex_t*       tri3
 )
{        
  int      mid    = 0;
  float    d  [3] = {0.0f};  
  vertex_t v1 [5], 
           v2 [5];

  for (int i=0;i<3;i++)
  {
    d [i] = FindDist (v [i],plane);

    if (fabs (d [i]) <= FL_EPS) mid++;
  }  

  if (mid >= 2)
  {
    tri1 [0] = v [0];
    tri1 [1] = v [1];
    tri1 [2] = v [2];

    return 1;
  } 

  vector3d_t prevP  = v [2];
  float      prevF  = d [2];
  int        count1 = 0,
             count2 = 0;

  //2- NEGATIVE, 1-POSITIVE

  for (i=0;i<3;i++)
  {
    vector3d_t curP = v [i];    
    float      curF = d [i];

    if (fabs (curF) < FL_EPS)
    {
      v2 [count2++] = curP;
      v1 [count1++] = curP;
    }
    else if (curF * prevF > 0)
    {
      if   (curF < FL_EPS) v2 [count2++] = curP;
      else                 v1 [count1++] = curP;
    }
    else
    {      
      if (curF < FL_EPS) //current in negative
      {
        float     t   = -curF / (prevF - curF);
        vertex_t  sp  = curP + (prevP - curP) * t;

        if (mid)
        {
          v2 [count2++] = curP;
          v2 [count2++] = sp;
        }
        else
        {
          v1 [count1++] = sp;

          v2 [count2++] = curP;
          v2 [count2++] = sp;
        }  
      }
      else  //current in positive
      {
        float     t   = -curF / (prevF - curF);
        vertex_t  sp  = curP + (prevP - curP) * t;

        if (mid)
        {
          v1 [count1++] = curP;
          v1 [count1++] = sp;
        }
        else
        {
          v2 [count2++] = sp;

          v1 [count1++] = curP;
          v1 [count1++] = sp;
        }
      }
    }

    prevP = curP;
    prevF = curF;            
  }

  if (count1 == 3 && count2 == 3)
  {  
    tri1 [0] = v1 [0];
    tri1 [1] = v1 [1];
    tri1 [2] = v1 [2];    

    tri2 [0] = v2 [0];
    tri2 [1] = v2 [1];
    tri2 [2] = v2 [2];    

    return 2;
  }
  else if (count1 == 4 || count2 == 4)
  {
    if (count1 == 3)
    {
      tri1 [0] = v1 [0];
      tri1 [1] = v1 [1];
      tri1 [2] = v1 [2];    

      tri2 [0] = v2 [0];
      tri2 [1] = v2 [1];
      tri2 [2] = v2 [2];          

      tri3 [0] = v2 [0];
      tri3 [1] = v2 [2];
      tri3 [2] = v2 [3];          
    }
    else if (count2 == 3)
    {
      tri1 [0] = v2 [0];
      tri1 [1] = v2 [1];
      tri1 [2] = v2 [2];    

      tri2 [0] = v1 [0];
      tri2 [1] = v1 [1];
      tri2 [2] = v1 [2];          

      tri3 [0] = v1 [0];
      tri3 [1] = v1 [2];
      tri3 [2] = v1 [3];          
    }

    return 3;     
  }

  tri1 [0] = v [0];
  tri1 [1] = v [1];
  tri1 [2] = v [2];    

  return 1;
}

int  SplitFacet         
 ( const MemListTmpl<vertex_t>& src,
   const plane_t&               plane,
   MemListTmpl<vertex_t>&       left,
   MemListTmpl<vertex_t>&       right
 )
{
  int                             mid   = 0;
  MemListTmpl<vertex_t>::iterator iter  = src;
  MemListTmpl<vertex_t>::iterator prev  = src;--prev;
  int                             count = src.count ();
  float                           state = 1.0f;

  for (int i=0;i<count;i++,iter++)
  {
    float d = FindDist (iter.data (),plane);

    if (fabs (d) <= FL_EPS) mid++;
    else                    state = d;
  }  

  if (mid >= 2)
  {
    if (state < FL_EPS)
    {
      for (i=0;i<count;i++,iter++) 
        left.insert (iter.data ());
    }
    else
    {
      for (i=0;i<count;i++,iter++) 
        right.insert (iter.data ());
    }  

    return 1;
  } 

  vector3d_t prevP  = prev.data();
  float      prevF  = FindDist (prevP,plane);  

  iter.reset ();

  for (i=0;i<count;i++,iter++)
  {
    vector3d_t curP = iter;
    float      curF = FindDist (curP,plane);

    if (fabs (curF) < FL_EPS)
    {
      left.insert  (curP);
      right.insert (curP);
    }
    else if (curF * prevF > 0)
    {
      if   (curF < FL_EPS) left.insert  (curP);
      else                 right.insert (curP);
    }
    else
    {      
      if (curF < FL_EPS) //current in negative
      {
        float     t   = -curF / (prevF - curF);
        vertex_t  sp  = curP + (prevP - curP) * t;

        if (mid)
        {
          left.insert (sp);
          left.insert (curP);
        }
        else
        {
          right.insert (sp);
          left.insert  (sp);
          left.insert  (curP);
        }  
      }
      else  //current in positive
      {
        float     t   = -curF / (prevF - curF);
        vertex_t  sp  = curP + (prevP - curP) * t;

        if (mid)
        {
          right.insert (curP);
          right.insert (sp);
        }
        else
        {
          left.insert  (sp);
          right.insert (sp);
          right.insert (curP);
        }
      }
    }

    prevP = curP;
    prevF = curF;            
  }

  return right.count () && left.count () ? 2 : 1;
}

int             SplitFacet         
 ( const vertex_t* v,
   int             count,
   const plane_t*  plane,
   vertex_t*       tri 
 )
{
  const int MAX_VERTEX = 32;
  vertex_t temp [3][MAX_VERTEX];

  MemListTmpl<vertex_t>  from   (3,(vertex_t*)v,3);
  MemListTmpl<vertex_t>  left   (MAX_VERTEX,temp [0]),
                         right  (MAX_VERTEX,temp [1]),
                         right1 (MAX_VERTEX,temp [2]);
  MemListTmpl<vertex_t>* src   = &from,
                         *dest = &right;
  int                    res   = 0;

  for (int i=0;i<count;i++,res = (res+1) & 1)
  {
    dest->reset ();
    left.reset  ();

    SplitFacet (*src,plane [i],left,*dest);

    if (!dest->count () || dest->count () >= MAX_VERTEX)
      break;

    src  = dest;  
    dest = dest == &right ? &right1 : &right;
  }

  if (dest->count () && dest->count () < MAX_VERTEX)
  {
    int   triCount = dest->count () - 2;
    int   vCount   = dest->count ();
    int   pos      = 0;

    for (int i=0;i<triCount;i++)
    {
      tri [pos++] = (*dest) [0];
      tri [pos++] = (*dest) [(i+1) % vCount];
      tri [pos++] = (*dest) [(i+2) % vCount];
    }  

    return triCount;
  }

  if (dest->count () >= MAX_VERTEX)
  {
    #ifdef DEBUG
      dout<<"SplitFacet: Error MAX_VERTEX: "<<dest->count ()<<endl;
    #endif
  }

  tri [0] = v [0];
  tri [1] = v [1];
  tri [2] = v [2];  
  
  return 1;
} */

BOOL    CalcBound        (int vCount,vertex_t* vertex,bound_t& b)
{
  D_Try
  {
    if (!vCount || !vertex)     
      return FALSE;

    vector3d_t c (.0f);
    vertex_t*  v = vertex;

    for (int i=0;i<vCount;i++,v++)
      c += *v;

    c /= float (vCount);

    vector3d_t d (.0f);
    v          = vertex;    

    for (i=0;i<vCount;v++,i++)
    {   
      vector3d_t dim  = abs (c - *v);

      if (dim.x > d.x)  d.x = dim.x;
      if (dim.y > d.y)  d.y = dim.y;
      if (dim.z > d.z)  d.z = dim.z;
    }

    b.sphere.center = c;
    b.sphere.r      = d.length ();
    b.box.min       = c - d;
    b.box.max       = c + d;    

    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Error at CalcBoundBox "<<endl;
    #endif
    return FALSE;
  }
}

BOOL   CalcBound  (int fCount,facet_t* facet,vertex_t* vertex,bound_t& b)
{
  D_Try
  {
    if (!vertex || !facet || !fCount)
      return FALSE;

    vector3d_t c (.0f);
    facet_t*   f = facet;

    for (int i=0;i<fCount;i++,f++)    
      for (int j=0;j<3;j++)
        c += vertex [f->index [j]];

    c /= float (fCount*3);

    vector3d_t d (.0f);
    f = facet;

    for (i=0;i<fCount;f++,i++)
      for (int j=0;j<3;j++)
      {   
        vector3d_t dim  = abs (c - vertex [f->index [j]]);

        if (dim.x > d.x)  d.x = dim.x;
        if (dim.y > d.y)  d.y = dim.y;
        if (dim.z > d.z)  d.z = dim.z;
      }

    b.sphere.center = c;
    b.sphere.r      = d.length ();
    b.box.min       = c - d;
    b.box.max       = c + d;    

    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Error at CalcBoundBox "<<endl;
    #endif
    return FALSE;
  }
}

void   CalcBound           (const boundbase_t& box,bound_t& b)
{
  vertex_t  vertex [8] = {
    box.pos,
    box.pos + box.axis [0],
    box.pos + box.axis [1],
    box.pos + box.axis [2],
    box.pos + box.axis [0] + box.axis [1],
    box.pos + box.axis [0] + box.axis [2],
    box.pos + box.axis [1] + box.axis [2],
    box.pos + box.axis [0] + box.axis [1] + box.axis [2]
  };  

  vector3d_t c (.0f);
  vertex_t*  v = vertex;

  for (int i=0;i<8;i++,v++)
    c += *v;

  c /= 8.0f;

  vector3d_t d (.0f);
  v          = vertex;    

  for (i=0;i<8;v++,i++)
  {   
    vector3d_t dim  = abs (c - *v);

    if (dim.x > d.x)  d.x = dim.x;
    if (dim.y > d.y)  d.y = dim.y;
    if (dim.z > d.z)  d.z = dim.z;
  }

  b.sphere.center = c;
  b.sphere.r      = d.length ();
  b.box.min       = c - d;
  b.box.max       = c + d;      
}

void            CalcBoundBase       (const matrix_t& m,boundbase_t& b)
{
  b.axis [0]    = vector3d_t (m [0][0],m [0][1],m [0][2]);
  b.axis [1]    = vector3d_t (m [1][0],m [1][1],m [1][2]);  
  b.axis [2]    = vector3d_t (m [2][0],m [2][1],m [2][2]);
  b.pos         = vector3d_t (m [3][0],m [3][1],m [3][2]);
}

textmat_t::textmat_t (int) { text [0] = 0; strcpy (name,"____null"); alp [0] = 0; }