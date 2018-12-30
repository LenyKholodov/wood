#ifndef __PAK_BIN_FILE__
#define __PAK_BIN_FILE__

#include <binfile\binfile.h>
#include <iomanip>
#include <misc.h>

typedef Ring<BinFile*>     FileRing;
typedef Ring<SearchFile*>  SearchFileRing;

class PackFile;

/////////////////////////////////////////////////////////////////////////////////////////////
///Кусок пака. При удалении unregistered
/////////////////////////////////////////////////////////////////////////////////////////////
class PackLump
{
  public:
                PackLump  (PackFile*,BinFile*);
        virtual ~PackLump ();

  protected:
      PackFile*       mPak;  
      BinFile*        mXFile;
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Протокласс для паков
/////////////////////////////////////////////////////////////////////////////////////////////
class   PackFile: public BinFile
{
  friend class PackLump;
  friend class PackSearch;

  protected:
                                PackFile  (Pool* files_pool = Memory::M_GetPool(),size_t = 0);
                                PackFile  (PackFile* pak); //copy pool

  public:
                       virtual  ~PackFile (); //autodel all open files

        virtual bool            valid () const { return mFilePool && mFiles.valid (); }

/////////////////////////////////////////////////////////////////////////////////////////////
///Открытие / закрытие файлов из пака
/////////////////////////////////////////////////////////////////////////////////////////////  
        virtual BinFile*        open  (const char* file_name,int access_mode) { return NULL; }
        virtual void            close (BinFile* file) { if (file) delete file; }

/////////////////////////////////////////////////////////////////////////////////////////////
///Поиск
/////////////////////////////////////////////////////////////////////////////////////////////
                SearchFile*     OpenSearch  (const char* tmpl);
                void            CloseSearch (SearchFile* s) { if (s) delete s; }
  protected:
        virtual SearchFile*     _OpenSearch (const char*) { return NULL; }          
  public:

/////////////////////////////////////////////////////////////////////////////////////////////
///Информация
/////////////////////////////////////////////////////////////////////////////////////////////
        virtual uint            count (bool recurse = true) const;
        virtual void            dump  (ostream&,bool recurse=true,int left=0) const;

/////////////////////////////////////////////////////////////////////////////////////////////
///Сканирование (в случае удалённых или объёмных паков)
/////////////////////////////////////////////////////////////////////////////////////////////
        virtual void            refresh () {}

  private:
                BinFile::read; BinFile::write; BinFile::_read; BinFile::_write;
                BinFile::seek; BinFile::reset; BinFile::resize;
                BinFile::_seek; BinFile::_reset; BinFile::_resize;

  protected:
/////////////////////////////////////////////////////////////////////////////////////////////
///Сброс буферов у всех вложенных файлов
/////////////////////////////////////////////////////////////////////////////////////////////
        virtual void            _flush   ();

                void            insert (BinFile* file) { mFiles.insert (file); }
                void            erase  (BinFile* file) { mFiles.erase  (file); }

                void            insert (SearchFile* s) { mSearches.insert (s); }
                void            erase  (SearchFile* s) { mSearches.erase  (s); }

                Pool*           GetPool () const { return mFilePool; }
                Pool*           M_GetPool () const { return mFilePool; }
                void            SetPool (Pool* pool)  { mFilePool = pool; }

/////////////////////////////////////////////////////////////////////////////////////////////
///Dump & count
/////////////////////////////////////////////////////////////////////////////////////////////
                void            _dump (const char* dir,ostream& os,int left,bool recurse) const;
                uint            _count (const char* dir,bool recurse) const;

  protected:
        autoptr_t<Pool> mFilePool;

  private:
        FileRing        mFiles;
        SearchFileRing  mSearches;        
        bool            mPoolOwner;
};

#endif