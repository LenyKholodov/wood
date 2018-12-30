#ifndef __ENGINE_MESSAGE_STREAM__
#define __ENGINE_MESSAGE_STREAM__

#include <engine\message.h>
#include <kernel\critsect.h>
#include <misc\flags.h>
#include <string.h>

class Thread;

////////////////////////////////////////////////////////////////////////////////////////////
///��⮪ - ��� ��஢�� ᮮ�饭��
////////////////////////////////////////////////////////////////////////////////////////////
class   MessageStream: public Flags32
{
  public:
                        MessageStream  ();
         virtual        ~MessageStream ();

                        enum States {
                          ST_FULL_WRITE  = 1,    //�������� ᮮ�饭�� � ��砥 ��� �������� �����
                          ST_WRITE_WAIT  = 2,    //�������� �᢮�������� ���� �� ������
                          ST_READ_WAIT   = 4,    //�������� ������ ᮮ�饭�� �� �⥭��
                          ST_READ        = 8,    //����� ����
                          ST_WRITE       = 16,   //����� �����
                          ST_WRITE_ERROR = 32,   //�᫨ ����� � १���� ����� �뫨 ���०����
                          ST_READ_ERROR  = 64,   //�᫨ ����� � १���� �⥭�� �뫨 ���०����
                          ST_WRITE_TOO_LARGE = 128, //�訡�� - �ॢ�襭�� ࠧ��� ����
                          ST_READ_TOO_LARGE = 256
                        };

                        enum Action {
                          ACT_LOCK_WRITE = 0,
                          ACT_UNLOCK_WRITE,
                          ACT_LOCK_READ,
                          ACT_UNLOCK_READ 
                        };

    typedef void (*ActionHandler)(Action,void* self);

////////////////////////////////////////////////////////////////////////////////////////////
///����祭�� ����㯠 �� ������ � ��⮪
////////////////////////////////////////////////////////////////////////////////////////////
                bool     LockWrite      (msg_t = -1);
                void     UnlockWrite    ();

////////////////////////////////////////////////////////////////////////////////////////////
///������
////////////////////////////////////////////////////////////////////////////////////////////
                size_t   write           (const void*,size_t); 
                size_t   PutMessage      (msg_t,const void*,size_t);
                
                template <class T>
      inline    bool     PutMessage      (msg_t msg,const T& data) { return PutMessage (msg,&data,sizeof (T)) == sizeof (T); }

////////////////////////////////////////////////////////////////////////////////////////////
///����஢���� ��ப� � ��⮪
////////////////////////////////////////////////////////////////////////////////////////////
                bool     puts            (const char*);

////////////////////////////////////////////////////////////////////////////////////////////
///����祭�� ����㯠 �� �⥭�� �� ��⮪�
////////////////////////////////////////////////////////////////////////////////////////////
                bool     LockRead        (msg_t& msg = mDefTemp);
                void     UnlockRead      ();
        
                size_t   read            (void*,size_t);
                size_t   GetMessage      (msg_t&,void*,size_t);

                template <class T>
      inline    bool     GetMessage      (msg_t& msg,T& data)   { return GetMessage (msg,&data,sizeof (T)) == sizeof (T); }

////////////////////////////////////////////////////////////////////////////////////////////
///�����祭�� ��ப� �� ��⮪�
////////////////////////////////////////////////////////////////////////////////////////////
                size_t   gets            (char* s); //�����頥� ࠧ��� ���⠭��� ��ப� ��� '/0'

////////////////////////////////////////////////////////////////////////////////////////////
///���
////////////////////////////////////////////////////////////////////////////////////////////
      inline    uint     count           () const { return mMsgCount; }

      inline    bool     IsReadLock      () const { return mRead;  }
      inline    bool     IsWriteLock     () const { return mWrite; }

      virtual   bool     valid           () const { return true; } //�஢�ઠ �� ���४⭮���

/////////////////////////////////////////////////////////////////////////////////////////////
///��ᮥ������� ���墠�稪� �襭��
/////////////////////////////////////////////////////////////////////////////////////////////
                void     BindAction      (Action,ActionHandler = NULL,void* data = NULL);

  protected:
////////////////////////////////////////////////////////////////////////////////////////////
///�࣠������ ����権 �⥭�� / �����
////////////////////////////////////////////////////////////////////////////////////////////
      virtual   bool     _LockWrite      (msg_t)  = 0; 
      virtual   size_t   _write          (const void*,size_t) = 0;
      virtual   bool     _UnlockWrite    () = 0; 

      virtual   bool     _LockRead       (msg_t&) = 0;
      virtual   size_t   _read           (void*,size_t) = 0;
      virtual   size_t   _gets           (char*,bool& end);
      virtual   bool     _UnlockRead     () = 0;

  private:
////////////////////////////////////////////////////////////////////////////////////////////
///��������
////////////////////////////////////////////////////////////////////////////////////////////
                void    WaitWrite        ();
                void    ResumeWrite      ();

                void    WaitRead         ();
                void    ResumeRead       ();

  private:
      CritSect  mCSWrite, mCSRead, mTrans;
      bool      mRead, mWrite;
      uint      mMsgCount;
      Thread    *mWaitWrite, *mWaitRead;

      struct    action_t {
        ActionHandler    fn;
        void*            data;

                         action_t (): data (NULL), fn (NULL) {}
      }mActions [4];

      static msg_t mDefTemp;
};

inline MessageStream& operator << (MessageStream& os,const char* x)
{
  os.puts (x);

  return os;
}

inline MessageStream& operator << (MessageStream& os,const uchar* x)
{
  os.puts ((const char*)x);

  return os;
}

template <typename T>
inline  MessageStream& operator << (MessageStream& os,const T& x)
{
  os.write (&x,sizeof (T));

  return os;
}


inline MessageStream& operator >> (MessageStream& is,char* x)
{
  is.gets (x);
  return is;
}

inline MessageStream& operator >> (MessageStream& is,uchar* x)
{
  is.gets ((char*)x);
  return is;
}

template <typename T>
inline  MessageStream& operator >> (MessageStream& is,T& x)
{
  is.read (&x,sizeof (T));

  return is;
}

inline bool IsValid (MessageStream& s) { return &s && s.valid (); }
inline bool IsValid (MessageStream* s) { return s && s->valid (); }

#endif