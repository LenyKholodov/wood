#include <kernel\app.h>
#include <window.h>

#include <time.h>
#include <screen.h>

void     Idle ();

Window*  wnd    = NULL;
memScreen*  screen = NULL;

int      main (int,char* [])
{
  D_BindMapFile ("test4.map");

  wnd    = new SimpleWindow (M_GetPool(),rect_t (0,0,800,600),"Test Windows");
  screen = Screen::create   (wnd);

  wnd->ShowCursor (TRUE);

  MainLoop (Idle);

  delete screen;

  return EXIT_SUCCESS;
}

void     Idle ()
{
  static clock_t last = clock ();
  static count        = 0;

  int x = random (screen->width()),
      y = random (screen->height ()),
      w = random (screen->width()-x),
      h = random (screen->height()-y);

  screen->bar (x,y,w,h,random (0xFFFFFFFF));
  screen->refresh ();

  count++;

  if (clock () - last > CLK_TCK)
  {
    screen->fill (x,y,random (0xFFFFFFFF),random (0xFFFFFFFF));
    dout<<"FPS: "<<float (count*CLK_TCK)/float (clock ()-last)<<endl;
    last = clock ();
    count = 0;
  }
}
