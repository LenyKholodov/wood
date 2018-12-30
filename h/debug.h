#ifndef __DEBUG_SERVICE_FACE__
#define __DEBUG_SERVICE_FACE__

#include <service\debug.h>

extern accessptr_t<DebugService>    dbgService;

#define    D_BindMapFile dbgService->BindMap
#define    D_Dump        dbgService->Dump

#define    D_TestExc(X)      __except (dbgService->Dump (_exception_info ())) { \
                                   dout<<X<<endl; \
                                    }
#define    D_TestExcRet(X,Y) __except (dbgService->Dump (_exception_info ())) { \
                                   dout<<X<<endl; \
                                   return Y; \
                                    }

DebugService*  D_Load (const char* map_file = NULL,
                       Pool*                = ::M_GetPool ()
                      );
void       D_Done (DebugService* = dbgService);

/////////////////////////////////////////////////////////////////////////////////////////////
///Информация по адресу
/////////////////////////////////////////////////////////////////////////////////////////////
void*      D_FindLoadAddr  (istream& mapStream); //ret: 0 - no find
bool       D_FindFuncName (istream& mapStream,
                           void*    addr,
                           char*    funcName,
                           char*    fileName
                          );
int        D_FindLine     (istream& mapStream,
                           void*    addr,
                           void*    load_addr,
                           char*    fileName,
                           char*    libName, //only out
                           ulong&   bytes
                          ); //ret: -1 - no find


#endif