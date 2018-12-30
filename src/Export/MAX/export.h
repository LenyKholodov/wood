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
  Interface*      i;            //����䥩� ����㯠
  ExpInterface*   ei;           //����䥩� ����㯠 ��� �ᯮ��
  TimeValue       staticFrame;  //����� ����  
  Flags32         flags;        //����� �ᯮ��
  Flags32         bin;          //��ᯮ�� � ����୮� ����
  RootExport*     root;         //��७�

//��ࠡ�⪠
  int             totalSteps, curStep;
};

//////////////////////////////////////////////////////////////////////////////////////////////
///������ �ᯮ।���⥫� ��� ��� �ᯮ��஢
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
///���।������ �����
//////////////////////////////////////////////////////////////////////////////////////////////
        void*   operator new (size_t size,Pool* pool = mExportPool) { return ::operator new (size,pool); }

  protected:
//////////////////////////////////////////////////////////////////////////////////////////////
///���।������ �����
//////////////////////////////////////////////////////////////////////////////////////////////
        Pool*   M_GetPool () const { return mExportPool; }
        Pool*   GetPool   () const { return mExportPool; }
        void    SetPool   (Pool* pool) { mExportPool = pool; }

/////////////////////////////////////////////////////////////////////////////////////////////
///Access to flags
/////////////////////////////////////////////////////////////////////////////////////////////
        bool              check   (ulong x) const { return info.flags.check (x); }

/////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ��
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
///���� �ᯮ�� ��稭����� ���
/////////////////////////////////////////////////////////////////////////////////////////////
class   RootExport: public BaseExport
{
  public:
                RootExport (FrameWriter&,ExportInfo&);

/////////////////////////////////////////////////////////////////////////////////////////////
///���樨஢���� ���ਠ���
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
///��ᯮ����
/////////////////////////////////////////////////////////////////////////////////////////////
class WXFExport: public SceneExport
{
  public:
                        WXFExport  ();
        virtual         ~WXFExport ();

/////////////////////////////////////////////////////////////////////////////////////////////
///���ଠ�� �� �ᯮ����
/////////////////////////////////////////////////////////////////////////////////////////////
        virtual int             ExtCount      ();              //������⢮ �����ন������ ���७��
        virtual const TCHAR *   Ext           (int n);         //N-��� ���७��
        virtual const TCHAR *   LongDesc      ();              //������ ���ᠭ�� ����� �ᯮ����
        virtual const TCHAR *   ShortDesc     ();              //���⪮� ���ᠭ�� ����� �ᯮ����
        virtual const TCHAR *   AuthorName    ();              //��� ����
        virtual const TCHAR *   CopyrightMessage();            //Copyright
        virtual const TCHAR *   OtherMessage1 ();              //??
        virtual const TCHAR *   OtherMessage2 ();              //??
        virtual unsigned int    Version       ();              //����� ���ᨨ 㬭������ �� 100

/////////////////////////////////////////////////////////////////////////////////////////////
///�������� About
/////////////////////////////////////////////////////////////////////////////////////////////
        virtual void            ShowAbout     (HWND hWnd) ;          

/////////////////////////////////////////////////////////////////////////////////////////////
///����⢥��� �ᯮ��
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
///�����窠 ��� �ᯮ���
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