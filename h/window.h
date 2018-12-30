#ifndef __WINDOW_MANAGER__
#define __WINDOW_MANAGER__

#include <window\window.h>
#include <window\smpwindow.h>
#include <window\fswindow.h>
#include <window\popupwindow.h>

/*
  Типы Win32 API окон
*/

enum Win32APIWindows
{
  SIMPLE_WINDOW = 1,
  SIMPLE_WINDOW_CHILD,
  FULLSCREEN_WINDOW,
  FULLSCREEN_WINDOW_CHILD,
  POPUP_WINDOW,
  POPUP_WINDOW_CHILD
};

/*
  Создаёт окно по заданным параметрам
*/

Window*         IF_CreateWindow (int _Type,Pool*,...);

/*
  Удаляет окно
*/

void            IF_CloseWindow  (Window*);

#endif