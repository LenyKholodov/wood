#ifndef __MESSAGE_STREAM_WITH_BUFFER__
#define __MESSAGE_STREAM_WITH_BUFFER__

#include <engine\stream.h>
#include <memory\memory.h>
#include <misc\excpt.h>

////////////////////////////////////////////////////////////////////////////////////////////
///Поток сообщений с привязанным буфером
////////////////////////////////////////////////////////////////////////////////////////////
class   BufMessageStream: public MessageStream
{
  public:
                BufMessageStream   (Pool*,size_t) throw (ExcNoMemory&);
                BufMessageStream   (void*,size_t);
    virtual     ~BufMessageStream  ();

      virtual   bool     valid     () const { return mBuf && mReadPos && mWritePos; }

  protected:
      virtual   bool     _LockWrite      (msg_t); 
      virtual   size_t   _write          (const void*,size_t);
      virtual   bool     _UnlockWrite    (); 

      virtual   bool     _LockRead       (msg_t&);
      virtual   size_t   _read           (void*,size_t);
      virtual   size_t   _gets           (char*,bool& end);
      virtual   bool     _UnlockRead     ();

  private:
      size_t       mSize, mWriteAvail, mReadAvail;
      uchar        *mBuf, *mReadPos, *mWritePos;
      message_t*   mWriteCur;
      bool         mBufDel;
};

#endif