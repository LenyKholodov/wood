#ifndef __FILE_CACHE_BIN_FILE__
#define __FILE_CACHE_BIN_FILE__

#include <binfile\binfile.h>
#include <memory\memory.h>
#include <time.h>

enum {
  BF_DEFAULT_CACHE_SIZE = 256*1024
};

////////////////////////////////////////////////////////////////////////////////
///Простейший линейный кэш
////////////////////////////////////////////////////////////////////////////////
class CacheBinFile: public MemObject, public BinFile
{
  public:

                 CacheBinFile  (BinFile*   file,
                                Pool*      pool = Memory::M_GetPool(),
                                size_t     cache_size = BF_DEFAULT_CACHE_SIZE,
                                bool       autoDel = false
                               );
    virtual      ~CacheBinFile ();

    virtual     bool       valid  () const { return mFile && mBuf; }  

                void       SetAutodel (bool del) { mAutoDel = del; }

                MemObject::operator new;
                MemObject::operator delete;

  protected:
    struct buffer_t;

    virtual     size_t     _read   (void*,size_t,filepos_t);
    virtual     size_t     _write  (const void*,size_t,filepos_t);
    virtual     size_t     _resize (size_t);
    virtual     void       _flush  () { if (mBuf) WriteToDisk (*mBuf); }

////////////////////////////////////////////////////////////////////////////////
///Работа с буфером
////////////////////////////////////////////////////////////////////////////////        
    virtual     buffer_t*  Find           (filepos_t);

                size_t     WriteToDisk    (buffer_t&);
                size_t     ReadFromDisk   (buffer_t&);

  protected:
    BinFile*    mFile;
    size_t      mClasterSize;

  private:
    buffer_t*   mBuf;
    bool        mAutoDel;
};

/*

////////////////////////////////////////////////////////////////////////////////
///Кэш с несколькими буферами
////////////////////////////////////////////////////////////////////////////////
class  clcacheBinFile: public BinFile
{
  public:
                 enum {
                    DEFAULT_CACHE_SIZE = 256*1024
                 };


                clcacheBinFile   (BinFile*,Pool* = M_GetPool(),int num = 1,size_t = DEFAULT_CACHE_SIZE);
    virtual     ~clcacheBinFile  ();

     virtual    bool valid       () const;

  protected:

  protected:
    virtual     size_t  _read          (void*,size_t);
    virtual     size_t  _write         (const void*,size_t);  
                
                buffer_t&  FindLastUse    ();
                void       Reset          (buffer_t&,filepos_t);
                buffer_t&  Reset          (filepos_t);
                buffer_t*  Find           (filepos_t);

  private:
                clock_t    clock          () { return mTimePos++; }

  private:
                int             mNum;
                size_t          mClasterSize;
                buffer_t*       mBuffer;
                buffer_t*       mLast;
                BinFile*        mFile;
                clock_t         mTimePos;
                Pool*           mPool;
};

*/

#endif