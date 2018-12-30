#ifndef __GAME_OPENGL_CONSOLE__
#define __GAME_OPENGL_CONSOLE__

#include <console\conbase.h>
#include <graph\grtypes.h>
#include <time.h>

#include "defs.h"

////////////////////////////////////////////////////////////////////////////////
///Консоль средствами OpenGL
////////////////////////////////////////////////////////////////////////////////
class    OGLConsole: public GraphConsole
{
  public:
                        OGLConsole   (Pool*,
                                      ConsoleBase&,
                                      int font = 0,
                                      int width = 80,
                                      int height = 25
                                     );
          virtual       ~OGLConsole  ();

                        enum {
                          CON_DOWN,
                          CON_UP,
                          CON_DISABLE,
                          CON_ENABLE
                        };

////////////////////////////////////////////////////////////////////////////////
///Отрисовка
////////////////////////////////////////////////////////////////////////////////
          void          Draw          ();

          void          SetSpeed      (uint speed) { mSpeed = speed; }
          void          SetMode       (uint mode) { mMode = mode; }          
          void          ChangeMode    ();

////////////////////////////////////////////////////////////////////////////////
///Изменение атрибутов
////////////////////////////////////////////////////////////////////////////////
          void          SetTextColor  (long color) { mTextColor = color & 0xF; }
  virtual void          textcolor     (long color) { SetTextColor (color); }

          void          SetFont       (int font) { mFont = font; }

          static        rgb_t colorTable [16];

  protected:                  
    virtual     void    writeln (int,const char*);
    virtual     void    begin   ();
    virtual     void    end     ();    

  protected:
        int             mFont;
        long            mTextColor;
        int             mLineHeight, mCharWidth, mConWidth, mConHeight;

        int             mOldWidth, mOldHeight;
        int             mConTexture [2];
        vector3d_t      mOffs [2];

        int             mCurHeight;
        uint            mSpeed;
        uint            mMode;
};

#endif