#ifndef __INPUT_EVENT__
#define __INPUT_EVENT__

#include <types.h>

class    InputClient;

////////////////////////////////////////////////////////////////////////////////
///����饭�� �����
////////////////////////////////////////////////////////////////////////////////
class   InputEvent
{
  public:
        virtual void    process (InputClient&) = 0;
};


#endif