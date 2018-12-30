#ifndef __TAG_STREAM_READER__
#define __TAG_STREAM_READER__

#include <memory\memory.h>
#include <misc.h>
#include <iostream.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///Чтение данных из потока
/////////////////////////////////////////////////////////////////////////////////////////////
class    FrameReader: public MemObject
{
  class  Reader;
  class  Frame;
  friend class iobuffer;
  friend class Frame;
  public:
                FrameReader  (Pool* = Memory::M_GetPool ());
    virtual     ~FrameReader ();

/////////////////////////////////////////////////////////////////////////////////////////////
///Символы открытия / закрытия тэга, закрытия строки
/////////////////////////////////////////////////////////////////////////////////////////////
                enum {
                        open_tag   = '{',
                        close_tag  = '}',
                        end_line   = ';',
                        empty      = ' ',

                        max_string_len = 0x10000
                }; 

/////////////////////////////////////////////////////////////////////////////////////////////
///Функция чтения 
/////////////////////////////////////////////////////////////////////////////////////////////
    typedef bool  (*ReadValueFn)(istream& is,void* data);

/////////////////////////////////////////////////////////////////////////////////////////////
///Функция создания тэгов тэгов
/////////////////////////////////////////////////////////////////////////////////////////////
    typedef void* (*CreateTagFn)(void*  obj,            //Данные (для create obj=NULL)
                                 FrameReader& rdr,      //Читатель
                                 void*        self      //Указатель на заказные данные
                                );

/////////////////////////////////////////////////////////////////////////////////////////////
///Собственно чтение
/////////////////////////////////////////////////////////////////////////////////////////////
    bool        ReadStream   (const char*,ostream& err = dout);

/////////////////////////////////////////////////////////////////////////////////////////////
///Присоединение переменных
/////////////////////////////////////////////////////////////////////////////////////////////

    template <class Type> 
    bool        BindValue    (const char* tag,Type& obj) //Чтение одиночных данных (инкапсулированных)
    {
      return bind (tag,StdRead<Type>::ReadValue,&obj);
    }

    template <class Type>
    bool        BindArray    (const char* tag,Type* arr) //Чтение массива
    {
      return bind (tag,StdRead<Type>::ReadValue,arr,sizeof (Type));
    }

    bool        BindString   (const char* tag,char* string);  //Чтение строки

    bool        BindReader   (const char* tag,void* readerFn,void* self = NULL); //Функция чтения пользователя

/////////////////////////////////////////////////////////////////////////////////////////////
///Объявление вложеного фрейма
/////////////////////////////////////////////////////////////////////////////////////////////
    bool        DeclareFrame (const char*);

/////////////////////////////////////////////////////////////////////////////////////////////
///Чтение вложенного тэга
/////////////////////////////////////////////////////////////////////////////////////////////
    template <class Type>
    bool        BindCreater  (const char* tag,Type*& ptr,void* createrFn,void* self)
    {
      return bindtag (tag,(void**)&ptr,createrFn,self);
    }

    bool        BindCreater  (const char* tag,void* createrFn,void* self);

/////////////////////////////////////////////////////////////////////////////////////////////
///Работа с фреймами
/////////////////////////////////////////////////////////////////////////////////////////////
    void*       GetFrameObj        () const; //вернуть указатель на объект текущего фрейма    
    void*       GetParentFrameObj  () const; //вернуть указатель на объект родительского фрейма    

  private:
    bool        bind         (const char* tag,void* fn,void* self,size_t = 0);
    bool        bindtag      (const char* tag,void** ptr,void* fn,void* self,size_t = 0);

    FrameReader* find        (const char* src,const char*& dest,bool full = true);

    static bool ReadString   (istream&,char*);

    template <class Type>
    struct   StdRead {
      static bool ReadValue    (istream& is,Type& obj)
      {
        is>>obj; 
        return is.good ();
      }
    };

    bool        ReadStream   (Reader&);

  private:
    Frame*  mRoot, *mCurFrame;
};

extern bool ReadString (istream&,char*);

#ifdef DEBUG
  #pragma comment (lib,"scriptsd.lib")
#else
  #pragma comment (lib,"scripts.lib")
#endif

#endif