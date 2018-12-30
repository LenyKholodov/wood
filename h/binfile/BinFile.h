#ifndef __FILE_BINFILE__
#define __FILE_BINFILE__

#include <memory\object.h>
#include <kernel\critsect.h>
#include <debug\defs.h>

class PackFile;

typedef unsigned __int64 filesize_t;
typedef unsigned __int64 filetime_t;
typedef uint             fileattr_t;

enum FileAttrib {
  BF_ATTR_NORMAL     = 0,       //пустые   
  BF_ATTR_ARCHIVE    = 1,       //архивный
  BF_ATTR_DIRECTORY  = 2,       //каталог
  BF_ATTR_READ_ONLY  = 4        //только для чтения
};

///////////////////////////////////////////////////////////////////////////////
///Описатель времени файла
///////////////////////////////////////////////////////////////////////////////
struct systime_t
{
  uint16 year, month, dayOfWeek, day, hour, minute, second, milisec;

         systime_t () {}
         systime_t (filetime_t);
         systime_t& operator = (filetime_t);

  friend std::ostream& operator << (std::ostream&,const systime_t&);
};

///////////////////////////////////////////////////////////////////////////////
///Объект поиска
///////////////////////////////////////////////////////////////////////////////
class   SearchFile
{
  friend class PackFile;
  protected:
                SearchFile  (PackFile*);

  public:
       virtual  ~SearchFile ();

                enum  FileTimeType {
                  CREATE = 0,      //создан
                  WRITE,           //последняя запись
                  ACCESS           //доступ
                };

//////////////////////////////////////////////////////////////////////////////
///Поиск 
//////////////////////////////////////////////////////////////////////////////  
       virtual  bool        next () = 0; //итератор
       virtual  bool        valid () const = 0;

       virtual  const char* name () = 0;
       virtual  filesize_t  size () = 0;
       virtual  fileattr_t  attr () = 0;
       virtual  filetime_t  time (FileTimeType = ACCESS) = 0;

  protected:
       static   bool        cmp  (const char* tmpl,const char* fname); 

  protected:
       PackFile*   mPak;
};

////////////////////////////////////////////////////////////////////////////////
///Абстрактный класс файла
////////////////////////////////////////////////////////////////////////////////
class    BinFile: public MemManager
{
  friend class FileService;

  protected:
                BinFile  (int mode = 0);
                BinFile  (const BinFile&); 

  public:
      virtual   ~BinFile () {}

                enum States { 
                  can_read   = 1, 
                  can_write  = 2, 
                  can_seek   = 4, 
                  can_reset  = 8,
                  can_chsize = 16,
                  can_read_chsize = 32,
                  can_pack   = 64,
                  create     = 128,
                  can_resize = can_chsize,
                  can_read_resize = can_read_chsize,
                  can_rw     = can_read | can_write,                  
                  can_all    = 0xFFFFFFFF
                };

                typedef long filepos_t;

///////////////////////////////////////////////////////////////////////////////
///Корректность
///////////////////////////////////////////////////////////////////////////////
        virtual bool            valid    () const { return true; }        

///////////////////////////////////////////////////////////////////////////////
///Чтение / запись
///////////////////////////////////////////////////////////////////////////////
                size_t          read     (void*,size_t,filepos_t = -1); //ret: 0 - no read
                size_t          write    (const void*,size_t,filepos_t = -1); //ret: 0 - no write

///////////////////////////////////////////////////////////////////////////////
///Сброс в начало / перемещение / изменение размера
///////////////////////////////////////////////////////////////////////////////
                bool            reset    (void);  
                filepos_t       seek     (filepos_t); //ret: current file position

                size_t          resize   (size_t);  //ret: current file length

///////////////////////////////////////////////////////////////////////////////
///Сброс буферов на диск
///////////////////////////////////////////////////////////////////////////////
                void            flush    (); 

///////////////////////////////////////////////////////////////////////////////
///Доступ
///////////////////////////////////////////////////////////////////////////////
                int             getmode  ()    const    { return mMode; }
                size_t          size     ()    const    { return mFileLen; }
                filepos_t       getpos   ()    const    { return mFilePos; }

                bool            eof      ()    const    { return (uint)mFilePos>=mFileLen; }

                bool            canread  ()    const    { return (mMode & can_read) != 0;   }
                bool            canwrite ()    const    { return (mMode & can_write) != 0;  }
                bool            canseek  ()    const    { return (mMode & can_seek) != 0;   }
                bool            canreset ()    const    { return (mMode & can_reset) != 0;  }
                bool            canresize()    const    { return (mMode & can_resize) != 0; }
                bool            canreadresize()const    { return (mMode & can_read_resize) != 0; }
                bool            canpack  ()    const    { return (mMode & can_pack) != 0;   }

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

///////////////////////////////////////////////////////////////////////////////////////
///Блокирование области файла
///////////////////////////////////////////////////////////////////////////////////////
        virtual bool            lock     (filepos_t,size_t)   { mCS.enter (); return true; }
        virtual bool            unlock   (filepos_t,size_t)   { mCS.leave (); return true; }

  protected: 
            int                 mMode;
            filepos_t           mFilePos;
            size_t              mFileLen;

            CriticalSection     mCS;
};

inline bool    IsValid (const BinFile* file) { return file  && file->valid (); }
inline bool    IsValid (const BinFile& file) { return &file && file.valid (); }

enum BFAccess { 
  BF_OPEN_READ   = BinFile::can_read,
  BF_OPEN_WRITE  = BinFile::can_write,
  BF_OPEN_SEEK   = BinFile::can_seek,
  BF_OPEN_RESET  = BinFile::can_reset,
  BF_OPEN_RESIZE = BinFile::can_chsize,
  BF_OPEN_CREATE = BinFile::create,

  BF_FILE_ATTR_MASK = 4095,
  BF_OPEN_SYS_ATTR = 4096,
  BF_OPEN_CACHE  = BF_OPEN_SYS_ATTR, 

  BF_PACK_ATTR_SHIFT  = 15,
  BF_PACK_ATTR_MASK   = 7,

  BF_OPEN_PACK_STREAM  = 1l << (BF_PACK_ATTR_SHIFT-1), //default
  BF_OPEN_PACK_STREAM0 = (1l << BF_PACK_ATTR_SHIFT) | BF_OPEN_PACK_STREAM, //no compression
  BF_OPEN_PACK_STREAM1 = (2l << BF_PACK_ATTR_SHIFT) | BF_OPEN_PACK_STREAM,
  BF_OPEN_PACK_STREAM2 = (3l << BF_PACK_ATTR_SHIFT) | BF_OPEN_PACK_STREAM,
  BF_OPEN_PACK_STREAM3 = (4l << BF_PACK_ATTR_SHIFT) | BF_OPEN_PACK_STREAM,
  BF_OPEN_PACK_STREAM4 = (5l << BF_PACK_ATTR_SHIFT) | BF_OPEN_PACK_STREAM, //best compression
  
  BF_FILE_ATTR   = BF_OPEN_SYS_ATTR - 1,
     
  BF_OPEN_RO_RA  = BF_OPEN_READ|BF_OPEN_SEEK|BF_OPEN_RESET,
  BF_OPEN_RW_RA  = BF_OPEN_READ|BF_OPEN_WRITE|BF_OPEN_SEEK|BF_OPEN_RESIZE|BF_OPEN_RESET,
  BF_OPEN_CR_RA  = BF_OPEN_CREATE|BF_OPEN_RW_RA,
  
  BF_OPEN_RO     = BF_OPEN_RO_RA,
  BF_OPEN_RW     = BF_OPEN_RW_RA,
  BF_OPEN_CR     = BF_OPEN_CR_RA,

  BF_OPEN_STREAM_RO = BF_OPEN_READ|BF_OPEN_RESET,
  BF_OPEN_STREAM_RW = BF_OPEN_WRITE|BF_OPEN_RESET|BF_OPEN_RESIZE,
  BF_OPEN_STREAM_CR = BF_OPEN_STREAM_RW|BF_OPEN_CREATE

//  BF_OPEN_RO  = 0,      //read-only
//  BF_OPEN_RW,           //rewrite
//  BF_OPEN_CR            //create at open  
};

#ifdef DEBUG
  #pragma comment (lib,"filed.lib")
#else
  #pragma comment (lib,"file.lib")
#endif

#endif