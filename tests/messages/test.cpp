#include <iostream.h>

template <const char* MessageName> 
class    Message
{
  public:
                Message (): msg (current++) {}

    const char* name    () const { return MessageName; }

  private:
            int     msg;      //������
    static  int     current;  //⥪�騩 ������ ��।������
};