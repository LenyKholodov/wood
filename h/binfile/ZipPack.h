#ifndef __BINFILE_ZIP_BINFILE__
#define __BINFILE_ZIP_BINFILE__

#include <binfile\packfile.h>
#include <zip\unzip.h>

typedef struct
{
        char    name[256];
        DWORD   attr;
        DWORD   offset;
        DWORD   size;
        DWORD   c_offset;
        ulong   time;
} fileinfo_t;

typedef struct
{
        char                    pak_name[256];
        unzFile                 uf;
        unz_global_info         gi;
        fileinfo_t              *fi;
} packfile_t;

////////////////////////////////////////////////////////////////////////////////
///ZIP-file
////////////////////////////////////////////////////////////////////////////////
class    ZipPackFile: public PackFile
{
  friend class  UnpackNoCompression;
  friend class  UnpackCompression;
  friend class  ZipSearch;

  public:
                  ZipPackFile  (const char*,Pool* = Memory::M_GetPool(),size_t = 0);
        virtual   ~ZipPackFile ();

        virtual BinFile*        open  (const char*,int = BF_OPEN_RO);

        virtual uint            count (bool = true) const;
        virtual void            dump  (ostream&,bool = true,int = 0) const;

        virtual bool            valid () const { return PackFile::valid () &&
                                                 IsValid (mStdFile) && 
                                                 mIndex.valid (); 
                                               }

  protected:
        virtual SearchFile*     _OpenSearch  (const char* tmpl);

  private:
                bool            OpenPack  (const char*);
    static      void            ConvNames (const char*,char*);
          
  protected:
                typedef Dict<long> IRing;

        BinFile*        mStdFile, *mCacheFile;
        packfile_t      mPackFile;
        IRing           mIndex;
};

#endif