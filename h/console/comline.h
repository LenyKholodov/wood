#ifndef __CONSOLE_COMMAND_LINE__
#define __CONSOLE_COMMAND_LINE__

#include <pool.h>
#include <console\history.h>
#include <time.h>

class    ConsoleBase;
class    Command;

/////////////////////////////////////////////////////////////////////////////////////////////
///Консольная коммандная строка
/////////////////////////////////////////////////////////////////////////////////////////////
class    CommandLine: public MemObject
{
  public:
                        CommandLine  (Pool*,
                                      ConsoleBase&,
                                      Command&,
                                      int length,
                                      int depth //history
                                     );
            virtual     ~CommandLine ();

            int         length       () const { return mLen; }

////////////////////////////////////////////////////////////////////////////////
///Отправление символа на консоль
////////////////////////////////////////////////////////////////////////////////
            void        put          (char);
            char*       gets         ();

////////////////////////////////////////////////////////////////////////////////
///Курсор
////////////////////////////////////////////////////////////////////////////////
            int         pos          () const { return mCursorPos; } //позиция курсора
            BOOL        cursor       ();

            void        prev         ();  //simbol
            void        next         ();  //simbol
            void        home         ();
            void        end          ();
            void        del          ();

            void        blink        (clock_t x) { mBlink = x; }

////////////////////////////////////////////////////////////////////////////////
///Управление историей команд
////////////////////////////////////////////////////////////////////////////////
            void        prevcmd      ();
            void        nextcmd      ();

  private:
        ConsoleHistory  mHistory;
        ConsoleBase&    mConBase;
        Command&        mCommand;
        char*           mBuf;
        int             mLen, mPos, mCursorPos;

        clock_t         mBlink, mLast;
        BOOL            mCursor;
};

#endif