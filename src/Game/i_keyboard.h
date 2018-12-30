#ifndef __INPUT_KEYBOARD__
#define __INPUT_KEYBOARD__

#include "i_event.h"
#include "i_client.h"

////////////////////////////////////////////////////////////////////////////////
///Сообщения клавиатуры
////////////////////////////////////////////////////////////////////////////////
class   KeyEvent: public InputEvent
{
  public:
        virtual void    process (InputClient& client) { client.event (*this); }

        enum {
          KE_DOWN,
          KE_UP,
          KE_LOOP
        };

  public:
        uint    key;
        uchar   simb;
        uint    msg;

                KeyEvent (uint _key,uchar _simb,uint _msg):key (_key), simb (_simb), msg (_msg) {}
};


#endif