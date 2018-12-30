#ifndef __LOG_SYSTEM__
#define __LOG_SYSTEM__

#include <types.h>

////////////////////////////////////////////////////////////////////////////////
///Описание log-system
////////////////////////////////////////////////////////////////////////////////
class   Log
{
  public:
                        Log  (ostream* os = NULL):logout (os) {}
        virtual         ~Log () {}               

                        enum{
                          MESSAGE_COLOR = RGB(255,255,0),
                          WARNING_COLOR = RGB(255,0,255),
                          ERRROR_COLOR  = RGB(255,0,0)
                        };

        ostream&        os   () { return *logout; }
        void            os   (ostream* _os) { logout = _os; }
        operator ostream&    () { return *logout; }

////////////////////////////////////////////////////////////////////////////////
///Работа с цветом
////////////////////////////////////////////////////////////////////////////////        
        virtual  void textcolor (long)  {}

  protected:
        ostream*        logout;
};

extern Log* logOut;

#define  lout (*logOut)

#define  LogMessage(X)  if (logOut){ logOut->textcolor (Log::MESSAGE_COLOR); logOut->os()<<X<<endl; }
#define  LogError(X)    if (logOut){ logOut->textcolor (Log::WARNING_COLOR); logOut->os()<<X<<endl; }
#define  LogWarning(X)  if (logOut){ logOut->textcolor (Log::ERROR_COLOR); logOut->os()<<X<<endl; }

#endif