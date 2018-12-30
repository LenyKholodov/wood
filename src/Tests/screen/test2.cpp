#include <window.h>
#include <screen.h>
#include <time.h>
#include <misc.h>
#include <debug.h>

void     Idle ();

memScreen*  screen    = NULL;
image_t*    image [5] = {NULL};
sprite_t*   sprite    = NULL;

int      main (int,char* [])
{
  Window* wnd = new SimpleWindow (rect_t (800,600),"Test Windows");
  screen      = Screen::create   (wnd);

  image [0]   = image_t::create  ("back.jpg");
  image [1]   = image_t::create  ("atoll.bmp");
  image [2]   = image_t::create  ("test1.bmp");
  image [3]   = image_t::create  ("test2.bmp");
  sprite      = sprite_t::create (image [3],0x00FFFFFF);
  sprite->save ("sprite.spr");
  delete sprite;
  sprite      = sprite_t::create ("sprite.spr");

  wnd->ShowCursor (TRUE);

  MainLoop (Idle);

  for (int i=0;i<5;i++)
    if (image [i])
      delete image [i];

  delete screen;

  return EXIT_SUCCESS;
}

void     Idle ()
{
  static clock_t last = clock ();
  static count        = 0;

  int x = random (screen->width()) - (screen->width()>>1),
      y = random (screen->height ()) - (screen->height()>>1);  

  screen->PutSprite (x,y,sprite);
  screen->refresh   ();

  count++;

  if (clock () - last > CLK_TCK)
  {
    dout<<"FPS: "<<float (count*CLK_TCK)/float (clock ()-last)<<endl;
    last = clock ();
    count = 0;
  }
} 


//void     Idle ()
//{
//  screen->PutSprite (0,0,sprite);
//  screen->refresh   ();
//} 
