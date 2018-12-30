#ifndef __KERNEL_FILE_SERVICE__
#define __KERNEL_FILE_SERVICE__

#include <binfile\packfile.h>
#include <kernel\service.h>
#include <kernel\critsect.h>
#include <misc.h>

class FileGroup;
class CurrentDirPak;

///////////////////////////////////////////////////////////////////////////////
///Прототип создания пак файлов
///////////////////////////////////////////////////////////////////////////////
typedef PackFile* (*OpenPackFileFn)(const char* pak_name,Pool* pak_pool);

////////////////////////////////////////////////////////////////////////////////
///Модуль работы с файлами
////////////////////////////////////////////////////////////////////////////////
class    FileService: public Service, public PackFile
{
  friend class FileGroup;
  friend class GroupSearch;
  friend class MainPackSearch;

  public:
                        enum {
                          MAX_PATH_LEN = 128
                        };

                        enum Errors {
                          FILE_NOT_FOUND     = 1,
                          GROUP_NOT_FOUND    = 2,
                          NO_ENOUGH_MEMORY   = 3,
                          ERROR_PAK_REGISTER = 4,
                          LAST_ERROR         = 5
                        };

                        FileService  (Pool*,size_t filePoolSize = 0);
          virtual       ~FileService ();
                                
    virtual   const char*       ServiceName () const { return "File service"; }

////////////////////////////////////////////////////////////////////////////////
///Дубляж с PackFile
////////////////////////////////////////////////////////////////////////////////
    virtual   BinFile*          open       (const char* name,int mode) { return OpenFile (name,mode); }

    virtual   SearchFile*       OpenSearch (const char*);
 
    virtual   uint              count      (bool = true) const;
    virtual   void              dump       (ostream& os,bool = true,int = 0) const;

////////////////////////////////////////////////////////////////////////////////
///Открытие файлов
////////////////////////////////////////////////////////////////////////////////
              BinFile*          OpenFile         (const char* group_name,
                                                  const char* file_name,
                                                  int         mode
                                                 );
              BinFile*          OpenFile         (const char* file_name,
                                                  int         mode
                                                 ) { return OpenFile (NULL,file_name,mode); }
              void              CloseFile        (BinFile*);

////////////////////////////////////////////////////////////////////////////////
///Добавление именованных путей к каталогам
////////////////////////////////////////////////////////////////////////////////
              bool              AddFastPath      (const char* fast_path,
                                                  const char* full_path
                                                 );
              void              DelFastPath      (const char* fast_path);                                                   

              void              SetCurrentDir    (const char* cur_dir);

////////////////////////////////////////////////////////////////////////////////
///Работа с группами
////////////////////////////////////////////////////////////////////////////////
              bool              RegisterGroup    (const char* group,
                                                  const char* file_name = NULL,
                                                  BinFile*              = NULL,
                                                  bool        autodel   = false
                                                 );
              void              UnregisterGroup  (const char* group,const char* file_name = NULL);

              bool              AddDefault       (const char* file_name,BinFile* file) { return RegisterGroup ("default",file_name,file); }
              void              DelDefault       (const char* file_name) { UnregisterGroup ("default",file_name); }

////////////////////////////////////////////////////////////////////////////////
///Регистрация пака
////////////////////////////////////////////////////////////////////////////////
              bool              RegisterPackFile (const char*    pak_name,
                                                  OpenPackFileFn fn         = OpenZipFile,
                                                  const char*    group_name = "default"
                                                 );

////////////////////////////////////////////////////////////////////////////////
///Создание псевдоимени файла
////////////////////////////////////////////////////////////////////////////////
              bool              AddAlias         (const char* group,const char* name,BinFile*);
              void              DelAlias         (const char* group,const char* name);
              bool              AddAlias         (const char* name,BinFile* f) { return AddAlias (NULL,name,f); }
              void              DelAlias         (const char* name) { DelAlias (NULL,name); }

////////////////////////////////////////////////////////////////////////////////
///Поиск
////////////////////////////////////////////////////////////////////////////////              
              SearchFile*       FirstFile        (const char* fileName) { return OpenSearch (fileName); }
              bool              NextFile         (SearchFile* s) { return s && s->next (); }
              void              CloseSearch      (SearchFile* s) { if (s) delete s; }

////////////////////////////////////////////////////////////////////////////////
///Инфо о ошибках
////////////////////////////////////////////////////////////////////////////////
    virtual   const char*       GetErrorMessage (int error) const;

              Service::operator new;
              Service::operator delete;
              
  protected:
        virtual         bool        valid            () const { return Service::valid () && 
                                                                PackFile::valid () && 
                                                                mGroups.valid () &&
                                                                IsValid (mDiskPack);
                                                              }
                        FileGroup*  GetGroup         (const char* name) const;
                        FileGroup*  GetGroup         (const char*  full_name,
                                                      const char*  group_name,
                                                      const char** file_name,
                                                      bool         create = false 
                                                     );
        static          PackFile* OpenZipFile      (const char*,Pool*); 

                        PackFile::M_GetPool;
                        PackFile::GetPool;
                        PackFile::SetPool;

  protected:
          typedef  StringDict<FileGroup*> GDict;

          CriticalSection  mCS;
          GDict            mGroups;

          BinFile*         mDiskPack;
          FileGroup*       mDefault; 
          CurrentDirPak*   mCurDir;

  private:
          static        const char* _GroupSign;
};

#endif