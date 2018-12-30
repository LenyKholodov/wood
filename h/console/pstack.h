#ifndef __CONSOLE_PARAM_STACK__
#define __CONSOLE_PARAM_STACK__

#include <pool.h>

const    int    COMMAND_LINE_LEN   = 512;
const    int    COMMAND_MAX_PARAMS = 32;

////////////////////////////////////////////////////////////////////////////////
///Стек параметров
////////////////////////////////////////////////////////////////////////////////
class    ParamStack
{
  public:
                        ParamStack  (const char*);
           virtual      ~ParamStack ();

           void         refresh     (const char*);

////////////////////////////////////////////////////////////////////////////////
///Доступ
////////////////////////////////////////////////////////////////////////////////
           char*        gets        (int) const;
           int          geti        (int) const;
           float        getf        (int) const;

           char*        command     () const;

           int          count       () const { return mCount; }

  protected:
           char*        next        (char*,char**) const;

  private:
        char    mStr [COMMAND_LINE_LEN];
        char*   mPtr [COMMAND_MAX_PARAMS];
        int     mCount;
};

#endif