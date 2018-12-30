#ifndef __COMMAND_VAR_POOL__
#define __COMMAND_VAR_POOL__

#include <pool.h>
#include <console\pstack.h>
#include <console\strpool.h>

class    VarPool;

const    int    VAR_NAME_LEN            = 32;
const    size_t VP_STRING_POOL_SIZE     = 16384;

////////////////////////////////////////////////////////////////////////////////
///Прототип функции
////////////////////////////////////////////////////////////////////////////////
typedef  void (*CommandFn)(VarPool&,uint access,ParamStack&,ostream&,DWORD);

struct   comvar_t
{
  union var_t {
    int       i;        //Целое
    float     f;        //Дробное
    char*     chptr;    //Указатель на строку
    CommandFn fn;       //Функция
    int*      iptr;     //Указатель на целое
    float*    fptr;     //Указатель на дробное
  }var, defvar;

  int     attr;                 //Атрибуты  
  int     type;                 //Тип
  DWORD   param, defParam;      //Для функций
  BOOL    use;                  //Флаг использования
  char    name [VAR_NAME_LEN];  //Имя
};

////////////////////////////////////////////////////////////////////////////////
///Пул переменных и функций
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
///Переменные
////////////////////////////////////////////////////////////////////////////////

////////////////////Создание либо изменение переменной//////////////////////////

        BOOL    set     (const char*,int,int prior = AC_FULL,int attr = 0);
        BOOL    set     (const char*,float,int prior = AC_FULL,int attr = 0);
        BOOL    set     (const char*,char*,int prior = AC_FULL,int attr = 0);
        BOOL    set     (const char*,
                         CommandFn,
                         DWORD param = 0,
                         int prior = AC_FULL,
                         int attr = 0
                        );

/////////////////////////Присоединение переменной///////////////////////////////

        BOOL    link    (const char*,int*,int prior = AC_FULL,int attr = 0);
        BOOL    link    (const char*,char*,int prior = AC_FULL,int attr = 0);
        BOOL    link    (const char*,float*,int prior = AC_FULL,int attr = 0);

/////////////////////////////////Восстановление default'а///////////////////////

        BOOL    restore (const char*,int prior = AC_FULL); 

/////////////////////////////////Создание макроса///////////////////////////////

        BOOL    alias   (const char* src,const char* dest);

/////////////////////////////////Удаление///////////////////////////////////////

        BOOL    kill        (const char*,int prior = AC_FULL);
        BOOL    killmacros  (const char*);

////////////////////////////Доступ к переменной/////////////////////////////////

        int     geti    (const char*,int prior = AC_FULL) const;
  const char*   gets    (const char*,int prior = AC_FULL) const; 
        float   getf    (const char*,int prior = AC_FULL) const;
    CommandFn   getfunc (const char*,DWORD&,int prior = AC_FULL) const;

//////////////////////////////Доступ к default'у////////////////////////////////

        int     getdefi    (const char*,int prior = AC_FULL) const;
  const char*   getdefs    (const char*,int prior = AC_FULL) const; 
        float   getdeff    (const char*,int prior = AC_FULL) const;

////////////////////////////////////////////////////////////////////////////////
///Доступ
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