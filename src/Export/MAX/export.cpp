#include <pch.h>

#include "export.h"
#include "material.h"
#include "node.h"

int   WXFExport::ExtCount ()
{
  return 1;
}

const TCHAR* WXFExport::Ext (int n)
{
  switch(n) 
  {
    case 0: return _T("WXF");
  }

  return _T ("");   
}

const TCHAR* WXFExport::LongDesc ()
{
  return _T ("UNT Wood Exchange 3D format (WXF)");
}

const TCHAR* WXFExport::ShortDesc ()
{
  return _T ("WXF Export");
}

const TCHAR* WXFExport::AuthorName  ()
{
  return _T ("DadyCool");
} 

const TCHAR* WXFExport::CopyrightMessage ()
{
  return _T ("Copyright 2003 by UNT Games Studio");
}

const TCHAR* WXFExport::OtherMessage1 ()
{
  return _T("");
}

const TCHAR* WXFExport::OtherMessage2 ()
{
  return _T("");
}

unsigned int  WXFExport::Version ()
{
  return 100;
}

void  WXFExport::ShowAbout     (HWND hWnd) 
{
}

int   WXFExport::DoExport  
 ( const TCHAR*     name,
   ExpInterface*    eiPtr,
   Interface*       iPtr,
   BOOL             suppressPrompts, 
   DWORD            options
 )
{
  dout<<"name: "<<name<<endl;

  BaseExport::mExportPool = ::M_GetPool (); //????

  ExportInfo info;

  info.i  = iPtr;
  info.ei = eiPtr;
  info.staticFrame = 0;  

//экспорт в бинарном виде
//  info.bin.set (BIN_VERTEXES|BIN_FACES|BIN_NORMALS|BIN_TEXCOORDS|BIN_COLORS);

  Str128 dir, file;

  ParseFileName (name,dir,file,NULL);

//  FrameWriter wrt (name,info.bin.flags ()?file+".bin":NULL,0,::M_GetPool(),1024*1024*4);
  FrameWriter wrt (name,info.bin.flags ()?file+".bin":NULL,0,::M_GetPool(),1024*1024*4);

//  mInfo = new ExportInfo (interface,expInterface,0); ///0?????

  info.flags.set (EXPORT_GEOMETRY|EXPORT_MESH|
                  EXPORT_TEXTURE_COORDS|EXPORT_VERTEX_COLORS|EXPORT_NORMALS|
                  EXPORT_MATERIALS|EXPORT_ANIM_KEYS|EXPORT_HELPERS);

  RootExport root (wrt,info);

  return 1;     
}

BOOL  WXFExport::SupportsOptions (int ext,DWORD options)
{
  assert(ext == 0);       // We only support one extension
  return(options == SCENE_EXPORT_SELECTED) ? TRUE : FALSE;
}

WXFExport::WXFExport  ()
{   }

WXFExport::~WXFExport ()
{   }

DWORD WINAPI RootExport::Progress (LPVOID arg)
{
  RootExport* root = (RootExport*)arg;
  ExportInfo& info = root->info;

  info.i->ProgressUpdate ((int)((float)info.curStep/info.totalSteps*100.0f)); 

  if (info.curStep >= info.totalSteps)
    info.i->ProgressEnd ();

  return 0;
}                

void    RootExport::Preprocess  (INode* node,int& total)
{
  total++;

  InsertMtl (node->GetMtl ());

  for (int i=0;i<node->NumberOfChildren ();i++) 
    if (!check (EXPORT_SELECTED) || node->GetChildNode (i)->Selected ())
      Preprocess (node->GetChildNode (i),total);
}

void    RootExport::ExportNodes (int total)
{
  info.totalSteps = total;
  info.curStep    = 0;

  info.i->ProgressStart (_T("Export scene..."),TRUE,Progress,this); 

  INodeExport node (*this);

  info.i->ProgressEnd ();
}

void    RootExport::ExportMaterials ()
{
  if (check (EXPORT_MATERIALS))
  {
//    info.i->ProgressStart (_T("Export materials..."),TRUE,Progress,this); 

//    info.totalSteps = 

    MtlRing::iter iter  = mMtlRing;
    int           count = mMtlRing.count ();

    for (int i=0;i<count;i++,iter++)
      MaterialExport mat (*this,iter,i);

//    info.i->ProgressEnd ();
  }   
}

void    RootExport::InsertMtl   (Mtl* m)
{
  if (!m)
    return;

  int index = -1;

  if (!mMtlDict.find (m,index))
  {
    mMtlDict.insert (m,mCurMtl++);
    mMtlRing.insert (m);
  }  
}

int     RootExport::GetMtlID    (Mtl* m) const
{
  if (!m)
    return -1;

  int index = -1;

  return mMtlDict.find (m,index) ? index : -1;
}

RootExport::RootExport (FrameWriter& os,ExportInfo& i)
           : BaseExport (os,i),
             mMtlDict   (GetPool ()),
             mMtlRing   (GetPool ()),
             mCurMtl    (0)
{
  i.root  = this;
  i.totalSteps = i.curStep = 0;

  int total = 0;

  Preprocess      (i.i->GetRootNode (),total);
  ExportMaterials ();
  ExportNodes     (total);  
}
