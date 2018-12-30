#include "pch.h"
#include <windows.h>
#include <kernel\keydefs.h>
#include "i_main.h"

static char digkeyshift [10] = {
  ')', '!', '@', '#', '$', '%', '^', '&', '*', '('
};

static char oemkeyshift [11] = {
 ':', '+', '<', '_', '>', '?', '~', '{', '|', '}', '"'  
};

static char oemkey [11] = {
  ';', '=', ',', '-', '.', '/', '`', '[', '\\', ']', 0x27
};

void  InputTranslator::SetFocus         (uint id,InputClient* client)
{
  switch (id)
  {
    case ID_KEYBOARD:
    {
      if (mKeyboard)
      {
        for (int i=0;i<256;i++)
          if (mKeyPress [i])
            mKeyboard->put (KeyEvent (i,0,KeyEvent::KE_UP));
      }

      mKeyboard = client;      

      break;
    }  
    case ID_MOUSE:
    {
      if (mMouse)
      {
           //не совсем корректно
        if (mMousePress [0]) mMouse->put (MouseEvent (0,0,MouseEvent::ME_LBUTTON_UP));
        if (mMousePress [1]) mMouse->put (MouseEvent (0,0,MouseEvent::ME_RBUTTON_UP));
        if (mMousePress [2]) mMouse->put (MouseEvent (0,0,MouseEvent::ME_MBUTTON_UP));
      }

      mMouse = client;

      break;
    }  
    default:
      break;
  }
}

InputClient*  InputTranslator::GetFocus         (uint id) const
{
  switch (id)
  {
    case ID_KEYBOARD: return mKeyboard;
    case ID_MOUSE:    return mMouse;
    default:          return NULL;    
  }
}

void   InputTranslator::GetMousePos      (int& x,int& y)
{
  POINT p;

  GetCursorPos(&p);

  x = p.x;
  y = p.y;
}

void   InputTranslator::SetMousePos      (int x,int y)
{
  SetCursorPos (x,y);
}

int    InputTranslator::handler          (Window*,UINT msg,WPARAM key,LPARAM lParam,DWORD par)
{
  D_Try
  {
    uint             message = 0;
    InputTranslator* input   = (InputTranslator*)par;

    switch (msg)
    {
      case WM_MOUSEMOVE:       
        if (input->mMouse)
          input->mMouse->put (MouseEvent (LOWORD(lParam),HIWORD(lParam),MouseEvent::ME_MOVE));
        break;
      case WM_LBUTTONDOWN:
        input->mMousePress [0] = TRUE;
        if (input->mMouse)
          input->mMouse->put (MouseEvent (LOWORD(lParam),HIWORD(lParam),MouseEvent::ME_LBUTTON_DOWN));
        break;
      case WM_LBUTTONUP:       
        input->mMousePress [0] = FALSE;
        if (input->mMouse)
          input->mMouse->put (MouseEvent (LOWORD(lParam),HIWORD(lParam),MouseEvent::ME_LBUTTON_UP));
        break;
      case WM_RBUTTONDOWN:     
        input->mMousePress [1] = TRUE;
        if (input->mMouse)
          input->mMouse->put (MouseEvent (LOWORD(lParam),HIWORD(lParam),MouseEvent::ME_RBUTTON_DOWN));
        break;
      case WM_RBUTTONUP:  
        input->mMousePress [1] = FALSE; 
        if (input->mMouse)
          input->mMouse->put (MouseEvent (LOWORD(lParam),HIWORD(lParam),MouseEvent::ME_RBUTTON_UP));
        break;
      case WM_MBUTTONDOWN:     
        input->mMousePress [2] = TRUE; 
        if (input->mMouse)
          input->mMouse->put (MouseEvent (LOWORD(lParam),HIWORD(lParam),MouseEvent::ME_MBUTTON_DOWN));
        break;
      case WM_MBUTTONUP:  
        input->mMousePress [2] = FALSE; 
        if (input->mMouse)
          input->mMouse->put (MouseEvent (LOWORD(lParam),HIWORD(lParam),MouseEvent::ME_MBUTTON_UP));
        break;
      case WM_KEYDOWN:         
        if (!input->mKeyPress [key&0xFF]) message = KeyEvent::KE_DOWN;
        else                              message = KeyEvent::KE_LOOP;

        input->mKeyPress [key&0xFF] = TRUE;

        if (input->mKeyboard) 
          input->mKeyboard->put (KeyEvent (key,input->getsimb (key),message));

        break;
      case WM_KEYUP:
        input->mKeyPress [key&0xFF] = FALSE;
        if (input->mKeyboard) 
          input->mKeyboard->put (KeyEvent (key,0,KeyEvent::KE_UP));
        break;
      default:
        return 0;  
    }    

    return 0;
  }
  D_ExcRet ("InputTranslator: Error at handler",0);
}

uchar     InputTranslator::getsimb  (uint key)
{
  BOOL shift = mKeyPress [VK_SHIFT];

  if (key == VK_SPACE)            return ' ';
  if (key == VK_RETURN)           return '\n';
  if (key == VK_BACK)             return 8;
  if (key == VK_TAB)              return '\t';
/*  if (key == VK_MULTIPLY)         return '*';
  if (key == VK_SEPARATOR)        return '\\';
  if (key == VK_DIVIDE)           return '/';
  if (key == VK_ADD)              return '+';
  if (key == VK_SUBTRACT)         return '-';*/

//  if (!shift)
//  {
    if (key >= VK_0 && key <= VK_9) return key-VK_0+'0';
    if (key >= VK_A && key <= VK_Z) return key-VK_A+'a';
/*  }
  else
  {
    if (key >= VK_0 && key <= VK_9) return digkeyshift [key-VK_0];
    if (key >= VK_A && key <= VK_Z) return key-VK_A+'A';
  }*/

  return 0;
}

InputTranslator::InputTranslator  (Pool* _Pool,Window* _Window)
                : MemObject (_Pool),
                  mWnd      (_Window),
                  mKeyboard (NULL), mMouse (NULL)
{
  D_Try
  {
    memset (mKeyPress,0,sizeof (mKeyPress));

    mWnd->InsertHandler    (WM_KEYDOWN,          handler, (DWORD)this);
    mWnd->InsertHandler    (WM_KEYUP,            handler, (DWORD)this);
    mWnd->InsertHandler    (WM_MOUSEMOVE,        handler, (DWORD)this);
    mWnd->InsertHandler    (WM_LBUTTONDOWN,      handler, (DWORD)this);
    mWnd->InsertHandler    (WM_LBUTTONUP,        handler, (DWORD)this);
    mWnd->InsertHandler    (WM_RBUTTONDOWN,      handler, (DWORD)this);
    mWnd->InsertHandler    (WM_RBUTTONUP,        handler, (DWORD)this);
    mWnd->InsertHandler    (WM_MBUTTONDOWN,      handler, (DWORD)this);
    mWnd->InsertHandler    (WM_MBUTTONUP,        handler, (DWORD)this);
  }
  D_Exc ("InputTranslator: Error at construct");
}

InputTranslator::~InputTranslator ()
{  
  D_Try
  {
    for (int i=ID_KEYBOARD;i<=ID_TIMER;i++)
      SetFocus (i,NULL);

    mWnd->InsertHandler    (WM_KEYDOWN,          NULL);
    mWnd->InsertHandler    (WM_KEYUP,            NULL);
    mWnd->InsertHandler    (WM_MOUSEMOVE,        NULL);
    mWnd->InsertHandler    (WM_LBUTTONDOWN,      NULL);
    mWnd->InsertHandler    (WM_LBUTTONUP,        NULL);
    mWnd->InsertHandler    (WM_RBUTTONDOWN,      NULL);
    mWnd->InsertHandler    (WM_RBUTTONUP,        NULL);
    mWnd->InsertHandler    (WM_MBUTTONDOWN,      NULL);
    mWnd->InsertHandler    (WM_MBUTTONUP,        NULL);
  }
  D_Exc ("InputTranslator: Error at destruct");
}

