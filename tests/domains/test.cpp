class Object
{
  private:
    Object* owner;

  public:
    virtual void ProcessMessage (const message&);

  protected:
  //����䥩� ࠡ��� � �������
    CreateMsgTable ();    
};

class Domain: private Object 
{
  //ᯨ᮪ ��ꥪ⮢
  //⠡��� ᮪�⮢
  //��।� �室��� ᮮ�饭��
  //��ᯥ���  
};

class Socket
{
  Domain* domain;
  std::list<Object*> obj;

  void PutMessage (message* msg) {
    domain->queue.PutMessage (msg);
  }
};
