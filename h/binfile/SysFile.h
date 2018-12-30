#ifndef __FILE_DISK_FILE__
#define __FILE_DISK_FILE__

#include <binfile\binfile.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///Файл обычный (открывается посредством ОС)
/////////////////////////////////////////////////////////////////////////////////////////////
class SysBinFile : public BinFile
{
  public:
                 SysBinFile  (const char* file_name,int access_type);
        virtual  ~SysBinFile ();

        virtual bool         valid  () const { return mSysHandle != -1; }

  protected:        
        virtual filepos_t    _seek   (filepos_t);
        virtual size_t       _resize (size_t);
        virtual size_t       _read   (void*,size_t,filepos_t);
        virtual size_t       _write  (const void*,size_t,filepos_t);

  private:
        int     mSysHandle;
};

#endif