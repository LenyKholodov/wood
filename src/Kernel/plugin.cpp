#include <pch.h>
#include <kernel\plugin.h>
#include <kernel\thread.h>
#include <debug.h>

void    Plugin::LoadPlugins  (Thread* thread,const char* _DirName)
{
  D_Try
  {
    WIN32_FIND_DATA  data;    
    char             name [MAX_PATH];
    BOOL             flag = TRUE;

    strcpy (name,_DirName);
    strcat (name,"*.dll");   

    for (HANDLE handle = FindFirstFile (name,&data);handle!=INVALID_HANDLE_VALUE && flag;
         flag          = FindNextFile  (handle,&data))
    {
      HMODULE lib = LoadLibrary (data.cFileName);

      if (lib)
        thread->include ( new (thread) Plugin (thread,lib) );
      else  
      {
        lib = LoadLibraryEx (data.cFileName,NULL,DONT_RESOLVE_DLL_REFERENCES);
   
        if (lib)
          thread->include ( new (thread) Plugin (thread,lib) );
      }  
    }

    FindClose (handle);
  }
  D_Exc ("Plugin: Error at LoadPlugins");
}

void*   Plugin::GetProcAddr  (const char* func)
{
  return (void*)::GetProcAddress (mHandle,func);
}

Plugin::Plugin  (Thread* thread,HMODULE handle)
       : Service (thread),
         mHandle (handle),
         mInit   (NULL), mDone (NULL),
         mType   (SERVICE_PLUGIN)
{    
//  D_Try
//  {
    mName [0] = 0;
    mInit     = (executer_t)GetProcAddr ("PluginInit");
    mDone     = (executer_t)GetProcAddr ("PluginDone");

    if (mInit)
      (*mInit)(thread);
//  }
//  D_Exc ("Plugin: Error at construct");
}

Plugin::~Plugin ()
{    
//  D_Try
//  {
    if (mDone)
      (*mDone)(thread);

    ::FreeLibrary (mHandle);
//  }
//  D_Exc ("Plugin: Error at destruct");  
}
