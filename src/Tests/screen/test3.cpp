#include <window.h>
#include <screen.h>
#include <time.h>
#include <misc.h>
#include <debug.h>

void     Idle ();

memScreen*   screen    = NULL;
image_t*     image [5] = {NULL};
imagelist_t* il        = NULL;

int      main (int,char* [])
{
  Window* wnd = new SimpleWindow (rect_t (800,600),"Test Windows");
  screen      = Screen::create   (wnd);

  image [0]   = image_t::create  ("back.jpg");
  image [1]   = image_t::create  ("atoll.bmp");
  image [2]   = image_t::create  ("test1.bmp");
  image [3]   = image_t::create  ("test2.bmp");

  il          = imagelist_t::create (4,image);
  il->save ("ilist.il");
  delete il;
  il          = imagelist_t::create ("ilist.il");

  wnd->ShowCursor (TRUE);

  MainLoop (Idle);

  for (int i=0;i<5;i++)
    if (image [i])
      delete image [i];

  delete screen;
  delete il;

  return EXIT_SUCCESS;
}

void     Idle ()
{
  screen->PutImage (100,100,il->GetImage (1));  
  screen->refresh  ();
} 
