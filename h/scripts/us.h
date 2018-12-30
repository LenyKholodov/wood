#ifndef __SCRIPTS_STREAM_READER__
#define __SCRIPTS_STREAM_READER__

#include <pool.h>

class   USStreamReader;

enum PrepareModes {
  US_PRECALL,
  US_POSTCALL
};

typedef void (*USCallFn)();
typedef void (*USPrepareFn)(USStreamReader*,uint mode,DWORD = 0);

class   USReader: public MemObject
{
  public:
                USReader  (Pool* pool):MemObject (pool) {}
        virtual ~USReader () {}
        virtual void read (istream&) = 0;
};

template <class Type>
class   USObjReader: public USReader
{
  public:
                USObjReader   (Pool* pool,Type& obj):USReader (pool),mObj (&obj) {}
        virtual void read      (istream&);

  protected:
        Type*         mObj; 
};

template <class Type>
class   USRingReader: public USReader
{
  public:
                USRingReader   (Pool* pool,RingTmpl<Type>& ring):USReader (pool),mRing (&ring) {}
        virtual void read      (istream&);

  private:
        RingTmpl<Type>*         mRing;        
};

typedef char* string_t;

class   USStringReader: public USObjReader<char>
{
  public:
                USStringReader (Pool* pool,char* x):USObjReader<char> (pool,*x){ mObj = x; }
        virtual void read      (istream&);
};

////////////////////////////////////////////////////////////////////////////////
///Обслуживание файлов инициализации утилит
////////////////////////////////////////////////////////////////////////////////
class   USStreamReader: public MemObject
{
  public:
                USStreamReader (Pool*,ostream&);
      virtual   ~USStreamReader ();

////////////////////////////////////////////////////////////////////////////////
///Присоединение к строковым тэгам переменных
////////////////////////////////////////////////////////////////////////////////
      void      BindCallFn      (const char*,USCallFn);
      template  <class Type>
      void      BindRing        (const char* tag,RingTmpl<Type>& ring)      {
        _bind (tag,new (pool()) USRingReader<Type> (pool(),ring));
      }
      template  <class Type>
      void      Bind            (const char* tag,Type& obj)      {
        USObjReader<Type>* rdr = ext_new USObjReader<Type> (GetPool(),obj);
        _bind (tag,rdr);
      }
      void      BindString      (const char* tag,char* str) {
        USStringReader* rdr = ext_new USStringReader (GetPool (),str);
        _bind (tag,rdr);
      }

      void      BindReader      (const char*,
                                 USStreamReader*,
                                 USPrepareFn = NULL,
                                 DWORD       = 0
                                );

////////////////////////////////////////////////////////////////////////////////
///Чтение потока
////////////////////////////////////////////////////////////////////////////////
      void      read            (istream&);
      void      read            (const char*);

  protected:
      virtual void      _read           (istream&);
      void      _bind           (const char*,USReader*);

  private:
        struct tag_t
        {
          USStreamReader* stream;
          USPrepareFn     fn;
          DWORD           param;
        };

        typedef Dict<USReader*> PtrDict;
        typedef Dict<USCallFn>  FnDict;
        typedef Dict<tag_t>     TagDict;

        istream*        mStream;
        ostream*        mOs;
        PtrDict         mPtrDict;
        FnDict          mFnDict;
        TagDict         mTagDict;
        BOOL            mCreateFlag;
};

template <class Type> void USRingReader<Type>::read (istream& is)
{
  static char buf [1024];

  is>>buf;

  if (strcmp (buf,"["))
    return;

  for (;;)
  {  
    streampos pos = is.tellg ();
    is>>buf;

    if (strcmp (buf,"]"))
    {
      is.seekg (pos);

      Type obj; is>>obj;
      mRing->insert (obj);
    }
    else
      break;
  }
} 

template <class Type> void USObjReader<Type>::read (istream& is)
{
  is>>*mObj;
}

#ifndef WOOD_LIB_COMPILE
  #ifdef DEBUG
    #pragma comment (lib,"scriptsd.lib")
  #else
    #pragma comment (lib,"scripts.lib")
  #endif
#endif

#endif