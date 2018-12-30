#ifndef __CONSOLE_COMMAND__
#define __CONSOLE_COMMAND__

#include <console\vpool.h>

////////////////////////////////////////////////////////////////////////////////
///�������� ��������
////////////////////////////////////////////////////////////////////////////////
class    Command: public MemObject
{
  public:
                        Command  (Pool*,int _VarNum);
        virtual         ~Command ();

  public:
        VarPool         vpool;

////////////////////////////////////////////////////////////////////////////////
///�ࠢ� ����㯠
////////////////////////////////////////////////////////////////////////////////
        uint            access          () const { return mAccess; }
        void            access          (uint x) { mAccess = x; }

////////////////////////////////////////////////////////////////////////////////
///��⮪ �뢮��
////////////////////////////////////////////////////////////////////////////////
        void            bind            (ostream& os) { mOs = &os; }

////////////////////////////////////////////////////////////////////////////////
///����㧪�/��࠭���� ���䨣��樨
////////////////////////////////////////////////////////////////////////////////
        void            LoadConfig      (const char*);
        void            SaveConfig      (const char*);

////////////////////////////////////////////////////////////////////////////////
///�믮������ �������
////////////////////////////////////////////////////////////////////////////////
        void            command         (const char*);
                        operator ()     (const char* x) { command (x); }

////////////////////////////////////////////////////////////////////////////////
///�������
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

