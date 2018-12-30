#include <gl/glut.h>
#include "vb.h"

struct vertex: public Vertex::vertex<float,3>
{ 
  vertex (float x,float y,float z) { v = vec3f (x,y,z); }
};

void draw ()
{
  glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);  

  static vertex vertexes [3] = {
    vertex (-10,-10,0),
    vertex ( 10,-10,0),
    vertex (  0, 10,0),
  };

  Vertex::SetVertexArray (vertexes);

  glDrawArrays (GL_TRIANGLES,0,3);

  glutSwapBuffers ();
}

void reshape (int w,int h)
{
  glViewport (0,0,w,h);
}

void main (int argc,char* argv [])
{
  glutInit (&argc,argv);
  glutInitWindowSize (800,600);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE);
  glutCreateWindow ("Vertex arrays test");
  glutReshapeFunc (reshape);  
  glutDisplayFunc (draw);

  glClearColor (0,0.4,0.4,0);
  glEnable (GL_DEPTH_TEST);    
  glOrtho  (-20,20,-20,20,-100,100);

  glutMainLoop ();
}
