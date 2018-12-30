#ifndef __CONSOLE_HISTORY__
#define __CONSOLE_HISTORY__

#include <console\pstack.h>

////////////////////////////////////////////////////////////////////////////////
///История команд
////////////////////////////////////////////////////////////////////////////////
class ConsoleHistory: public MemObject
{
  struct hisnode_t;
  public:
                        ConsoleHistory  (Pool*,int num);  
       virtual          ~ConsoleHistory ();

        void             remember        (const char*);
        const char*      up              ();
        const char*      down            ();

  private:
              void       first           (hisnode_t*);  

  private:
        struct hisnode_t
        {
          char          command [COMMAND_LINE_LEN];
          ulong         crc;
          hisnode_t     *prev, *next;
        };

        hisnode_t*      mNode;
        hisnode_t*      mFirst, *mLast;
        hisnode_t*      mPos;
};

#endif