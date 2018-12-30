#define  DEBUG
#include <windows.h>
#include <kernel.h>
#include <graph\x3f.h>
#include <graph\textenv.h>
#include <graph\utils.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>

const   float   VIS_VECTOR_Y_SCALE  = 0.9;
const   float   VIS_VECTOR_X_SCALE  = 0.2;
const   float   VIS_VECTOR_LEN      = 20;

SystemPool        sys;
Debug             debug (&sys);
Object3D*         obj;

//LightEnvironment  env (&sys,160);
TextEnvironment   tenv (&sys,16,16);
material_t        mat;
int               frame = 0;

void CALLBACK   Reshape (int,int);
void CALLBACK   Draw    ();
void CALLBACK   Idle    ();
void CALLBACK   Key     ();
void            Draw    (Object3D::Node,matrix_t,animseg_t*);

void            Recalc  (vector3d_t&,vecseg_t&);
void            Recalc  (vector3d_t&,float&,quatseg_t&,matrix_t&);
float           Ease    (float t, float from, float to);
void            DrawVector      (vertex_t& coord,vertex_t vect);

void     main ()
{
//  static Object3D        object   (&sys,"char.3ds");
//  static Object3D        object   (&sys,"door.3ds");
//  static Object3D        object   (&sys,"ruka.3ds");
//  static Object3D        object   (&sys,"tank.3ds");
  static Object3D        object   (&sys,"scene1.3ds");
//  static Object3D        object   (&sys,"scale.3ds");
  obj = &object;

  K_Init ();
  D_Init ("runlog");

  dout<<"Min: "<<obj->main ().bound ().box.min<<" Max: "<<obj->main ().bound ().box.max<<endl;
  dout<<"Facets: "<<obj->count (Object3D::FACET)<<endl;

  mat.diffuse   = vector3d_t (0.3,0.4,0.8);
  mat.ambient   = vector3d_t (0.3,0.1,0.2);
  mat.specular  = vector3d_t (0,0,0);
  mat.emission  = vector3d_t (0.1,0.1,0.1);
  mat.shiness   = 0;

  auxInitPosition       (0,0,800,600);
  auxInitDisplayMode    (AUX_RGB | AUX_DOUBLE | AUX_DEPTH);
  auxInitWindow         ("3DS Loader Test");

  auxKeyFunc            (AUX_SPACE,Key);
  auxReshapeFunc        (Reshape);
  auxIdleFunc           (Idle);
  auxMainLoop           (Draw);  

  K_Done		(1,D_Done);
}

void CALLBACK   Reshape (int width,int height)
{                            
//  const vector3d_t eye (0,0,-50);
//  const vector3d_t dir (0,0,-1);

    //char.3ds
//  const vector3d_t eye (20,0,4);
//  const vector3d_t dir (-1,0,0);

//  const vector3d_t eye (0,0,20);
//  const vector3d_t dir (0,0,-1);
    //char.3ds

  const vector3d_t eye (0,0,-200);
  const vector3d_t dir (0,0,1);

  glViewport       (0,0,width,height);
  glMatrixMode     (GL_PROJECTION);
  glLoadIdentity   ();
//  glOrtho          (-5,5,50,70,1.0,4000.0);
  glOrtho          (-80,80,-30,100,1.0,4000.0);
//  glOrtho          (-350,350,-500,500,1.0,5000.0);

//  glOrtho          (-4500,4500,-1000,1000,-4.0,1400.0);

  glMatrixMode     (GL_MODELVIEW);
  glLoadIdentity   ();
  gluLookAt        (eye.x,eye.y,eye.z,
                    dir.x,dir.y,dir.z,
                    0,1,0);    

  glClearColor     (0.5,0.5,0.5,1.0);
  glClear          (GL_COLOR_BUFFER_BIT);

  glShadeModel     (GL_SMOOTH);
  glPolygonMode    (GL_FRONT_AND_BACK,GL_LINE);

  glEnable         (GL_DEPTH_TEST); 
  glEnable         (GL_COLOR_MATERIAL);  
  glEnable         (GL_LIGHTING);
  glEnable         (GL_LIGHT0);
//  glEnable         (GL_TEXTURE_2D);

  GLfloat lpos [4] = {eye.x,eye.y,eye.z,1.0f};
  GLfloat ldir [4] = {dir.x,dir.y,dir.z,1.0f};  

  glLightfv        (GL_LIGHT0,GL_POSITION,lpos);
  glLightfv        (GL_LIGHT0,GL_SPOT_DIRECTION,ldir);
  glLightf         (GL_LIGHT0,GL_SPOT_CUTOFF,90.0f);  

  glColorMaterial  (GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);

  tenv.SetTextureName (0,"floor2.bmp");
}

void CALLBACK   Draw    ()
{
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  tenv.bind (0);

  glColor3f (1,1,1);

  Object3D::Node node  = obj->main ();

  dout<<"Draw"<<endl;

  Draw (node,matrix_t (1),NULL);

  dout<<"End"<<endl<<endl;

  glFinish ();

  auxSwapBuffers ();
}

void            Draw    (Object3D::Node node,matrix_t parent,animseg_t* panim)
{
  animseg_t* anim = (animseg_t*)node.anim ();
  matrix_t   m    = parent;  
  matrix_t   sam  = parent;

  if (anim)
  {
    vecseg_t   *trTrans = (vecseg_t*)node.trans (),  
               *trScale = (vecseg_t*)node.scale ();
    quatseg_t* trRot    = (quatseg_t*)node.rotate ();
    vector3d_t scale, trans;
    vector3d_t rot;
    float      angle;
    matrix_t   rm (1);
    matrix_t   start = anim->start;

    for (int i=0;i<anim->trans.count && trTrans->end < frame;i++,trTrans=i<anim->trans.count?trTrans+1:trTrans);
    for (i=0    ;i<anim->scale.count && trScale->end < frame;i++,trScale=i<anim->scale.count?trScale+1:trScale);
    for (i=0    ;i<anim->rot.count && trRot->end < frame;i++,trRot=i<anim->rot.count?trRot+1:trRot);

    Recalc (rot,angle,*trRot,rm);
    Recalc (trans,*trTrans);
    Recalc (scale,*trScale);   

    if (node.index () == 10) dout<<"Frame: "<<frame<<" "<<scale<<endl;
//    if (node.index () == 10 || node.index () == 17)  scale *= 200.0f;

    matrix_t x (1);

    x   *= matrix_t::translate (trans);

    x   *= matrix_t::rotate    (rot,angle);
    x   *= matrix_t::translate (-anim->pivot);

    x   *= matrix_t::scale     (scale); ///Странные вещи при scale = (-1,-1,-1)

    m   *= x;

    m   *= start;

    sam  = parent *
           matrix_t::translate (trans) *
           matrix_t::rotate    (rot,angle) *
           matrix_t::scale     (scale);
  }

  if (node.index () != 10 && node.index () != 17)  glColor3f (0,0,0);
  else                                             glColor3f (0,0,1);

//  if (node.index () >= 7 && node.index () <= 10)
//  if (node.index () == 15) //10, 17 - flame
  if (node.vertex() && node.facet())
  {
    vertex_t*  vertex = node.vertex ();
    facet_t*   facet  = node.facet  ();

    const material_t* mat  = &node.owner()->GetMaterial(facet [0].texture).m;    

//    if (mat) glColor3fv ((GLfloat*)&mat->diffuse);

//    if (node.index () == 10) dout<<m * vertex [facet->index [0]]<<endl;;
//    if (node.index () == 10) dout<<node.bound ().box.min<<" "<<node.bound ().box.max<<endl;;
 
    glBegin (GL_TRIANGLES);
      for (int i=0;i<node.count (Object3D::FACET);i++,facet++)
      {
        glNormal3fv ((GLfloat*)&facet->n);
        for (int j=0;j<3;j++)
        {
//          vertex_t v = vertex [facet->index [j]];
          vertex_t v = m * vertex [facet->index [j]];
//          if (frame == 23)dout<<v<<endl;
//          vertex_t v = matrix_t::scale (vector3d_t (20,20,20)) * matrix_t::translate (anim->offs) * vertex [facet->index [j]];
          glVertex3fv    ((GLfloat*)&v);
          glTexCoord2fv ((GLfloat*)&facet->tindex [j]);
        }  
      }
    glEnd   ();
  }

  for (Object3D::Node iter = node.node();iter.valid();iter=iter.next ())
  {
    dout<<node.index ()<<" "<<iter.index ()<<endl;
    Draw (iter,sam,anim);
  }  
}

void            Recalc  (vector3d_t& res,vecseg_t& seg)
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

void            Recalc  (vector3d_t& vec,float& angle,quatseg_t& seg,matrix_t& m)
{
/*  quat_t res;

  if (frame > seg.start && frame <= seg.end)
  {
    quat_t tmp1, tmp2, tmp3, tmp4, tmp5;
    float t = Ease ((frame - seg.start) * seg.locTime,seg.easeFrom,seg.easeTo);

    lerp (tmp1,seg.v1, seg.c1, t),
    lerp (tmp2,seg.c1, seg.c2, t),
    lerp (tmp3,seg.c2, seg.v2, t),
    lerp (tmp4,tmp1, tmp2, t),
    lerp (tmp5,tmp2, tmp3, t),

    lerp (res,tmp4, tmp5, t);   
  }
  else if (frame > seg.end)
    res = seg.q [1];
  else if (frame <= seg.start)
    res = seg.q [0];

  angle = 2.0f * acos (res.w);

  if (sin (angle / 2.0f) < 0.0f)  
    vec   = normalize (vector3d_t (-res.x,-res.y,-res.z));
  else
    vec   = normalize (vector3d_t (res.x,res.y,res.z));

  m = res.toMatrix ();*/

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
    vec   = normalize (vector3d_t (-res.x,-res.y,-res.z));
  else
    vec   = normalize (vector3d_t (res.x,res.y,res.z));

  m = res.toMatrix ();
}

void CALLBACK   Idle    ()
{
  static clock_t last = clock ();

  if (clock () - last > CLK_TCK>>4)
  {
    Draw ();
    last = clock ();
  }
}

void CALLBACK Key ()
{
  if (++frame > 100) frame = 0;
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
