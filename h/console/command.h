#ifndef __CONSOLE_COMMAND__
#define __CONSOLE_COMMAND__

#include <console\vpool.h>

////////////////////////////////////////////////////////////////////////////////
///Командный интерпретатор
////////////////////////////////////////////////////////////////////////////////
class    Command: public MemObject
{
  public:
                        Command  (Pool*,int _VarNum);
        virtual         ~Command ();

  public:
        VarPool         vpool;

////////////////////////////////////////////////////////////////////////////////
///Права доступа
////////////////////////////////////////////////////////////////////////////////
        uint            access          () const { return mAccess; }
        void            access          (uint x) { mAccess = x; }

////////////////////////////////////////////////////////////////////////////////
///Поток вывода
////////////////////////////////////////////////////////////////////////////////
        void            bind            (ostream& os) { mOs = &os; }

////////////////////////////////////////////////////////////////////////////////
///Загрузка/сохранение конфигурации
////////////////////////////////////////////////////////////////////////////////
        void            LoadConfig      (const char*);
        void            SaveConfig      (const char*);

////////////////////////////////////////////////////////////////////////////////
///Выполнение команды
////////////////////////////////////////////////////////////////////////////////
        void            command         (const char*);
                        operator ()     (const char* x) { command (x); }

////////////////////////////////////////////////////////////////////////////////
///Команды
////////////////////////////////////////////////////////////////////////////////
        BOOL            bind            (const char*,CommandFn,DWORD = 0,int attr = 0);
        BOOL            alias           (const char* src,const char* dest);
        void            unbind          (const char*);        

  protected:
        void            changevar       ();

  private:
    ParamStack  mPStack;
    uint        mAccess;        
    ostream*    mOs;
};

void    CmdSet          ( VarPool&, uint, ParamStack&, ostream&, DWORD);
void    CmdAlias        ( VarPool&, uint, ParamStack&, ostream&, DWORD);
void    CmdKill         ( VarPool&, uint, ParamStack&, ostream&, DWORD);
void    CmdKillMacros   ( VarPool&, uint, ParamStack&, ostream&, DWORD);

#endif

