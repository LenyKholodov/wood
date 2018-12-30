#include <pch.h>
#include <graph\hlpdraw.h>

void DrawBoundBox (const boundbox_t& box)
{
  vertex_t   v0 (box.min.x,     box.min.y,      box.max.z);
  vertex_t   v1 (box.max.x,     box.min.y,      box.max.z);
  vertex_t   v2 (box.max.x,     box.min.y,      box.min.z);
  vertex_t   v3 (box.min.x,     box.min.y,      box.min.z);
  vertex_t   v4 (box.min.x,     box.max.y,      box.max.z);
  vertex_t   v5 (box.max.x,     box.max.y,      box.max.z);
  vertex_t   v6 (box.max.x,     box.max.y,      box.min.z);
  vertex_t   v7 (box.min.x,     box.max.y,      box.min.z);  

  glBegin (GL_LINE_LOOP);
    glVertex3fv ((GLfloat*)&v0);
    glVertex3fv ((GLfloat*)&v1);
    glVertex3fv ((GLfloat*)&v2);
    glVertex3fv ((GLfloat*)&v3);
  glEnd   ();
  glBegin (GL_LINE_LOOP);
    glVertex3fv ((GLfloat*)&v4);
    glVertex3fv ((GLfloat*)&v5);
    glVertex3fv ((GLfloat*)&v6);
    glVertex3fv ((GLfloat*)&v7);
  glEnd   ();
  glBegin (GL_LINES);
    glVertex3fv ((GLfloat*)&v0);
    glVertex3fv ((GLfloat*)&v4);
    glVertex3fv ((GLfloat*)&v1);
    glVertex3fv ((GLfloat*)&v5);
    glVertex3fv ((GLfloat*)&v2);
    glVertex3fv ((GLfloat*)&v6);
    glVertex3fv ((GLfloat*)&v3);
    glVertex3fv ((GLfloat*)&v7);
  glEnd   ();  
}
