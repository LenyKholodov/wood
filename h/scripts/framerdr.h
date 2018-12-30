#ifndef __TAG_STREAM_READER__
#define __TAG_STREAM_READER__

#include <memory\memory.h>
#include <misc.h>
#include <iostream.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///�⥭�� ������ �� ��⮪�
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
///������� ������ / ������� ��, ������� ��ப�
/////////////////////////////////////////////////////////////////////////////////////////////
                enum {
                        open_tag   = '{',
                        close_tag  = '}',
                        end_line   = ';',
                        empty      = ' ',

                        max_string_len = 0x10000
                }; 

/////////////////////////////////////////////////////////////////////////////////////////////
///�㭪�� �⥭�� 
/////////////////////////////////////////////////////////////////////////////////////////////
    typedef bool  (*ReadValueFn)(istream& is,void* data);

/////////////////////////////////////////////////////////////////////////////////////////////
///�㭪�� ᮧ����� ��� ���
/////////////////////////////////////////////////////////////////////////////////////////////
    typedef void* (*CreateTagFn)(void*  obj,            //����� (��� create obj=NULL)
                                 FrameReader& rdr,      //���⥫�
                                 void*        self      //�����⥫� �� ������� �����
                                );

/////////////////////////////////////////////////////////////////////////////////////////////
///����⢥��� �⥭��
/////////////////////////////////////////////////////////////////////////////////////////////
    bool        ReadStream   (const char*,ostream& err = dout);

/////////////////////////////////////////////////////////////////////////////////////////////
///��ᮥ������� ��६�����
/////////////////////////////////////////////////////////////////////////////////////////////

    template <class Type> 
    bool        BindValue    (const char* tag,Type& obj) //�⥭�� �������� ������ (������㫨஢�����)
    {
      return bind (tag,StdRead<Type>::ReadValue,&obj);
    }

    template <class Type>
    bool        BindArray    (const char* tag,Type* arr) //�⥭�� ���ᨢ�
    {
      return bind (tag,StdRead<Type>::ReadValue,arr,sizeof (Type));
    }

    bool        BindString   (const char* tag,char* string);  //�⥭�� ��ப�

    bool        BindReader   (const char* tag,void* readerFn,void* self = NULL); //�㭪�� �⥭�� ���짮��⥫�

/////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������� �३��
/////////////////////////////////////////////////////////////////////////////////////////////
    bool        DeclareFrame (const char*);

/////////////////////////////////////////////////////////////////////////////////////////////
///�⥭�� ���������� ��
/////////////////////////////////////////////////////////////////////////////////////////////
    template <class Type>
    bool        BindCreater  (const char* tag,Type*& ptr,void* createrFn,void* self)
    {
      return bindtag (tag,(void**)&ptr,createrFn,self);
    }

    bool        BindCreater  (const char* tag,void* createrFn,void* self);

/////////////////////////////////////////////////////////////////////////////////////////////
///����� � �३����
/////////////////////////////////////////////////////////////////////////////////////////////
    void*       GetFrameObj        () const; //������ 㪠��⥫� �� ��ꥪ� ⥪�饣� �३��    
    void*       GetParentFrameObj  () const; //������ 㪠��⥫� �� ��ꥪ� த�⥫�᪮�� �३��    

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