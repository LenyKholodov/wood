#include <window.h>
#include <screen.h>
#include <time.h>
#include <misc.h>
#include <debug.h>

void     Idle ();

memScreen*  screen    = NULL;
memScreen*  gdi       = NULL;
image_t*    image [5] = {NULL};

int      main (int,char* [])
{
  Window* wnd = new SimpleWindow (rect_t (800,600),"Test Windows");
  gdi         = Screen::create   (wnd);
  screen      = Screen::create   (gdi,8);

  image [0]   = image_t::create  ("back.jpg");
  image [1]   = image_t::create  ("atoll.bmp");
  image [2]   = image_t::create  ("test1.bmp");
  image [3]   = image_t::create  ("test2.bmp");

  wnd->ShowCursor (TRUE);

  MainLoop (Idle);

  for (int i=0;i<5;i++)
    if (image [i])
      image_t::dispose (image [i]);

  delete screen,gdi;

  return EXIT_SUCCESS;
}

void     Idle ()
{
  screen->cls      ();
  screen->bar      (0,0,100,100,screen->rgb (0,0,255));
  screen->refresh  ();
  gdi->refresh     ();
} 
