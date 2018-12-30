#ifndef __WINDOW_POP_UP__
#define __WINDOW_POP_UP__

#include <window\window.h>

////////////////////////////////////////////////////////////////////////////////
///PopUp
////////////////////////////////////////////////////////////////////////////////
class  PopUpWindow: public Window
{
  public:
                        PopUpWindow  (Pool*,
                                       const rect_t&,
                                       const char* _WndName
                                     );
                        PopUpWindow  (Window*,
                                       const rect_t&,
                                       const char* _WndName
                                      );
           virtual      ~PopUpWindow ();

  protected:
           void         Init              (const rect_t&);

  private:
           char         mClassName [CLASS_NAME_LEN];     
};

#endif
