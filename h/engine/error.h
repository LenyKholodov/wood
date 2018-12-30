/////////////////////////////////////////////////////////////////////////////////////////////
///Ошибка, возникшая в результате работы сервера
/////////////////////////////////////////////////////////////////////////////////////////////
class   GameError
{
  public:
    typedef long err_t;

              GameError  ();
              GameError  (err_t);
     virtual  ~GameError ();     

     void     raise     (); 
     
     friend   ostream& operator << (ostream&,GameError&);

  protected:
     virtual  void     dump  (ostream& os);

  private:
    err_t       error;                    
};

inline ostream& operator << (ostream& os,GameError& err)
{
  err.dump (os);
  return os;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///Обработчик ошибок
/////////////////////////////////////////////////////////////////////////////////////////////
class   ErrorHandler
{
  friend class ErrorDispatch;
  public:
        virtual ~ErrorHandler () {}
        
        typedef GameError::err_t err_t;

/////////////////////////////////////////////////////////////////////////////////////////////
///Функция обработчик ошибок
/////////////////////////////////////////////////////////////////////////////////////////////
        typedef bool (*ErrHandler)(GameError&,void* self_data);

        virtual bool process (GameError&) = 0;

  private:
        static  bool ProcessError (GameError&,void*);
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Диспетчер обработки ошибок
/////////////////////////////////////////////////////////////////////////////////////////////
class ErrorDispatch: public ErrorHandler
{
  public:
                ErrorDispatch (ErrorHandler* owner,Pool*);
                ErrorDispatch (ErrorDispatch&);

        bool    BindError     (err_t,ErrHandler = NULL,void* self = NULL);
        bool    BindError     (ErrorHandler&,err_t);
        void    UnbindError   (err_t);

/////////////////////////////////////////////////////////////////////////////////////////////
///Отправление генерация ошибки
/////////////////////////////////////////////////////////////////////////////////////////////
        bool    raise         (GameError&);

/////////////////////////////////////////////////////////////////////////////////////////////
///Обработка ошибок адресованных данному диспетчеру (не перехваченных потомками)
/////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool process  (GameError&);

  private:
        struct errnode_t {
          ErrHandler    fn;
          void*         data;
        };

        typedef Dict<errnode_t,err_t>   ErrDict;

        ErrDict        mDict;
        ErrorHandler*  mOwner;
};