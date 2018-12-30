#ifndef __KERNEL_DEBUG_SERVICE__
#define __KERNEL_DEBUG_SERVICE__

#include <pool.h>
#include <kernel\service.h>
#include <debug\defs.h>
#include <misc.h>

class Flag;
class Var;

/////////////////////////////////////////////////////////////////////////////////////////////
///Служба отладки
/////////////////////////////////////////////////////////////////////////////////////////////
class   DebugService: public Service
{
  friend class  Flag;
  friend class  Var;

  public: 
                         DebugService  (const char* map_file,Pool* pool);
             virtual     ~DebugService ();

                         enum {
                           DBG_POOL_SIZE = 40*1024
                         };

    virtual   const char*       ServiceName () const { return "Debug service"; }

/////////////////////////////////////////////////////////////////////////////////////////////
///Печать инфрормации
/////////////////////////////////////////////////////////////////////////////////////////////
              void       PrintCrash (void* crash_addr,ostream&) const;
    static    void       PrintCrash (void* crash_addr,const char* map,ostream&);
              void       PrintVars  (ostream&)                  const;
              void       PrintFlags (ostream&)                  const;
              uint       Dump       (void*,ostream& = dout)     const;
    static    uint       Dump       (void*,const char* map_file,ostream& = dout);

/////////////////////////////////////////////////////////////////////////////////////////////
///Поток отладки
/////////////////////////////////////////////////////////////////////////////////////////////
                         Service::dbg;
              
/////////////////////////////////////////////////////////////////////////////////////////////
///Обработка исключений
/////////////////////////////////////////////////////////////////////////////////////////////
//    static   bool        FindLine   (const void* crash_addr,
//                                     char*       file_name,
//                                     char*       func_name,
//                                     uint&       line,
//                                     uint&       bytes_offs_from_start_of_line,
//                                     const  char* map = NULL
//                                    );

/////////////////////////////////////////////////////////////////////////////////////////////
///Присоединение map файла
/////////////////////////////////////////////////////////////////////////////////////////////
             void         BindMap   (const char*);

  private:
       static char*           GetExceptionMessage (DWORD,char*);
       static LONG  __stdcall Unhandled           (void*);

  private:     
              FloatPool   mTempPool;
              char        mMapFile [128];
              Ring<Var*>  mVars;
              Ring<Flag*> mFlags;
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Отслеживание переменной
/////////////////////////////////////////////////////////////////////////////////////////////
class   Var
{
  friend class DebugService;
  protected:
                        Var ();
                        Var (const char*,void*);
                        Var (const Var&);
  public:

           virtual      ~Var();

           operator = (const Var& var);

           void*        GetPtr() const;

           virtual   void        print (ostream&) const = 0;
           virtual   void        input (istream&) const = 0; 

    friend ostream& operator << (ostream& os,const Var& v) { v.print (os); return os; }
    friend istream& operator >> (istream& is,Var& v)       { v.input (is); return is; }

  private:
    char           name [32];
    void*          ptr;
    DebugService*  owner;
};

template <class Type>
class   SearchVar: public Var
{
  public:
                  SearchVar(char*,Type*);

    Type*  operator ->    () const { return search; }
           operator Type* () const { return search; }

   virtual  void        print(ostream&) const;
   virtual  void        input(istream&) const;
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Флаг входа
/////////////////////////////////////////////////////////////////////////////////////////////
class   Flag
{
  friend class DebugService;

  public:
           Flag          ();
           Flag          (const char* flag_name,bool state=false);
           Flag          (const Flag& flag);
           operator =    (const Flag& flag);
           operator =    (const bool state);           
           ~Flag         ();

           operator bool () const;

           friend ostream& operator<< (ostream& os,const Flag& f) { return os<<f.name<<' '<<f.flag; }

  private:
           char           name [128];
           bool           flag;   
           DebugService*  owner;
};
      

template <class Type> inline void SearchVar<Type>::print (ostream& os) const
{
  os<<*(Type*)GetPtr();
}

template <class Type> inline void SearchVar<Type>::input (istream& is) const
{
//  is>>*(Type*)GetPtr();
}

template <class Type>
SearchVar<Type>::SearchVar (char* name,Type* ptr)
                     : Var (name,ptr)
{       }

#endif          