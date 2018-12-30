#include <iostream.h>

template <const char* MessageName> 
class    Message
{
  public:
                Message (): msg (current++) {}

    const char* name    () const { return MessageName; }

  private:
            int     msg;      //индекс
    static  int     current;  //текущий индекс распределения
};