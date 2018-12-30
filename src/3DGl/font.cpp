#include <pch.h>
#include <graph\font.h>
#include <graph\bmp.h>
#include <graph\tga.h>
#include <graph\jpeg.h>
#include <stdarg.h>
#include <stdio.h>

struct     font_t;
void       DestroyFont (font_t&);
bitmap_t*  LoadBitmap  (BinFile*,Pool*);

struct   font_t
{
  int   texture;
  int   list, count;        
  int   use;

  glfont_t  glFont;

        font_t (): texture (-1), list (-1), count (0), use (0) {}
        ~font_t () {
          DestroyFont (*this);
        }
};

static font_t FontTable [MAX_FONTS];
static int    CurFont = 0;
static int    ScreenWidth = 100, ScreenHeight = 100;

BOOL  LoadFont  
 ( int          fntindex,
   BinFile*     file,
   Pool*        pool,
   float        step,
   int          count,
   int          xstart,
   int          ystart)
{
  BOOL      push = FALSE;
  bitmap_t* bmp  = NULL;

  D_Try
  {
    if (fntindex < 0 || fntindex >= MAX_FONTS)
    {
      #ifdef DEBUG
        dout<<"LoadFont: Wrong index: "<<fntindex<<endl;
      #endif
      return;  //throw
//      D_Error (EXCEPTION_ERROR);
    }

    font_t& font = FontTable [fntindex];

    glGenTextures            (1,(uint*)&font.texture);
    font.list = glGenLists   (font.count = count);  

    if (!(bmp = LoadBitmap (file,pool)))
    {
      #ifdef DEBUG
        dout<<"LoadFont: Error at load bitmap"<<endl;
      #endif
      return;  //throw
//      D_Error (EXCEPTION_ERROR);
    }

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    push = TRUE;
    step = fabs (step);

    static int fmt [5] = {0,GL_LUMINANCE,0,GL_RGB,GL_RGBA};
    int        format  = fmt [bmp->bpp>>3];
    float      cx, cy,
               xs    = float (xstart) / float(bmp->width),
               ys    = float (ystart) / float(bmp->height),
               xstep = 1.0f / float (bmp->width >> 4),
               ystep = 1.0f / float (bmp->height >> 4);

    if (!format)
    {
      #ifdef DEBUG
        dout<<"LoadFont: Wrong bitmap bpp: "<<bmp->bpp<<endl;
      #endif
      return;  //throw
//      D_Error (EXCEPTION_ERROR);     
    }

    glBindTexture    (GL_TEXTURE_2D,font.texture);
    glTexParameteri  (GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri  (GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);


    glTexImage2D     (GL_TEXTURE_2D,0,bmp->bpp>>3,bmp->width,bmp->height,
                      0,format,GL_UNSIGNED_BYTE,bmp->data);

    pool->free    (bmp);
    bmp = NULL;
        
    glPushMatrix();

    for (int i=0;i<count;i++)
    {
      cx=float(i&15) / 16.0f;
      cy=float(i>>4) / 16.0f;

      glNewList (font.list + i,GL_COMPILE);
      glBegin   (GL_QUADS);
        glTexCoord2f (xs + cx,ys + 1.0f - cy - ystep);
        glVertex2i   (0,16);
        glTexCoord2f (xs + cx + xstep,ys + 1.0f - cy - ystep);
        glVertex2i   (16,16);
        glTexCoord2f (xs + cx + xstep,ys + 1.0f - cy);
        glVertex2i   (16,0);
        glTexCoord2f (xs + cx,ys + 1.0f - cy);
        glVertex2i   (0,0);
      glEnd();
                        
      glTranslated (16.0f * (step+1),0,0);
      glEndList    ();
    }

    glPopMatrix ();
    glPopAttrib ();

    push      = FALSE;
    font.use  = TRUE;

    font.glFont.count       = count;
    font.glFont.step        = step;
    font.glFont.simb.width  = 16;
    font.glFont.simb.height = 16;

    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"LoadFont: Error at load font: "<<fntindex<<endl;
    #endif
    
    D_Try
    {
      if (push) glPopAttrib ();
      if (bmp)  pool->free  (bmp);
    }
    D_Exception (EXCEPTION_EXECUTE_HANDLER)
    {
    }

    DestroyFont (fntindex);

    return FALSE;
  }
}

BOOL  LoadFont  
 ( int          fntindex,
   const  char* name,
   Pool*        pool,
   float        step,
   int          count,
   int          xstart,
   int          ystart)
{
  D_Try
  {
    BinFile* file = F_OpenFile (name,BF_OPEN_RO);

    bool res = LoadFont (fntindex,file,pool,step,count,xstart,ystart);

    delete file;

    return res;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"LoadFont: Error at open image-file"<<endl;
    #endif
    return FALSE;
  }
}

void     DestroyFont    (font_t& font)
{
  D_Try
  {
    if (font.use)
    {
      glDeleteLists    (font.list,font.count);
      glDeleteTextures (1,(uint*)&font.texture);

      font.texture = -1;
      font.use     = FALSE;
      font.list    = -1;
      font.count   = 0;
    }  
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"DestroyFont: Error at destroy"<<endl;
    #endif
  }
}

void     DestroyFont    (int font)
{
  if (font >= 0 && font < MAX_FONTS)
    DestroyFont (FontTable [font]);
}

void     SetFont        (int font)
{
  if (font >= 0 && font < MAX_FONTS) 
    CurFont = font;  
}

int      GetFont        ()
{
  return CurFont;
}

BOOL     GetFont        (int font,glfont_t& glFont)
{
  if (FontTable [font].use)
  {
    glFont = FontTable [font].glFont;
    return TRUE;
  }
  else
    return FALSE;
} 

void     SetScreenSize  (int width,int height)
{
  if (width > 0 && height > 0)
  {
    ScreenWidth  = width;
    ScreenHeight = height;
  }
}

void     GetScreenSize  (int& w,int& h)
{
  w = ScreenWidth;
  h = ScreenHeight;
}

void     OutText        (int x,int y,const char* string,...)
{
  char    text [MAX_TEXT_MESSAGE];
  va_list aPrint;
  font_t& font = FontTable [CurFont];

  if (!string)
    return;

  va_start   (aPrint,string);
    vsprintf (text,string,aPrint);
  va_end     (aPrint);
        
  glPushAttrib  (GL_ALL_ATTRIB_BITS);

  glEnable      (GL_BLEND);
  glDisable     (GL_CULL_FACE);
  glBlendFunc   (GL_SRC_ALPHA,GL_ONE);
  glBindTexture (GL_TEXTURE_2D,font.texture);
//  glTexEnvi     (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_BLEND); 

  glDisable     (GL_DEPTH_TEST);
  glDisable     (GL_LIGHTING);
  glEnable      (GL_TEXTURE_2D);

  glMatrixMode  (GL_PROJECTION);
  glPushMatrix  ();
  glLoadIdentity();
  glOrtho       (0,ScreenWidth,ScreenHeight,0,-1,1);
  glMatrixMode  (GL_MODELVIEW);
  glPushMatrix  ();
  glLoadIdentity();
  glTranslated  (x,y,0);

  glListBase    (font.list-32); //???
  glCallLists   (strlen(text),GL_BYTE,text);       

  glMatrixMode  (GL_PROJECTION); //???
  glPopMatrix   ();
  glMatrixMode  (GL_MODELVIEW);
  glPopMatrix   ();

  glEnable      (GL_DEPTH_TEST);

  glPopAttrib   ();
}

void     OutText        (int x,int y,int findex,const char* string,...)
{
  if (findex >= 0  && findex < MAX_FONTS)
  {
    char    text [MAX_TEXT_MESSAGE];
    va_list aPrint;
    font_t& font = FontTable [findex];

    if (!string)
      return;

    va_start   (aPrint,string);
      vsprintf (text,string,aPrint);
    va_end     (aPrint);
          
    glPushAttrib  (GL_ALL_ATTRIB_BITS);

    glEnable      (GL_BLEND);
    glBlendFunc   (GL_SRC_ALPHA,GL_ONE);
    glBindTexture (GL_TEXTURE_2D,font.texture);
    glTexEnvi     (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE); 
//    glTexEnvi     (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL); 
//    glTexEnvi     (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_BLEND); 

    glDisable     (GL_DEPTH_TEST);
    glDisable     (GL_LIGHTING);
    glDisable     (GL_CULL_FACE);
    glEnable      (GL_TEXTURE_2D);

    glMatrixMode  (GL_PROJECTION);
    glPushMatrix  ();
    glLoadIdentity();
    glOrtho       (0,ScreenWidth,ScreenHeight,0,-1,1);
    glMatrixMode  (GL_MODELVIEW);
    glPushMatrix  ();
    glLoadIdentity();
    glTranslated  (x,y,0);

    glListBase    (font.list-32); //???
    glCallLists   (strlen(text),GL_BYTE,text);       

    glMatrixMode  (GL_PROJECTION); //???
    glPopMatrix   ();
    glMatrixMode  (GL_MODELVIEW);
    glPopMatrix   ();

    glEnable      (GL_DEPTH_TEST);

    glPopAttrib   ();
  }
}

void     OutText        (int x,int y,int findex,const vector3d_t& offs,const char* string,...)
{
  if (findex >= 0  && findex < MAX_FONTS)
  {
    char    text [MAX_TEXT_MESSAGE];
    va_list aPrint;
    font_t& font = FontTable [findex];

    if (!string)
      return;

    va_start   (aPrint,string);
      vsprintf (text,string,aPrint);
    va_end     (aPrint);
          
    glPushAttrib  (GL_ALL_ATTRIB_BITS);

    glEnable      (GL_BLEND);

    glBlendFunc   (GL_SRC_ALPHA,GL_ONE);
    glBindTexture (GL_TEXTURE_2D,font.texture);
//    glTexEnvi     (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL); 
//    glTexEnvi     (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_BLEND); 

    glDisable     (GL_DEPTH_TEST);
    glDisable     (GL_LIGHTING);
    glDisable     (GL_CULL_FACE);
    glEnable      (GL_TEXTURE_2D);

    glMatrixMode  (GL_PROJECTION);
    glPushMatrix  ();
    glLoadIdentity();
    glOrtho       (0,ScreenWidth,ScreenHeight,0,-1,1);
    glMatrixMode  (GL_MODELVIEW);
    glPushMatrix  ();
    glLoadIdentity();
    glTranslatef  (offs.x+x,offs.y+y,0);

    glListBase    (font.list-32); //???
    glCallLists   (strlen(text),GL_BYTE,text);       

    glMatrixMode  (GL_PROJECTION); //???
    glPopMatrix   ();
    glMatrixMode  (GL_MODELVIEW);
    glPopMatrix   ();

    glEnable      (GL_DEPTH_TEST);

    glPopAttrib   ();
  }
}


bitmap_t*  LoadBitmap  (BinFile* file,Pool* pool)
{
  bitmap_t*     bmp     = NULL;

  if (file)
  {  
    if (!(bmp=LoadTGA (file,pool,cerr)))
      if (!(bmp=LoadBMP (file,pool,cerr)))
        if (!(bmp=LoadJPEG (file,pool,cerr)))
          return NULL;
  }
  else
    return NULL;

  return bmp;
}