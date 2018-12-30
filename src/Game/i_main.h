#ifndef __INPUT_SYSTEM_MANAGER__
#define __INPUT_SYSTEM_MANAGER__

#include <pool.h>
#include <window.h>

#include "i_keyboard.h"
#include "i_mouse.h"
#include "i_time.h"

class    Window;

////////////////////////////////////////////////////////////////////////////////
///���࠭���� ᮮ�饭�� �����
////////////////////////////////////////////////////////////////////////////////
class    InputTranslator: public MemObject
{
  public:
                        InputTranslator  (Pool*,Window*);
       virtual          ~InputTranslator ();

                        enum {
                          ID_KEYBOARD = 0,
                          ID_MOUSE,
                          ID_NET,
                          ID_TIMER
                        };

////////////////////////////////////////////////////////////////////////////////
///��४⭠� ᬥ�� ��⮪� ���
////////////////////////////////////////////////////////////////////////////////
       void             SetFocus         (uint id,InputClient*);
       InputClient*     GetFocus         (uint id) const;        

////////////////////////////////////////////////////////////////////////////////
///�����
////////////////////////////////////////////////////////////////////////////////
       BOOL             IsKeyPressed     (uint key) const { return mKeyPress [key & 0xFF]; }

       void             GetMousePos      (int&,int&);
       void             SetMousePos      (int,int);

  private:
       static int       handler          (Window*,UINT,WPARAM,LPARAM,DWORD);
              uchar     getsimb          (uint);

  private:
        Window*         mWnd;

        InputClient*    mKeyboard;
        InputClient*    mMouse; 
//        InputClient*    mTime;

        uchar           mKeyPress [256];
        uchar           mMousePress [3];
};

#endif