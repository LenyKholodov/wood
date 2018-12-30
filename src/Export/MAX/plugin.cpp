#include <pch.h>
#include <kernel.h>

#include "export.h"

HINSTANCE    hInstance;
int          controlsInit   = FALSE;
StaticInit*  kernel         = NULL;
int          kernelInit     = 0;

BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) 
{
  hInstance = hinstDLL;

  if (!controlsInit) 
  {
    controlsInit = TRUE;
    InitCustomControls (hInstance);
    InitCommonControls ();
  }

  switch(fdwReason) 
  {
    case DLL_PROCESS_ATTACH:   
      if (!kernelInit++)  kernel = new StaticInit;
      else
        MessageBox (NULL,"Произошла повторная инициализация плагина. Обратитесь к разработчику","Внимание!",MB_OK);
      break;
    case DLL_THREAD_ATTACH:    break;
    case DLL_THREAD_DETACH:    break;
    case DLL_PROCESS_DETACH:   
      if (!--kernelInit) 
      {
        if (kernel) delete kernel;
        kernel = NULL;
      }  
      else
        MessageBox (NULL,"Произошла повторная деинициализация плагина. Обратитесь к разработчику","Внимание!",MB_OK);
      break;
  }

  return TRUE;
}

__declspec (dllexport) 
const TCHAR * LibDescription() 
{ 
  return _T ("WXF file exporter");  //Wood Exchange Format
} 

__declspec (dllexport) 
int LibNumberClasses() 
{ 
  return 1; 
}

__declspec (dllexport) 
ClassDesc* LibClassDesc(int i) 
{
  switch(i) 
  {
    case 0:  return &_ExpClassDesc;  //?????
    default: return 0;          
  }
}

__declspec (dllexport) 
ULONG LibVersion () 
{ 
  return VERSION_3DSMAX; 
}

// Let the plug-in register itself for deferred loading
__declspec (dllexport) 
ULONG CanAutoDefer()
{
  return 1;
}

__declspec( dllexport ) 
TCHAR* ClassDesc::GetRsrcString (INT_PTR)
{
  return _T(""); //GetString
}

///////////////////////////Оболочка/////////////////////////////////////////////

ExpClassDesc _ExpClassDesc;

int             ExpClassDesc::IsPublic () 
{ 
  return 1; 
}

void *          ExpClassDesc::Create(BOOL) 
{ 
  return new WXFExport;
}

const TCHAR *   ExpClassDesc::ClassName () 
{ 
  return _T ("3D Studio"); 
}

SClass_ID       ExpClassDesc::SuperClassID () 
{ 
  return SCENE_EXPORT_CLASS_ID; 
}

Class_ID        ExpClassDesc::ClassID () 
{                                     
  return Class_ID (0x155567fb,0x3c9b4d72);
}

const TCHAR*    ExpClassDesc::Category () 
{ 
  return _T ("Scene Export");//GetString(IDS_TH_SCENEEXPORT);  
}
