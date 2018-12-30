#ifndef __NET_TCP_IP_BIN_FILE__
#define __NET_TCP_IP_BIN_FILE__

#include <binfile.h>
#include <winsock2.h>

///////////////////////////////////////////////////////////////////////////////////////////
///Файл читающий из сокета
///////////////////////////////////////////////////////////////////////////////////////////
class    socketBinFile: public BinFile
{
  public:
                        socketBinFile (const char* inet_addr,
                                       ulong       port,
                                       int         mode, //RO/RW
                                       int         domain = AF_INET,
                                       int         type   = SOCK_STREAM,
                                       int         protocol = 0
                                      );
                        socketBinFile (ulong       inet_addr,
                                       ulong       port,
                                       int         mode, //RO/RW
                                       int         domain = AF_INET,
                                       int         type   = SOCK_STREAM,
                                       int         protocol = 0
                                      );
         virtual        ~socketBinFile ();

////////////////////////////////////////////////////////////////////////
///Определяет является ли файл - файлом с быстрым обменом
////////////////////////////////////////////////////////////////////////
                 void            SetFastXCHNG (bool flag = false) { mFlags = flag ? MSG_OOB : 0; }
                         
         virtual bool            valid    () const { return BinfFile::valid () && 
                                                     mSocket >= 0 && mConnect; } 
                          
         virtual bool            reset    ()  { mFilePos = 0; }
         virtual filepos_t       seek     (filepos_t) { return mFilePos; } //no seek ???

  protected:
         virtual size_t          resize   (size_t size)  { mFileLen = size; }

  protected:
        virtual size_t          _read    (void*,size_t);
        virtual size_t          _write   (const void*,size_t);

  private:
                bool            init     (ulong,ulong,int,int,int,int);

  private:
    SOCKET      mSocket;
    int         mFlags, mConnect;
};

#endif
