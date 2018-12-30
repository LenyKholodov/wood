#ifndef __GAME_SYSTEM_SERVER__
#define __GAME_SYSTEM_SERVER__

#include <engine\stream.h>
#include <memory\memory.h>
#include <misc\dict.h>
#include <misc\ring.h>
#include <memory\pool.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///����饭�� ����騥 �ࢥ�� (��⮪�����)
/////////////////////////////////////////////////////////////////////////////////////////////
enum ServerMsg 
{
  MSG_TICK = MSG_GLOBAL_SERVER,
  MSG_PAUSE,
  MSG_STOP,
  MSG_RESUME,
  MSG_RESTART  
};

class    Server;

/////////////////////////////////////////////////////////////////////////////////////////////
///���������� � �ࢥ஬ (䠪��᪨ ��)
/////////////////////////////////////////////////////////////////////////////////////////////
class    Connection
{
  friend class Server;
  public:
                Connection  (Pool* = M_GetPool ());
   virtual      ~Connection ();

/////////////////////////////////////////////////////////////////////////////////////////////
///��⮪� ����� - �뢮��
/////////////////////////////////////////////////////////////////////////////////////////////
    MessageStream&    ws () { return *mWrite; } //��⮪ � ����� ���� ������
    MessageStream&    rs () { return *mRead;  } //��⮪ �� ���ண� �⠥� ������

    bool    BindWriteStream (MessageStream&);
    bool    BindReadStream  (MessageStream&);

/////////////////////////////////////////////////////////////////////////////////////////////
///����� � ᮮ�饭�ﬨ
/////////////////////////////////////////////////////////////////////////////////////////////
    bool    BindMessage   (msg_t from,msg_t to = -1);
    void    UnbindMessage (msg_t);

  private:
    void    BindServer   (Server&);
    void    UnbindServer ();

    msg_t   ReindexMsg   (msg_t) const; //ret: -1 - not find

    static  void ConnectionHandler (MessageStream::Action,Connection&);

  private:
    typedef Dict<msg_t,msg_t>   MsgMap;
              
  private:
        MsgMap          mMsgMap;
        MessageStream   *mWrite, *mRead;
        Server*         mServer;
};

/////////////////////////////////////////////////////////////////////////////////////////////
///������㫨஢���� �ᨭ�஭�� ����� �ࠢ����� ��஢� ����ᮬ
/////////////////////////////////////////////////////////////////////////////////////////////
class    Server: public MemObject
{
  friend class ServerThread;
  friend class Connection;
  public:
                        Server  (Pool* = Memory::M_GetPool ());
            virtual     ~Server ();

/////////////////////////////////////////////////////////////////////////////////////////////
///��ᯥ��� ᮮ�饭��
/////////////////////////////////////////////////////////////////////////////////////////////
//    class MsgHandler;    

/////////////////////////////////////////////////////////////////////////////////////////////
///�᭮��� �������
/////////////////////////////////////////////////////////////////////////////////////////////
//                void    pause    ();
//                void    resume   ();
//                void    restart  ();

//                void    save     ();
//                bool    load     ();

/////////////////////////////////////////////////////////////////////////////////////////////
///���⥬� ��।������ �६��� + Ticker
/////////////////////////////////////////////////////////////////////////////////////////////
//    void        AddTickClient   (); 

/////////////////////////////////////////////////////////////////////////////////////////////
///�뭥ᥭ��� ��ࠡ�⪠ �訡�� �ࢥ� (ᮮ�饭�� �����⠬ �� �訡��� �ࢥ�)
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
///����� � �������묨 �ਯ⠬�
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
///���।������ ����� ����� �ࢥ�
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
///����� � ����ᠬ�
/////////////////////////////////////////////////////////////////////////////////////////////
//1) ��।������, �����
//2) ��࠭����, ����⠭�������

/////////////////////////////////////////////////////////////////////////////////////////////
///����� � ������묨 ��� �ࢥ� (����᪨�� ��� ��� �������� �����⮢) ��६���묨 //???
/////////////////////////////////////////////////////////////////////////////////////////////
//        typedef ServerStaticVar static_t;

/////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����᪮�� ᮥ������� � ��㣨� ��஢� �ࢥ஬
/////////////////////////////////////////////////////////////////////////////////////////////
//ᮧ�����, 㭨��஢����
//��⨢��� / ���ᨢ���
            bool        AddConnection (Connection&);
            void        DelConnection (Connection&);

  protected:
/////////////////////////////////////////////////////////////////////////////////////////////
///��஢� ᮮ�饭��  
/////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool        ProcessMessage (msg_t,MessageStream&,MessageStream&);

  private:
            void ConnectionHandler (MessageStream::Action,Connection&);
            int  ProcessThread     ();

  private:
        Thread*            mThread;

        Ring<Connection*>  mConnections;

        Ring<Connection*>  mMsgList;
        CritSect           mMsgTrans;
        uint               mWaitMsgNum;
        Thread*            mWaitThread;

        bool               mSleep;
};

#endif