#ifndef __CONSOLE_BASE__
#define __CONSOLE_BASE__

#include <console\conbuf.h>
#include <console\comline.h>

class    ConsoleBase;
class    ioconsolebuf;
class    ioconsolebase;
class    ioconsolestream;

////////////////////////////////////////////////////////////////////////////////
///Абстрактная графическая консоль
////////////////////////////////////////////////////////////////////////////////
class    GraphConsole: public MemObject
{
  friend class ConsoleBase;
  public:
                        GraphConsole  (Pool*,ConsoleBase&);
           virtual      ~GraphConsole ();

    virtual     void    textcolor (long) = 0;

  protected:
    virtual     void    writeln (int,const char*) = 0;
    virtual     void    begin   () = 0;
    virtual     void    end     () = 0;    

                void    width   (int);
                void    height  (int);
  
  protected:
        ConsoleBase&    mConBase;
};

////////////////////////////////////////////////////////////////////////////////
///Прототип текстовой консоли
////////////////////////////////////////////////////////////////////////////////
class    ConsoleBase: public MemObject
{
  friend class GraphConsole;
  friend class ioconsolebuf;
  public:
//                ConsoleBase (Pool*,int _LineNum,int _LineLen);
                ConsoleBase  (Pool*,const char* log,int _LineNum,int _LineLen);
  virtual       ~ConsoleBase ();

                enum {
                  DEFAULT_CONSOLE_WIDTH  = 80,
                  DEFAULT_CONSOLE_HEIGHT = 25,

                  CJ_LEFT = 0, CJ_RIGHT
                };

////////////////////////////////////////////////////////////////////////////////
///Доступ
////////////////////////////////////////////////////////////////////////////////
       ostream&                 os         ();
       int                      count      () const     { return mBuf->count (); }
       operator ostream&                   ()           { return os  ();         }

       int                      top        () const     { return mTop;          }
       int                      bottom     () const     { return mTop+mHeight;  }
       int                      width      () const     { return mWidth;        }
       int                      height     () const     { return mHeight;       }

       GraphConsole*            grcon      ()           { return mGrConsole; }

////////////////////////////////////////////////////////////////////////////////
///Изменение окна отсечения консоли
////////////////////////////////////////////////////////////////////////////////
  protected:
       void                     width      (int w) { mWidth = w;         }
       void                     height     (int h) { mHeight = h;        }

  public:
       void                     top        (int);
       
       void                     ScrollDown (int = 1);
       void                     ScrollUp   (int = 1);

////////////////////////////////////////////////////////////////////////////////
///Пересылка сообщения на консоль
////////////////////////////////////////////////////////////////////////////////
       BOOL                     message   (const char*); 

////////////////////////////////////////////////////////////////////////////////
///Обновление консоли
////////////////////////////////////////////////////////////////////////////////
       void                     refresh   (); 
       void                     refresh   (BOOL);  //auto message and scroll redraw

  protected:
       void                     printf    (int,const char*,int);

  public:
       void                     bind      (GraphConsole* con) { mGrConsole = con; }
       void                     bind      (CommandLine& cl) { mComLine = &cl; }
       void                     unbind    (GraphConsole* con) { if (mGrConsole == con) mGrConsole = NULL; }
       void                     unbind    (CommandLine& cl) { if (mComLine == &cl) mComLine = NULL; }

  private:
        ConsoleBuffer*          mBuf;
        BinFile*                mFile;
        GraphConsole*           mGrConsole;
        CommandLine*            mComLine;
        int                     mWidth, mHeight, mTop; 
        char*                   mTempBuf;
        BOOL                    mAutoRedraw;
        ioconsolestream*        mOs;        
};

#endif