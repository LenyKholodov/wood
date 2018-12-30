#ifndef __MAX_EXPORT_WXF_DEFINES__
#define __MAX_EXPORT_WXF_DEFINES__

#include <types.h>
#include <fstream.h>
#include <max.h>

#include "defs.h"
#include "dump.h"

class RootExport;

struct ExportInfo
{
  Interface*      i;            //Интерфейс доступа
  ExpInterface*   ei;           //Интерфейс доступа для экспорта
  TimeValue       staticFrame;  //Статичный кадр  
  Flags32         flags;        //Флаги экспорта
  Flags32         bin;          //Экспорт в бинарном виде
  RootExport*     root;         //Корень

//Обработка
  int             totalSteps, curStep;
};

//////////////////////////////////////////////////////////////////////////////////////////////
///Базовый распоределитель для всех экспортеров
//////////////////////////////////////////////////////////////////////////////////////////////
class    BaseExport: public MemManager
{
  friend class WXFExport;
  public:
                BaseExport  (FrameWriter&,ExportInfo&); //for root
                BaseExport  (const char* tag_name,
                             BaseExport& owner
                            );
    virtual     ~BaseExport (); 

//////////////////////////////////////////////////////////////////////////////////////////////
///Распределение памяти
//////////////////////////////////////////////////////////////////////////////////////////////
        void*   operator new (size_t size,Pool* pool = mExportPool) { return ::operator new (size,pool); }

  protected:
//////////////////////////////////////////////////////////////////////////////////////////////
///Распределение памяти
//////////////////////////////////////////////////////////////////////////////////////////////
        Pool*   M_GetPool () const { return mExportPool; }
        Pool*   GetPool   () const { return mExportPool; }
        void    SetPool   (Pool* pool) { mExportPool = pool; }

/////////////////////////////////////////////////////////////////////////////////////////////
///Access to flags
/////////////////////////////////////////////////////////////////////////////////////////////
        bool              check   (ulong x) const { return info.flags.check (x); }

/////////////////////////////////////////////////////////////////////////////////////////////
///Разбивка на тэги
/////////////////////////////////////////////////////////////////////////////////////////////
        void              OpenFrame (const char*);
        bool              CloseFrame();

  protected:
           FrameWriter& dump;
           ExportInfo&  info;

  private:  
    static Pool*        mExportPool;
           BaseExport*  mOwner;           
           int          mIndent;
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Весь экспорт начинается отсюда
/////////////////////////////////////////////////////////////////////////////////////////////
class   RootExport: public BaseExport
{
  public:
                RootExport (FrameWriter&,ExportInfo&);

/////////////////////////////////////////////////////////////////////////////////////////////
///Ассоциирование материалов
/////////////////////////////////////////////////////////////////////////////////////////////
                void    InsertMtl   (Mtl*);
                int     GetMtlID    (Mtl* m) const;
                int     GetMtlCount () const { return mMtlRing.count (); }

  private:
                void    ExportNodes (int);
                void    ExportMaterials ();

                void    Preprocess  (INode*,int&);

    static      DWORD WINAPI Progress (void*);

  private:
        typedef Dict<int,Mtl*>  MtlDict;
        typedef Ring<Mtl*>      MtlRing;

        MtlDict mMtlDict;
        MtlRing mMtlRing;
        int     mCurMtl;
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Экспортёр
/////////////////////////////////////////////////////////////////////////////////////////////
class WXFExport: public SceneExport
{
  public:
                        WXFExport  ();
        virtual         ~WXFExport ();

/////////////////////////////////////////////////////////////////////////////////////////////
///Информация об экспортёре
/////////////////////////////////////////////////////////////////////////////////////////////
        virtual int             ExtCount      ();              //Количество поддерживаемых расширений
        virtual const TCHAR *   Ext           (int n);         //N-ное расширение
        virtual const TCHAR *   LongDesc      ();              //Полное описание имени экспортёра
        virtual const TCHAR *   ShortDesc     ();              //Короткое описание имени экспортёра
        virtual const TCHAR *   AuthorName    ();              //Имя автора
        virtual const TCHAR *   CopyrightMessage();            //Copyright
        virtual const TCHAR *   OtherMessage1 ();              //??
        virtual const TCHAR *   OtherMessage2 ();              //??
        virtual unsigned int    Version       ();              //Номер версии умноженный на 100

/////////////////////////////////////////////////////////////////////////////////////////////
///Показать About
/////////////////////////////////////////////////////////////////////////////////////////////
        virtual void            ShowAbout     (HWND hWnd) ;          

/////////////////////////////////////////////////////////////////////////////////////////////
///Собственно экспорт
/////////////////////////////////////////////////////////////////////////////////////////////
        virtual int             DoExport  (const TCHAR*     name,
                                           ExpInterface*    ei,
                                           Interface*       i, 
                                           BOOL             suppressPrompts = FALSE, 
                                           DWORD            options = 0
                                          );

        virtual BOOL            SupportsOptions (int ext,DWORD options);
};

////////////////////////////////////////////////////////////////////////////////
///Оболочка для экспортера
////////////////////////////////////////////////////////////////////////////////
class ExpClassDesc: public ClassDesc 
{
  public:
        virtual int              IsPublic      ();
        virtual void *           Create        (BOOL = FALSE);
        virtual const TCHAR *    ClassName     ();
        virtual SClass_ID        SuperClassID  ();
        virtual Class_ID         ClassID       ();
        virtual const TCHAR*     Category      ();
};

extern ExpClassDesc _ExpClassDesc;

#endif