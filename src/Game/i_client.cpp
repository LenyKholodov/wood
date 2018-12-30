#include "pch.h"
#include "i_main.h"

InputClient::InputClient  (InputTranslator& _Input)
            : mInput (_Input)
{   }

InputClient::~InputClient ()
{
  D_Try
  {
    for (int i=InputTranslator::ID_KEYBOARD;i<=InputTranslator::ID_TIMER;i++)
      if (mInput.GetFocus (i) == this)
        mInput.SetFocus (i,NULL);
  }
  D_Exc ("InputClient: Error at destruct");
}