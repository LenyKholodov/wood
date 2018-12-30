#ifndef __INPUT_MOUSE__
#define __INPUT_MOUSE__

#include "i_event.h"
#include "i_client.h"

////////////////////////////////////////////////////////////////////////////////
///Сообщения крысы
////////////////////////////////////////////////////////////////////////////////
class   MouseEvent: public InputEvent
{
  public:
        virtual void    process (InputClient& client) { client.event (*this); }

        enum {
          ME_LBUTTON_DOWN,
          ME_LBUTTON_UP,
          ME_RBUTTON_DOWN,
          ME_RBUTTON_UP,
          ME_MBUTTON_DOWN,
          ME_MBUTTON_UP,
          
          ME_MOVE  
          //SCROLL!!!
        };

  public:
        int     x,y;
        uint    msg;

                MouseEvent (int _x,int _y,uint _msg):x(_x), y(_y), msg(_msg) {}
};

#endif
