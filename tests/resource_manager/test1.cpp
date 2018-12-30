
class Archive;

////////////////////////////////////////////////////////////////////////////////
///Абстрактный класс файла
////////////////////////////////////////////////////////////////////////////////
class File: public Resource
{
  protected:
                enum Attrib { 
                  can_read   = 1, 
                  can_write  = 2, 
                  can_seek   = 4, 
                  can_reset  = 8,
                  can_resize = 16,
                  can_all    = 0xFFFFFFFF
                };


                BinFile  (const Archive&,uint mode); 

  public:
      virtual   ~BinFile () {}

                typedef int  filepos_t;
                typedef uint filesize_t;

///////////////////////////////////////////////////////////////////////////////
///Чтение / запись
///////////////////////////////////////////////////////////////////////////////
                size_t          read     (void*,size_t,filepos_t = -1); //ret: 0 - no read
                size_t          write    (const void*,size_t,filepos_t = -1); //ret: 0 - no write

///////////////////////////////////////////////////////////////////////////////
///Сброс в начало / перемещение / изменение размера
///////////////////////////////////////////////////////////////////////////////
                bool            reset    ();  
                filepos_t       seek     (filepos_t); //ret: current file position

                size_t          resize   (filesize_t);  //ret: current file length

///////////////////////////////////////////////////////////////////////////////
///Сброс буферов на диск
///////////////////////////////////////////////////////////////////////////////
                void            flush    (); 

///////////////////////////////////////////////////////////////////////////////
///Доступ
///////////////////////////////////////////////////////////////////////////////
                size_t          size     ()    const    { return mFileLen; }
                filepos_t       getpos   ()    const    { return mFilePos; }

                bool            eof      ()    const    { return (uint)mFilePos>=mFileLen; }

                bool            canread  ()    const    { return (mMode & can_read) != 0;   }
                bool            canwrite ()    const    { return (mMode & can_write) != 0;  }
                bool            canseek  ()    const    { return (mMode & can_seek) != 0;   }
                bool            canreset ()    const    { return (mMode & can_reset) != 0;  }
                bool            canresize()    const    { return (mMode & can_resize) != 0; }

  protected:
///////////////////////////////////////////////////////////////////////////////////////
///Следующие функции перегружаются в потомках и работают без проверки на корректность
//////////////////////////////////////////////////////////////////////////////////////
        virtual size_t          _read    (void*,size_t,filepos_t)       { return 0; }
        virtual size_t          _write   (const void*,size_t,filepos_t) { return 0; }
        virtual bool            _reset   ()                   { return _seek (0) == 0; } 
        virtual filepos_t       _seek    (filepos_t pos)      { return pos;  }
        virtual size_t          _resize  (size_t size)        { return size; }
        virtual void            _flush   ()                   {}

  private:
    struct cachechunk_t;

    Archive&   mArchive;
    filepos_t  mFilePos;
    filesize_t mFileSize;     
    uint       mAttrib;
    CriticalSection mCS; 
    cachechunk_t* mFirst;    
};

class Archive: public ResourceManager
{
  public:
    typedef char* filename_t;

        File* OpenFile (const filename_t&,Mode);
        void  DeleteFile (const filename_t&);
        void  CopyFile (const filename_t&,const filename_t&);
        void  MoveFile (const filename_t&,const filename_t&);
        Search* OpenSearch  (const filename_t&);
        void  FileInfo (const filename_t&,fileinfo_t&);
        void  AddComment (const filename_t&,const char* comment);
        void  AddComment (const char* comment);
        
  private:
};
