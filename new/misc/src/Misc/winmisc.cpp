#include "pch.h"

int      WND_Done     (Window*,UINT,WORD,DWORD,DWORD)
{
  PostQuitMessage (0);
  return 0;
}

void     W32_PostQuitMessage (int code)
{
  PostQuitMessage (code);
}

BOOL     W32_SetCursorPos    (int x,int y)
{
  return SetCursorPos (x,y);
}