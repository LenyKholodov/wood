#include <kernel.h>
#include <render\render.h>
#include <render\nodepass.h>
#include <render\startup.h>
#include <kernel\app.h>

#include <render\interface\text.h>
#include <render\fontdefs.h>
#include <render\bitmap.h>

#include <gl\gl.h>

class TestFont: public TrueTypeFont
{
  public:
        TestFont (const char*);

  protected:
    virtual bool SetCurrent ();

  private:
    Bitmap      bmp;
};

void     DoTest ();

int main (int argc,char* argv [])
{
  K_FastInit (NULL);

  DoTest ();

  return EXIT_SUCCESS;
}

void     DoTest ()
{
  SetRenderPool (M_GetPool ());

  Render        render;    
  RenderStartup startup;
  Viewport      vp;
  RenderText    text;  

  render.RegisterInterface (text);

  TestFont  font ("image.jpg"); 
//  RasterFont  font;  

//  vp.GetProj ().SetOrtho (-10,10,-10,10,0,100);
  vp.GetProj ().SetPerspective (-10,10,-10,10,4,100);

  render.SetContext (&startup);

  render.SetViewport(&vp);

//  font.fill = WF_TT_DRAW_LINES;
  font.charset = RUSSIAN_CHARSET;
  font.face = "Comic Sans MS";

//  glColor3f         (1,0,0);
  glColor3f         (1,1,1);
  glClearColor      (0,0,0,0);
  glClear           (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable          (GL_DEPTH_TEST);

  glTranslatef      (-10,0,-5);
  glRotatef         (30,0,1,0);
  glRotatef         (30,1,0,0);
  glScalef          (4,4,4);

  render.BeginDraw  ();

  text.SetCurrent   (font);
//  text.SetTextScreenSize (20,20);
  text.SetSimbolSize (100,400);
//  text.Print3D      (Matrix (1),"Привет мир!");
  text.Print2D      (0,500,"Привет мир!");

  render.EndDraw    ();

  MainLoop          ();
}


bool TestFont::SetCurrent ()
{
  if (TrueTypeFont::SetCurrent ())
  {
    glEnable (GL_TEXTURE_2D);
    glEnable   (GL_TEXTURE_GEN_S);
    glEnable   (GL_TEXTURE_GEN_T);

    static GLfloat s_params[4] = {1.0f,1.0f,0.0f,1};
    static GLfloat t_params[4] = {0.817f,-0.817f,0.817f,1};

    glTexGeni  (GL_S,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
    glTexGeni  (GL_T,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
    glTexGenfv (GL_S,GL_OBJECT_PLANE,s_params);
    glTexGenfv (GL_T,GL_OBJECT_PLANE,t_params);


//    glTexGeni  (GL_S,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
//    glTexGeni  (GL_T,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
    glTexEnvi  (GL_TEXTURE_2D,GL_TEXTURE_ENV,GL_REPLACE);

    GetRender ().GetMatEnv ().SetCurrent (bmp);
    return true;
  }

  return false;
}

TestFont::TestFont (const char* name) : bmp (name)
{   }
