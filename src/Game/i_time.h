#ifndef __INPUT_TIME__
#define __INPUT_TIME__

#include "i_event.h"
#include "i_client.h"

////////////////////////////////////////////////////////////////////////////////
///Сообщения таймера
////////////////////////////////////////////////////////////////////////////////
class   TimeEvent: public InputEvent
{
  public:
        virtual void    process (InputClient& client) { client.event (*this); }

  public:
        uint    clock_t;
        DWORD   param;
};


#endif