#ifndef __WINDOW_FULL_SCREEN__
#define __WINDOW_FULL_SCREEN__

#include <window\window.h>
#include <graph\grtypes.h>

////////////////////////////////////////////////////////////////////////////////
///Окно с поддержкой полноэкранного режима
////////////////////////////////////////////////////////////////////////////////
class  FullScreenWindow: public Window
{
  public:
                        FullScreenWindow  (Pool*,
                                           const rect_t&,
                                           const char* _WndName
                                          );
                        FullScreenWindow  (Window*,
                                           const rect_t&,
                                           const char* _WndName
                                          );
           virtual      ~FullScreenWindow ();

  protected:
           void         Init              (const rect_t&);

  private:
           char         mClassName [CLASS_NAME_LEN];     
};

#endif