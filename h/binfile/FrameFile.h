#ifndef __FILE_ABINFILE__
#define __FILE_ABINFILE__

#include <binfile\binfile.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///Фрейм другого файла
/////////////////////////////////////////////////////////////////////////////////////////////
class FrameBinFile : public BinFile
{
  public:
                        FrameBinFile  (BinFile*  file,
                                       filepos_t from,
                                       size_t    len,
                                       int       mode = -1 //copy attributes from owner
                                  );

        virtual bool    valid    () const { return mFile != NULL; }

  protected:
        virtual size_t  _read    (void*,size_t,filepos_t);
        virtual size_t  _write   (const void*,size_t,filepos_t);
        virtual size_t  _resize  (size_t);

  protected:
        BinFile*        mFile;
        filepos_t       mOffs;
};

#endif