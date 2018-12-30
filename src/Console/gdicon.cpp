#include <pch.h>
#include <console\gdicon.h>
#include <ctype.h>
#include <stdlib.h>

rgb_t         GDIConsole::colorTable [16] ={
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

void          GDIConsole::SetTextColor (long color)
{
  mDrawWnd->SetTextColor (mTextColor = color);
}

void          GDIConsole::SetBkColor   (long color)
{
  mDrawWnd->SetBkColor (mBkColor = color);
}
          
void          GDIConsole::SetFont      (const gdifont_t& font)
{
  if (mFont) Window::DeleteObject (mFont);

  mFont  = Window::CreateGDIFont  (font);

  mCharWidth  = font.width;
  mLineHeight = font.height;

  width  (mDrawWnd->width ()/font.width-1);
  height (mDrawWnd->height ()/mLineHeight-1);
}

void          GDIConsole::SetBrush     (const brush_t& brush)
{
  if (mBrush) Window::DeleteObject (mBrush);

  mBrush      = Window::CreateBrush (brush);
}

void    GDIConsole::writeln (int line,const char* msg)
{
  int    y   = line * mLineHeight;
  int    x   = 0;

  if (strchr (msg,'^'))
  {
    char*  str = (char*)msg;
    char*  lst = str;

    for (str=strchr (str,'^');str;str=strchr (str,'^'))
    {
      if (lst!=str)
      {
        *str = 0;
        mDrawWnd->OutText (x,y,lst);

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

        int color = atoi (buf) & 0xF;

        SetTextColor (RGB (colorTable [color].red,
                           colorTable [color].green,
                           colorTable [color].blue
                          ) 
                     );

        lst = str;
      }           
      else
        lst = str - 1;
    }

    if (lst && *lst)
      mDrawWnd->OutText (x,y,lst);
  }
  else
  {
    mDrawWnd->SetTextColor (mTextColor);
    mDrawWnd->OutText      (0,y,msg);
  }
}

void    GDIConsole::begin   ()
{
  mDrawWnd->BeginDraw    ();
  mDrawWnd->SetBkColor   (mBkColor);
  mDrawWnd->Cls          (mBrush);
  mDrawWnd->SelectObject (mFont);
}

void    GDIConsole::end     ()
{
  mDrawWnd->EndDraw ();
}

GDIConsole::GDIConsole   (Pool* _Pool,Window* wnd,ConsoleBase& base)
           : GraphConsole (_Pool,base),
             mDrawWnd     (wnd),   //no!!! PopUp
             mLineHeight  (0),
             mTextColor   (RGB (255,255,255)), mBkColor (RGB(0,0,128)),
             mFont        (NULL), mBrush (NULL)
{
//  D_Try
//  {
    static gdifont_t __ConFont   (8,16);
    static brush_t   __ConBrush  (RGB(0,0,128),BS_SOLID,0);

    mCharWidth  = __ConFont.width;
    mLineHeight = __ConFont.height;
    mFont       = Window::CreateGDIFont  (__ConFont);
    mBrush      = Window::CreateBrush (__ConBrush);

    width  (wnd->width ()/__ConFont.width-1);
    height (wnd->height ()/mLineHeight-1);

    mConBase.refresh ();
//  }
//  D_Exc ("GDIConsole: Error at construct");
}

GDIConsole::~GDIConsole  ()
{
//  D_Try
//  {
    if (mFont)  Window::DeleteObject (mFont);
    if (mBrush) Window::DeleteObject (mBrush);
//  }
//  D_Exc ("GDIConsole: Error at destruct");
}
