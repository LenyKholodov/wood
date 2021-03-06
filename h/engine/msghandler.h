
/////////////////////////////////////////////////////////////////////////////////////////////
///��ࠡ��稪 ᮮ�饭��
/////////////////////////////////////////////////////////////////////////////////////////////
class   MessageHandler
{
  friend class MsgDispatch;
  public:
        virtual      ~MessageHandler () {}
 
/////////////////////////////////////////////////////////////////////////////////////////////
///�㭪�� ��ࠡ��稪 ᮮ�饭�� ��� ���墠�
/////////////////////////////////////////////////////////////////////////////////////////////
        typedef bool (*MsgHandler)(msg_t,MessageStream&,MessageStream&,void* self_data);

        virtual bool process (msg_t,MessageStream& in,MessageStream& out) = 0;

  private:
        static  bool ProcessMessage (msg_t,MessageStream&,MessageStream&,void*);
};

/////////////////////////////////////////////////////////////////////////////////////////////
///��ᯥ��� ᮮ�饭��
/////////////////////////////////////////////////////////////////////////////////////////////
class   MsgDispatch: public MessageHandler
{
  public:
                MsgDispatch   (Pool*);

        bool    BindMessage   (msg_t from,msg_t to = -1,MsgHandler = NULL,void* self = NULL);
        bool    BindMessage   (MessageHandler&,msg_t from,msg_t to = -1);
        void    UnbindMessage (msg_t);

/////////////////////////////////////////////////////////////////////////////////////////////
///��ࠢ����� ᮮ�饭�� ��ᯥ����
/////////////////////////////////////////////////////////////////////////////////////////////
        bool    PutMessage    (msg_t,MessageStream&);

/////////////////////////////////////////////////////////////////////////////////////////////
///��ࠡ�⪠ ᮮ�饭�� ���ᮢ����� ������� ��ᯥ���� (�� ���墠祭��� ��⮬����)
/////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool process  (msg_t,MessageStream&);

  private:
        struct msgnode_t
        {
          MsgHandler    fn;
          void*         data;
          msg_t         toMsg;
        };

        typedef Dict<msgnode_t,msg_t>  MsgDict;

        MsgDict          mDict;
};
