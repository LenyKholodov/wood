class Object
{
  private:
    Object* owner;

  public:
    virtual void ProcessMessage (const message&);

  protected:
  //интерфейс работы с доменом
    CreateMsgTable ();    
};

class Domain: private Object 
{
  //список объектов
  //таблица сокетов
  //очередь входных сообщений
  //диспетчер  
};

class Socket
{
  Domain* domain;
  std::list<Object*> obj;

  void PutMessage (message* msg) {
    domain->queue.PutMessage (msg);
  }
};
