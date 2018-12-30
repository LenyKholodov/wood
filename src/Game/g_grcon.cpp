#include "pch.h"
#include "g_grcon.h"
#include <gl\gl.h>
#include <graph\font.h>
#include <graph\textenv.h>
#include <ctype.h>
#include <misc.h>
#include <stdlib.h>

rgb_t         OGLConsole::colorTable [16] ={
  {0,0,0},
  {0,0,128},  
  {0,128,0},
  {0,128,128},  
  {128,0,0},  
  {128,0,128},
  {128,128,0},
  {128,128,128},  

  {200,200,200},
  {0,0,255},  
  {0,255,0},
  {0,255,255},  
  {255,0,0},
  {255,0,255},  
  {255,255,0},
  {255,255,255}
};

void    OGLConsole::ChangeMode    ()
{
  switch (mMode)
  {
    case CON_ENABLE:
    case CON_DOWN:
      mMode = CON_UP;
      break;
    case CON_DISABLE:
    case CON_UP:
      mMode = CON_DOWN;
      break;
  }
}

void    OGLConsole::Draw          ()
{
  D_Try
  {
    if (mMode == CON_DISABLE)
      return;

    mConBase.refresh ();

    if (mMode == CON_DOWN) 
      if (mCurHeight < mConHeight) 
        if ((mCurHeight += mSpeed) >= mConHeight)  
        {
          mCurHeight = mConHeight - 1;
          mMode      = CON_ENABLE;
        }  

    if (mMode == CON_UP) 
      if (mCurHeight > 0) 
        if ((mCurHeight -= mSpeed) < 0)  
        {
          mCurHeight = -1;
          mMode      = CON_DISABLE;
        }  
  }
  D_Exc ("OGLConsole: Error at Draw");
}

void    OGLConsole::begin   ()
{
  GetScreenSize (mOldWidth,mOldHeight);
  SetScreenSize (mConWidth,mConHeight*2); 
  
  glPushAttrib  (GL_DEPTH_TEST);
  glPushAttrib  (GL_LIGHTING);
  glPushAttrib  (GL_CULL_FACE);

  glDisable     (GL_DEPTH_TEST);
  glDisable     (GL_LIGHTING);
  glDisable     (GL_CULL_FACE);

  glMatrixMode  (GL_PROJECTION);
  glPushMatrix  ();
  glLoadIdentity();
  glOrtho       (0,mConWidth,0,mConHeight*2,-2,2);
  glTranslatef  (0,mConHeight*2,0);
  glScalef      (1,-1,1);       
  glMatrixMode  (GL_MODELVIEW);
  glPushMatrix  ();
  glLoadIdentity();

  glColor3f     (1,1,1);

  if (glIsTexture (mConTexture [0]))
  {
    glBindTexture (GL_TEXTURE_2D,mConTexture [0]);

    glMatrixMode  (GL_TEXTURE);
    glPushMatrix  ();
    glTranslatef  (mOffs [0].x,mOffs [0].y,mOffs [0].z);

    glBegin       (GL_QUADS);
      glTexCoord2f  (0,1);      glVertex2i (0,mCurHeight);
      glTexCoord2f  (1,1);      glVertex2i (mConWidth,mCurHeight);
      glTexCoord2f  (1,0);      glVertex2i (mConWidth,mCurHeight-mConHeight);
      glTexCoord2f  (0,0);      glVertex2i (0,mCurHeight-mConHeight);
    glEnd         ();

    if (glIsTexture (mConTexture [1]))
    {
      glEnable     (GL_BLEND);

      glTranslatef  (mOffs [1].x,mOffs [1].y,mOffs [1].z);

      glBindTexture (GL_TEXTURE_2D,mConTexture [1]);
      glColor4f     (1,1,1,0.5);

      glBegin       (GL_QUADS);
        glTexCoord2f  (0,1);      glVertex2i (0,mCurHeight);
        glTexCoord2f  (1,1);      glVertex2i (mConWidth,mCurHeight);
        glTexCoord2f  (1,0);      glVertex2i (mConWidth,mCurHeight-mConHeight);
        glTexCoord2f  (0,0);      glVertex2i (0,mCurHeight-mConHeight);
      glEnd         ();

      glDisable (GL_BLEND);
    }

    glPopMatrix  ();
    glMatrixMode (GL_MODELVIEW);
  }
  else
  {
    glBegin       (GL_QUADS);
      glVertex2i (0,mCurHeight);
      glVertex2i (mConWidth,mCurHeight);
      glVertex2i (mConWidth,mCurHeight-mConHeight);
      glVertex2i (0,mCurHeight-mConHeight);
    glEnd         ();
  }  

  mOffs [0] += vector3d_t (0.001,0.002,0);
  mOffs [1] += vector3d_t (-0.002,-0.001,0);

  glMatrixMode  (GL_PROJECTION);
  glPopMatrix   ();
  glMatrixMode  (GL_MODELVIEW);
  glPopMatrix   ();

  glPopAttrib   ();
  glPopAttrib   ();
  glPopAttrib   ();   
}

void    OGLConsole::end     ()
{
  SetScreenSize    (mOldWidth,mOldHeight);
}

void    OGLConsole::writeln (int line,const char* msg)
{
  int    y   = line * mLineHeight;
  int    x   = 0;

  glColor3ubv   ((GLubyte*)(colorTable + mTextColor));

  if (strchr (msg,'^'))
  {
    char*  str = (char*)msg;
    char*  lst = str;

    for (str=strchr (str,'^');str;str=strchr (str,'^'))
    {
      if (lst!=str)
      {
        *str = 0;

        OutText  (x,y,mFont,vector3d_t (0,mCurHeight-mConHeight,0),lst);

        x     += strlen (lst) * mCharWidth;

        *str++ = '^';       
      }                                
      else
        str++;

      if (isdigit (*str))
      {
        char buf [16];
        for (int i=0;*str && isdigit (*str);buf [i++]=*str++);

        buf [i]   = 0;

        SetTextColor (atoi (buf) & 0xF);
        glColor3ubv  ((GLubyte*)(colorTable + mTextColor));

        lst = str;
      }           
      else
        lst = str - 1;
    }

    if (lst && *lst)
      OutText  (x,y,mFont,vector3d_t (0,mCurHeight-mConHeight,0),lst);
  }
  else
  {
    glColor3ubv  ((GLubyte*)(colorTable + mTextColor));
    OutText     (0,y,mFont,vector3d_t (0,mCurHeight-mConHeight,0),msg);
  }
}


OGLConsole::OGLConsole   (Pool* _Pool,ConsoleBase& base,int font,int width,int height)
           : GraphConsole (_Pool,base),
             mLineHeight  (0),
             mCharWidth   (0),
             mTextColor   (15), 
             mFont        (font), mConWidth (0), mConHeight (0),
             mCurHeight   (-1), mSpeed (0),
             mMode        (CON_DISABLE)
{
  D_Try
  {
    glfont_t fnt;

    if (!GetFont (font,fnt))
    {
      D_Message ("OGLConsole: Font #"<<font<<" not found");
      return;
    }
   
    mLineHeight = fnt.simb.height;
    mCharWidth  = fnt.simb.width + fnt.step;
    mConWidth   = width * (fnt.simb.width + fnt.step);
    mConHeight  = height * fnt.simb.height;
    mSpeed      = mConHeight / 4;

    mConTexture  [0] = TextEnvironment::CreateTexture (OGL_CONSOLE_TEXTURE0,GetPool());
    mConTexture  [1] = TextEnvironment::CreateTexture (OGL_CONSOLE_TEXTURE1,GetPool());

    base.refresh  (FALSE);

    mOffs [0] = mOffs [1] = vector3d_t (.0f);

    this->width  (width);
    this->height (height);
  }
  D_Exc ("OGLConsole: Error at construct");    
}

OGLConsole::~OGLConsole  ()
{
  D_Try
  {
    if (glIsTexture (mConTexture [0]))
      glDeleteTextures (1,(uint*)&mConTexture [0]);
    if (glIsTexture (mConTexture [1]))
      glDeleteTextures (1,(uint*)&mConTexture [1]);
  }     
  D_Exc ("OGLConsole: Error at destruct");
}
