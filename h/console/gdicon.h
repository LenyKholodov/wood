#ifndef __GDI_CONSOLE__
#define __GDI_CONSOLE__

#include <window\window.h>
#include <console\conbase.h>

////////////////////////////////////////////////////////////////////////////////
///Консоль средствами GDI
////////////////////////////////////////////////////////////////////////////////
class    GDIConsole: public GraphConsole
{
  public:
                        GDIConsole   (Pool*,Window*,ConsoleBase&);
          virtual       ~GDIConsole  ();

////////////////////////////////////////////////////////////////////////////////
///Изменение атрибутов
////////////////////////////////////////////////////////////////////////////////
          void          SetTextColor (long color);
          void          SetBkColor   (long color);
  virtual void          textcolor    (long i) 
            { SetTextColor (RGB (colorTable [i].red,colorTable [i].green,colorTable [i].blue)); }
          
          void          SetFont      (const gdifont_t&);
          void          SetBrush     (const brush_t&);

          static        rgb_t colorTable [16];

  protected:                  
    virtual     void    writeln (int,const char*);
    virtual     void    begin   ();
    virtual     void    end     ();    

  protected:
        Window*         mDrawWnd;
        HFONT           mFont;
        HBRUSH          mBrush;
        long            mTextColor, mBkColor;
        int             mLineHeight, mCharWidth;
};

#endif