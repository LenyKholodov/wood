#ifndef __TAG_WRITE_STREAM__
#define __TAG_WRITE_STREAM__

#include <misc\string.h>
#include <misc\autoptr.h>
#include <memory\memory.h>

#include <iostream.h>

class    Pool;
class    BinFile;

/////////////////////////////////////////////////////////////////////////////////////////////
///Форматированная запись в поток
/////////////////////////////////////////////////////////////////////////////////////////////
class    FrameWriter: public ostream
{
  public:
                  FrameWriter  (const char* stream,
                                const char* assign   = NULL,
                                uint        compress = 0,//0..4 default: no compression, no zip
                                Pool*       pool     = Memory::M_GetPool (),
                                size_t      cache_size = 16*1024                                
                               ); 
                  FrameWriter  (BinFile* file,
                                BinFile* assign = NULL,
                                Pool*    pool   = Memory::M_GetPool (),
                                size_t   cache_size = 16*1024                                
                               );
      virtual     ~FrameWriter ();

/////////////////////////////////////////////////////////////////////////////////////////////
///Открытие / закрытие тэгов
/////////////////////////////////////////////////////////////////////////////////////////////
            void  OpenFrame  (const char* tag_name);
            bool  CloseFrame ();

            int   GetLevel   () const { return mLevel; }

/////////////////////////////////////////////////////////////////////////////////////////////
///Установка масштаба
/////////////////////////////////////////////////////////////////////////////////////////////
            void  SetLevelSpace (int s = 1) { mSpaceCount = s; }
            int   GetLevelSpace () const { return mSpaceCount; }

/////////////////////////////////////////////////////////////////////////////////////////////
///Запись бинарных данных
/////////////////////////////////////////////////////////////////////////////////////////////
            size_t write        (const void*,size_t);
            size_t write        (const char* tag_name,const void*,size_t,long offs = -1);

            template <class T>
            uint   WriteArray   (const char* tag_name,const T* arr,uint count,long offs = -1) {
              return write (tag_name,arr,count * sizeof (T),offs) / sizeof (T);
            }

            BinFile* assign     () const { return mAssign; }

  private:
    class tagiobuf: public streambuf
    {
      public:
           tagiobuf (BinFile*,FrameWriter&,Pool*,size_t);
           virtual ~tagiobuf ();

       virtual int   overflow   (int);  
       virtual int   underflow  (void) { return EOF; }

      private:
               void  flush      (); 

      private:
           FrameWriter&  stream;
           bool          newLine;
           BinFile*      file;

           char*         buf, *pos, *end;
           size_t        bufSize;
    };
    friend class tagiobuf;

  private:
    autoptr_t<BinFile>     mFile, mAssign;
    bool        mDelFile, mDelAssign; 

    tagiobuf    mBuf;
    int         mLevel, mSpaceCount;
};

class   EndFrameLine
{
  public:
        friend ostream& operator << (ostream&,const EndFrameLine&);
};

extern EndFrameLine  endline; //only for FrameWrite!

#ifndef WOOD_LIB_COMPILE
  #ifdef DEBUG
    #pragma comment (lib,"scriptsd.lib")
  #else
    #pragma comment (lib,"scripts.lib")
  #endif
#endif

#endif