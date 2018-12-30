#include <pch.h>
#include <screen\screen.h>
#include <screen\mscreen.h>
#include <screen\fscreen.h>
#include <screen\gdiscreen.h>
#include <window.h>
#include <debug.h>

memScreen*    Screen::create   (Window* window)
{
  return new GDIScreen (window);
}

Screen*       Screen::create   (Window*,const crect_t&)
{
  return NULL;
}

memScreen*    Screen::create   (Screen* src,int bpp)
{
  memScreen* screen = NULL;

  switch (bpp)
  {
    case 32: screen = new fScreen32bit (src->width (),src->height (),NULL);
    case  8: screen = new fScreen8bit  (src->width (),src->height (),NULL);
  }

  if (!::IsValid (screen))
    return NULL;

  screen->RefreshScreen (src);

  return screen;
}
