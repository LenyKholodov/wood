#ifndef __BINFILES_GROUP__
#define __BINFILES_GROUP__

#include <binfile\packfile.h>
#include <misc.h>

struct    filedesc_t
{
  Str256    name;
  BinFile*  file;
  bool      autodel, isPak;
  ulong     crc, len;
  
          BinFile* operator -> () const { return file; }
          operator BinFile*    () const { return file; }

          filedesc_t (const char* s,BinFile* f,bool del = false): 
            file (f), name (s), len (name.length ()),autodel (del), isPak (false)
              { name.locase (); crc = name.crc ();}

          filedesc_t (const char* s,PackFile* f,bool del = false): 
            file ((BinFile*)f), name (s), len (name.length ()),autodel (del), isPak (true)
              { name.locase (); crc = name.crc ();}
};

typedef   Ring<filedesc_t>  FDRing;

class FileService;

////////////////////////////////////////////////////////////////////////////////
///Группа файлов
////////////////////////////////////////////////////////////////////////////////
class   FileGroup: public PackFile
{
  friend class GroupSearch;
  friend class FileService;

  public:
                        FileGroup  (const char* group_name,Pool* = Memory::M_GetPool (),size_t size = 0);
                        FileGroup  (const char* group_name,FileService*);
             virtual    ~FileGroup ();

////////////////////////////////////////////////////////////////////////////////
///Работа с группами файлов
////////////////////////////////////////////////////////////////////////////////
              bool      Register   (const char* name,BinFile* file,bool autodel = false);
              bool      Register   (const char* name,PackFile* pak,bool autodel = false);
              void      Unregister (const char* name);

////////////////////////////////////////////////////////////////////////////////
///pak
////////////////////////////////////////////////////////////////////////////////
   virtual    BinFile*     open       (const char* name,int mode);

   virtual    uint         count      (bool recurse = true) const;
   virtual    void         dump       (ostream& os,bool = true,int = 0) const;

////////////////////////////////////////////////////////////////////////////////
///Установка первого файла в списке поиска
////////////////////////////////////////////////////////////////////////////////
              void         SetFirstPak (int first) {  mFirst = first; }

  protected:
   virtual    SearchFile*  _OpenSearch (const char*);
              
  protected:
               FDRing           mFileDesc;
               Str128           mName;

  private:
               FileService*     mOwner;
               int              mFirst;
};

class  CurrentDirPak: public FastPathPackFile
{
  public:
         CurrentDirPak  (PackFile* file);

  protected:
        virtual BinFile*        open  (const char*,int = BF_OPEN_RO);
        virtual SearchFile*     _OpenSearch (const char* tmpl);

  private:
         bool   mOpen, mOpenSearch;
};

#endif