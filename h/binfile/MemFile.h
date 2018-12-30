#ifndef __FILE_MEMORY_FILE__
#define __FILE_MEMORY_FILE__
        
#include <binfile\binfile.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///Файл в памяти
/////////////////////////////////////////////////////////////////////////////////////////////
class MemBinFile : public BinFile
{
  public:
                   MemBinFile  (Pool*,size_t size = 0);
                   MemBinFile  (void* buf,size_t size);
    virtual        ~MemBinFile ();

/////////////////////////////////////////////////////////////////////////////////////////////
///Доступ к буферу
/////////////////////////////////////////////////////////////////////////////////////////////
               void*    GetBuffer   ()  const { return mBuffer; } 

/////////////////////////////////////////////////////////////////////////////////////////////
///Чтение / запись буферов в файл
/////////////////////////////////////////////////////////////////////////////////////////////
               size_t   AssignFrom  (BinFile&); 
               size_t   AssignTo    (BinFile&); 

    virtual    bool     valid       () const { return mBuffer != NULL; }

  protected:        
    virtual    size_t   _resize (size_t);
    virtual    size_t   _read   (void*,size_t,filepos_t);
    virtual    size_t   _write  (const void*,size_t,filepos_t);

               bool     open    (size_t size,int mode); 

  private:
     bool      mOwner;
     char*     mBuffer;
     Pool*     mPool;
};


#endif