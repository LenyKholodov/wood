#ifndef __INPUT_CLIENT__
#define __INPUT_CLIENT__

#include <types.h>
#include "i_event.h"

class    InputTranslator;
class    InputEvent;
class    KeyEvent;
class    MouseEvent;
class    TimeEvent;

////////////////////////////////////////////////////////////////////////////////
///������ �����
////////////////////////////////////////////////////////////////////////////////
class    InputClient
{
  public:
                InputClient  (InputTranslator&);
   virtual      ~InputClient ();

           InputTranslator&  input () const { return (InputTranslator&)mInput; }                     

////////////////////////////////////////////////////////////////////////////////
///����饭�� ᮮ�饭�� �� ��ࠡ���
////////////////////////////////////////////////////////////////////////////////
           void put          (InputEvent& msg)     { msg.process (*this); }

   virtual void event        (const KeyEvent&)     {}
   virtual void event        (const MouseEvent&)   {}
   virtual void event        (const TimeEvent&)    {}   

  protected:
    InputTranslator&    mInput;                
};

#endif