#include <pch.h>
#include <stdarg.h>

Window*         IF_CreateWindow (int type,Pool* pool,...)
{
  D_Try
  {
    va_list list;
    Window* wnd = NULL;

    va_start (list,pool);

    switch (type)
    {
      case SIMPLE_WINDOW:
      {
        rect_t&     r  = va_arg (list,rect_t);
        const char* s  = va_arg (list,const char*);

        wnd = new (pool) SimpleWindow (pool,r,s);

        break;
      }
      case SIMPLE_WINDOW_CHILD:
      {
        Window*     w = va_arg (list,Window*);
        rect_t&     r = va_arg (list,rect_t);
        const char* s = va_arg (list,const char*);

        wnd = new (pool) SimpleWindow (w,r,s);

        break;
      }
      case FULLSCREEN_WINDOW:
      {
        rect_t&     r  = va_arg (list,rect_t);
        const char* s  = va_arg (list,const char*);

        wnd = new (pool) FullScreenWindow (pool,r,s);

        break;
      }
      case FULLSCREEN_WINDOW_CHILD:
      {
        Window*     w = va_arg (list,Window*);
        rect_t&     r = va_arg (list,rect_t);
        const char* s = va_arg (list,const char*);

        wnd = new (pool) FullScreenWindow (w,r,s);

        break;
      }
      case POPUP_WINDOW:
      {
        rect_t&     r  = va_arg (list,rect_t);
        const char* s  = va_arg (list,const char*);

        wnd = new (pool)  PopUpWindow (pool,r,s);

        break;
      }
      case POPUP_WINDOW_CHILD:
      {
        Window*     w = va_arg (list,Window*);
        rect_t&     r = va_arg (list,rect_t);
        const char* s = va_arg (list,const char*);

        wnd = new (pool) PopUpWindow (w,r,s);

        break;
      }
      default:
        wnd = NULL;
    }

    va_end   (list);

    return wnd;
  }
  D_ExcRet ("Error at IF_CreateWindow",NULL);
}

void            IF_CloseWindow  (Window* wnd)
{
  D_Try
  {
    delete wnd;
  }
  D_Exc ("Error at IF_CloseWindow");
}
