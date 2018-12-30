#include "material.h"

bool Material::apply ()
{
  uint glSide;  

  glFrontFace (front_face);

  if (render_front_face && render_back_face)
  {
    glSide = GL_FRONT_AND_BACK;
    glDisable (GL_CULL_FACE);
  }
  else if (render_front_face)
  {
    glSide = GL_FRONT_FACE;
    glCullFace  (GL_BACK_FACE);
    glEnable    (GL_CULL_FACE);        
  }
  else if (render_back_face)
  {
    glSide = GL_BACK_FACE;
    glCullFace  (GL_FRONT_FACE);
    glEnable    (GL_CULL_FACE);        
  }
  else return false;

  glShadeModel (shade_model);

  if (lighting)
  {
    glEnable     (GL_LIGHTING);

    glMaterialfv (glSide,GL_AMBIENT,   ambient);
    glMaterialfv (glSide,GL_DIFFUSE,   diffuse);
    glMaterialfv (glSide,GL_SPECULAR,  specular);
    glMaterialfv (glSide,GL_EMISSION,  emission);
    glMaterialf  (glSide,GL_SHININESS, power * 128.0f);
  }
  else
  {
    glColor3fv (wire_color);
    glDisable  (GL_LIGHTING);
  }

  if (alpha_test)
  {
    glEnable (GL_ALPHA_TEST);
    glAlphaFunc (alpha_test->func,alpha_func->ref);
  }
  else 
    glDisable (GL_ALPHA_TEST);

  if (blend)
  {
    glEnable (GL_BLEND);
    glBlendFunc (blend->src,blend->dest);
    glBlendEquation (blend->mode);
    glBlendColor4fv (blend->constant);
  }
  else
    glDisable (GL_BLEND);

  return true;
}

