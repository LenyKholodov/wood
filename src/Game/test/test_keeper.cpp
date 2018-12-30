/////////////////////implementation of class TestKeeper/////////////////////////

#include <gl\gl.h>

struct testkprprop_t
{
  boundbox_t  box;
};

void       TestKeeper::SetPosition      (model_t* mdl,const vector3d_t& v)
{
  vector3d_t oldPos = mdl->pos.pos;

  ModelKeeper::SetPosition (mdl,v);

  mdl->pos.box       = ((testkprprop_t*)mdl->data.data)->box;
  mdl->pos.box.min  += v - oldPos;
  mdl->pos.box.max  += v - oldPos;

  mModelEnv->InsertModel (mdl);
}

BOOL           TestKeeper::InitModel       (model_t* mdl,MODELDATA,va_list list) 
{
  boundbox_t box      = va_arg (list,boundbox_t);

  mdl->data.size      = sizeof (testkprprop_t);
  mdl->data.data      = GetModelPool ()->alloc (mdl,mdl->data.size);

  if (!mdl->data.data)
    return FALSE;    

  testkprprop_t* prop = (testkprprop_t*)mdl->data.data;

  prop->box           = box;
  mdl->pos.pos        = (box.max+box.min)/2.0f;
  mdl->pos.box        = box;

  mModelEnv->InsertModel (mdl);

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


void           TestKeeper::render           (const modelnode_t* node)
{  
  for (;node;node=node->next)
    drawBox (node->model->pos.box);
}

TestKeeper::TestKeeper  (Pool* _Pool,ModelEnv* _Env)
           : ModelKeeper (_Pool,_Env)
{  }

TestKeeper::~TestKeeper ()
{  }
  

////////////////////////////////////////////////////////////////////////////////
///Test keeper
////////////////////////////////////////////////////////////////////////////////
class   TestKeeper: public ModelKeeper
{
  public:
                        TestKeeper  (Pool*,ModelEnv*);
        virtual         ~TestKeeper ();

  protected:
    virtual     size_t     GetModelDataSize (va_list src,va_list dest) { return 0; }
    virtual     void       SetPosition      (model_t*,const vector3d_t&);
    virtual     BOOL       InitModel        (model_t*,MODELDATA,va_list);

    virtual     void       render           (const modelnode_t*);
};
