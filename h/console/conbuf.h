#ifndef __CONSOLE_BUFFER__
#define __CONSOLE_BUFFER__

#include <pool.h>
#include <binfile.h>

////////////////////////////////////////////////////////////////////////////////
///Буфер для работы с текстовой консолью
////////////////////////////////////////////////////////////////////////////////
class    ConsoleBuffer: public MemObject
{
  friend class ioconsolebuf;
  public:
                        ConsoleBuffer  (Pool*,BinFile*,int _LineNum,int _LineLen);
        virtual         ~ConsoleBuffer ();

////////////////////////////////////////////////////////////////////////////////
///Работа с потоком (ввод)
////////////////////////////////////////////////////////////////////////////////
        BOOL            message        (const char*);

////////////////////////////////////////////////////////////////////////////////
///Работа со строками (вывод)
////////////////////////////////////////////////////////////////////////////////
        int             count          () const { return mCount; }
        int             pos            () const { return mPos;   }

        const char*     string         (int line,char*);
        void            reset          ();

  private:
        void            readln         (char*);
                        MemObject::GetPool;

  private:
        BinFile*                mFile;
        int                     mCount, mPos, mLineLen;
        ulong*                  mOffs;
};

#ifdef DEBUG
  #pragma comment (lib,"gamed.lib")
#else
  #pragma comment (lib,"game.lib")
#endif

#endif