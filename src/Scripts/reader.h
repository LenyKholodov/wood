#include <scripts\framerdr.h>

class    BinFile;
class    Pool;

/////////////////////////////////////////////////////////////////////////////////////////////
///Буффер поддержки
/////////////////////////////////////////////////////////////////////////////////////////////
class    iobuffer: public streambuf
{
  friend class FrameReader::Reader;
  public:
                iobuffer (FrameReader::Reader& rdr);

    virtual     int     underflow ();
    virtual     int     overflow  (int c) { return c; }

/////////////////////////////////////////////////////////////////////////////////////////////
///Сброс                
/////////////////////////////////////////////////////////////////////////////////////////////
                bool    reset     ();

  private:
        FrameReader::Reader& mReader;
        bool                      mNeedClose, mUse;
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Читатель
/////////////////////////////////////////////////////////////////////////////////////////////
class    FrameReader::Reader: public MemObject
{
  friend class iobuffer;
  public:
                        Reader  (BinFile*    file,
                                 BinFile*    assign,
                                 ostream&    err,
                                 Pool*       pool,
                                 size_t      cache_buf_size = 16*1024
                                );
       virtual          ~Reader ();  

/////////////////////////////////////////////////////////////////////////////////////////////
///Выявление лексемы и присоединённого потока
/////////////////////////////////////////////////////////////////////////////////////////////
          char*         next    (ulong& crc); 
          istream&      stream  (bool noEndLine = false);

/////////////////////////////////////////////////////////////////////////////////////////////
///Открытие / загрытие тэга
/////////////////////////////////////////////////////////////////////////////////////////////
          struct state_t
          {
            int   level;
            bool  openTag, closeTag;
          };
                
          void          OpenTag   (state_t&); 
          void          CloseTag  (const state_t&);

/////////////////////////////////////////////////////////////////////////////////////////////
///Поток ошибок
/////////////////////////////////////////////////////////////////////////////////////////////
          ostream&      err     () { return mErr; }

/////////////////////////////////////////////////////////////////////////////////////////////
///Текущий уровень вложения 
/////////////////////////////////////////////////////////////////////////////////////////////
          uint          level   () const { return mLevel; }      

/////////////////////////////////////////////////////////////////////////////////////////////
///Пропуск тэга
/////////////////////////////////////////////////////////////////////////////////////////////
           void         TraceTag (); 

/////////////////////////////////////////////////////////////////////////////////////////////
///Вывод сообщения об ошибке
/////////////////////////////////////////////////////////////////////////////////////////////
           void         error    (const char*,...);

/////////////////////////////////////////////////////////////////////////////////////////////
///Чтение из файла поддержки
/////////////////////////////////////////////////////////////////////////////////////////////
           size_t       read     (void*,size_t,long = -1);

           void         ChangeAssignFile (const char*,bool compress = false);
           void         ChangeAssignFile (BinFile* file,bool autoDel = false);
           void         ChangeAssignDir  (const char* dir);

  private:
           int           read         ();
           void          back         ();
           void          ReadComments (bool);
           size_t        LoadBuffer   ();

  private:
        iobuffer            mIOBuffer;
        istream_withassign  mStream; 
        ostream&            mErr;

        BinFile             *mFile, *mAssign;
        bool                mAssignDel;
        char                *mBuf, *mEnd, *mPos;
        size_t              mBufSize;
        Str128              mTag;       

        int                 mLevel, mCurLevel, mLine;
        bool                mOpenTag, mCloseTag, mReadString1, mReadString2;

        Str128              mAssignDir;
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Узел в дереве разбора
/////////////////////////////////////////////////////////////////////////////////////////////
class    FrameReader::Frame: public MemObject
{
  public:
                Frame  (Frame*);
                Frame  (FrameReader*);
                ~Frame ();

/////////////////////////////////////////////////////////////////////////////////////////////
///Обработка
/////////////////////////////////////////////////////////////////////////////////////////////
    bool    process    (FrameReader::Reader*);

/////////////////////////////////////////////////////////////////////////////////////////////
///Добавление тэгов
/////////////////////////////////////////////////////////////////////////////////////////////
    bool    AddTag     (const char*,FrameReader::ReadValueFn,void*,size_t);    
    bool    AddTag     (const char*,void**,FrameReader::CreateTagFn,void*,size_t); //size only for init
    void    DelTag     (const char*);

/////////////////////////////////////////////////////////////////////////////////////////////
///Изменение заказных параметров
/////////////////////////////////////////////////////////////////////////////////////////////
    void    SetParams  (void** ptr,FrameReader::CreateTagFn init,void* self = NULL);

/////////////////////////////////////////////////////////////////////////////////////////////
///Доступ к данным
/////////////////////////////////////////////////////////////////////////////////////////////
    void*    GetObj     () const { return mObj; }
    Frame*   GetParent  () const { return mParent; }

  private:
    struct tag_t
    {
      void*             data;
      size_t            size; //for arrays
      ReadValueFn       fn;           
      Frame*            frame;

                        tag_t (ReadValueFn,void*,size_t);
                        tag_t (Frame*,void* = NULL,size_t = 0);
    };

    struct last_t
    {
      ulong   crc;   //CRC
      tag_t*  tag;   //тэг

              last_t (): tag (NULL), crc (0xFFFFFFFF) {}
    };

    typedef StringDict<tag_t>   TagDict;
    typedef StringDict<Frame*>  ChildDict;

  private:
    bool    ReadFrame     (tag_t*);

    bool    ReadDirective (const char* dirName);
    void    ReadDirAssign ();
    bool    ReadDirChunk  ();

    void    clear         (ChildDict::iter);

/////////////////////////////////////////////////////////////////////////////////////////////
///Вход / обработка / выход из фрейма
/////////////////////////////////////////////////////////////////////////////////////////////
    bool    open       (FrameReader::Reader*);
    void    next       ();
    bool    close      ();

/////////////////////////////////////////////////////////////////////////////////////////////
///Поиск вложенных фреймов
/////////////////////////////////////////////////////////////////////////////////////////////
    Frame*  find       (const char*,const char*&) const;

  private:
    FrameReader::Reader*         mReader;
    FrameReader*                 mOwner;
    FrameReader::Reader::state_t mState;
    bool                         mOpen;
    
    Frame**                      mCurFrame;

    FrameReader::CreateTagFn     mHandler;
    void*                        mSelf;
    void**                       mPtr, *mObj;

    TagDict                      mTags;
    ChildDict                    mChildren;
    Frame*                       mParent;
    last_t                       mLast;    
};
