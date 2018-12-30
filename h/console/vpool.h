#ifndef __COMMAND_VAR_POOL__
#define __COMMAND_VAR_POOL__

#include <pool.h>
#include <console\pstack.h>
#include <console\strpool.h>

class    VarPool;

const    int    VAR_NAME_LEN            = 32;
const    size_t VP_STRING_POOL_SIZE     = 16384;

////////////////////////////////////////////////////////////////////////////////
///���⨯ �㭪樨
////////////////////////////////////////////////////////////////////////////////
typedef  void (*CommandFn)(VarPool&,uint access,ParamStack&,ostream&,DWORD);

struct   comvar_t
{
  union var_t {
    int       i;        //�����
    float     f;        //�஡���
    char*     chptr;    //�����⥫� �� ��ப�
    CommandFn fn;       //�㭪��
    int*      iptr;     //�����⥫� �� 楫��
    float*    fptr;     //�����⥫� �� �஡���
  }var, defvar;

  int     attr;                 //��ਡ���  
  int     type;                 //���
  DWORD   param, defParam;      //��� �㭪権
  BOOL    use;                  //���� �ᯮ�짮�����
  char    name [VAR_NAME_LEN];  //���
};

////////////////////////////////////////////////////////////////////////////////
///�� ��६����� � �㭪権
////////////////////////////////////////////////////////////////////////////////
class    VarPool: public MemObject
{
  friend class Command;
  public:
                        VarPool  (Pool*,uint _Num = 128,size_t _StringPoolSize = VP_STRING_POOL_SIZE);
              virtual   ~VarPool ();

                        enum VarAccess {
                          VAR_SYSTEM    = 1,
                          VAR_USER      = 2,
                          VAR_CHEAT     = 4,
                          VAR_READ_ONLY = 8,

                          AC_FULL       = 1|2|4,
                          AC_READ_ONLY  = VAR_READ_ONLY,
                          AC_NO_CHEATS  = VAR_USER|VAR_SYSTEM
                        };

                        enum VarTypes {
                          VAR_INT       = 0,
                          VAR_FLOAT,
                          VAR_FLOAT_PTR,
                          VAR_INT_PTR,
                          VAR_CHPTR,
                          VAR_MACROS,
                          VAR_FUNC,
                          VAR_STRING
                        };

////////////////////////////////////////////////////////////////////////////////
///��६����
////////////////////////////////////////////////////////////////////////////////

////////////////////�������� ���� ��������� ��६�����//////////////////////////

        BOOL    set     (const char*,int,int prior = AC_FULL,int attr = 0);
        BOOL    set     (const char*,float,int prior = AC_FULL,int attr = 0);
        BOOL    set     (const char*,char*,int prior = AC_FULL,int attr = 0);
        BOOL    set     (const char*,
                         CommandFn,
                         DWORD param = 0,
                         int prior = AC_FULL,
                         int attr = 0
                        );

/////////////////////////��ᮥ������� ��६�����///////////////////////////////

        BOOL    link    (const char*,int*,int prior = AC_FULL,int attr = 0);
        BOOL    link    (const char*,char*,int prior = AC_FULL,int attr = 0);
        BOOL    link    (const char*,float*,int prior = AC_FULL,int attr = 0);

/////////////////////////////////����⠭������� default'�///////////////////////

        BOOL    restore (const char*,int prior = AC_FULL); 

/////////////////////////////////�������� �����///////////////////////////////

        BOOL    alias   (const char* src,const char* dest);

/////////////////////////////////��������///////////////////////////////////////

        BOOL    kill        (const char*,int prior = AC_FULL);
        BOOL    killmacros  (const char*);

////////////////////////////����� � ��६�����/////////////////////////////////

        int     geti    (const char*,int prior = AC_FULL) const;
  const char*   gets    (const char*,int prior = AC_FULL) const; 
        float   getf    (const char*,int prior = AC_FULL) const;
    CommandFn   getfunc (const char*,DWORD&,int prior = AC_FULL) const;

//////////////////////////////����� � default'�////////////////////////////////

        int     getdefi    (const char*,int prior = AC_FULL) const;
  const char*   getdefs    (const char*,int prior = AC_FULL) const; 
        float   getdeff    (const char*,int prior = AC_FULL) const;

////////////////////////////////////////////////////////////////////////////////
///�����
////////////////////////////////////////////////////////////////////////////////
        int     count   () const { return mCount; } 
        int     pos     () const { return mPos;   }

        int     attr    (const char*) const; //-1 if not exist
        void    attr    (const char*,int prior = AC_FULL,int attr = 0);

        int     type    (const char*) const; //-1 if not exist
        void    type    (const char*,int type,int prior = AC_FULL);

        BOOL    exist   (const char* x) const { return find (x) != NULL; }

        BOOL    IsSystem       (const char* x) const { return attr (x) & VAR_SYSTEM; }
        BOOL    IsCheat        (const char* x) const { return attr (x) & VAR_CHEAT; }
        BOOL    IsUser         (const char* x) const { return attr (x) & VAR_USER; }
        BOOL    IsReadOnly     (const char* x) const { return attr (x) & VAR_READ_ONLY; }

////////////////////////////////////////////////////////////////////////////////
///Dump
////////////////////////////////////////////////////////////////////////////////
        int     helpfind       (char*,ostream&);

  protected:
        comvar_t* find        (const char*) const;
        comvar_t* create      (const char*);        
        BOOL      compatible  (int prior,int attr) const;
        int       strncmp     (const char*,const char*,int,BOOL&);

  private:
        typedef Dict<int> VarDict;

        ClPool<comvar_t>  mVarPool;
        int               mCount, mPos;
        VarDict           mDict;       
        StringPool        mStrPool;
};

#endif