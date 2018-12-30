#ifndef __WINDOW_SIMPLE__
#define __WINDOW_SIMPLE__

#include <window\window.h>

////////////////////////////////////////////////////////////////////////////////
///Обычное окно
////////////////////////////////////////////////////////////////////////////////
class  SimpleWindow: public Window
{
  public:
                        SimpleWindow  (Pool*,
                                       const rect_t&,
                                       const char* _WndName
                                       );
                        SimpleWindow  (Window*,
                                       const rect_t&,
                                       const char* _WndName
                                      );
           virtual      ~SimpleWindow ();

  protected:
           void         Init              (const rect_t&);

  private:
           char         mClassName [CLASS_NAME_LEN];     
};

#endif
