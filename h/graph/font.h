#ifndef __OGL_FONT_DRAWING__
#define __OGL_FONT_DRAWING__

#include <graph\grtypes.h>
#include <binfile.h>

const    int MAX_FONTS        = 16;
const    int MAX_TEXT_MESSAGE = 512;

struct   glfont_t
{
  struct {
    char width, height;  //Размеры символа
  }simb;

  int           count;  //Количество символов
  float         step;   //Шаг между символами
};

////////////////////////////////////////////////////////////////////////////////
///Система вывода текста в OpenGL
///Поддерживается до MAX_FONTS шрифтов 16x16
////////////////////////////////////////////////////////////////////////////////

BOOL     LoadFont       (int font,
                         BinFile*,
                         Pool*          = ::M_GetPool(),
                         float step     = 0,
                         int count      = 128,
                         int xstart     = 0,
                         int ystart     = 0
                        );
BOOL     LoadFont       (int font,
                         const char*,
                         Pool*          = ::M_GetPool(),
                         float step     = 0,
                         int count      = 128,
                         int xstart     = 0,
                         int ystart     = 0
                        );
void     DestroyFont    (int font);
void     SetFont        (int font);
int      GetFont        ();
BOOL     GetFont        (int,glfont_t&);
void     SetScreenSize  (int width,int height);
void     GetScreenSize  (int&,int&);
void     OutText        (int x,int y,const char*,...);
void     OutText        (int x,int y,int font,const char*,...);
void     OutText        (int x,int y,int font,const vector3d_t& offs,const char*,...);

#endif