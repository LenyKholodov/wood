#ifndef __FAST_PATH_BIN_FILE__
#define __FAST_PATH_BIN_FILE__

#include <binfile\packfile.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///Каталог
/////////////////////////////////////////////////////////////////////////////////////////////
class    FastPathPackFile: public PackFile
{
  friend class FastPathSearch;

  public:
                FastPathPackFile  (PackFile* file,const char* full_path);
      virtual   ~FastPathPackFile ();

        virtual BinFile*        open  (const char*,int = BF_OPEN_RO);
        virtual void            close (BinFile*);

        virtual void            dump  (ostream&,bool,int) const;

        virtual bool            valid () const { return mPak != NULL; }

                void            SetDirName (const char* dname) { mDirName = dname; }

  protected:
        virtual SearchFile*     _OpenSearch (const char* tmpl);
      
  protected:
        Str256     mDirName;
        PackFile*  mPak;
};

#endif